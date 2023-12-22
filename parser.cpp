#pragma once

#include <iostream>
#include <vector>
#include <optional>
#include <map>
using namespace std;

#include "errorHandler.cpp"
#include "nodes.cpp"
#include "lexer.cpp"
using namespace lexer;

namespace parser
{
    vector<Token> *tokens;
    int index;

    map<TokenType, int> precedences =
        {
            {TokenType::is_equal, 2},
            {TokenType::less_than, 2},
            {TokenType::greater_than, 2},

            {TokenType::plus, 3},
            {TokenType::minus, 3},

            {TokenType::star, 4},
            {TokenType::backslash, 4},
    };

    bool tokensLeft(int ahead = 1)
    {
        return index + ahead < tokens->size();
    }

    optional<Token> tryPeek()
    {
        if (tokensLeft())
        {
            return tokens->at(index + 1);
        }
        else
        {
            return {};
        }
    }

    Token peek(int ahead = 1)
    {
        if (!tokensLeft())
        {
            errorHandler::error("Unexpected end of file.");
        }
        return tokens->at(index + ahead);
    }

    Token consume()
    {
        if (!tokensLeft())
        {
            errorHandler::error("Unexpected end of file.");
        }
        index++;
        return tokens->at(index);
    }

    Token tryConsume(TokenType type)
    {
        Token foundToken = consume();
        if (foundToken.type != type)
        {
            errorHandler::error("Expected token of type '" + lexer::tokenNames[type] + "', found token of type '" + lexer::tokenNames[foundToken.type] + "' at " + to_string(index));
        }
        return foundToken;
    }

    int getPrecedence(TokenType op)
    {
        if (precedences.count(op) == 0)
        {
            return -1;
        }
        int prec = precedences[op];
        return prec;
    }

    nodeExpr parseExpr(int minPrec = 0);

    nodeTerm parseTerm()
    {
        nodeTerm term;
        if (peek().type == TokenType::int_lit)
        {
            term.term = int_lit{tryConsume(TokenType::int_lit).value};
        }
        else if (peek().type == TokenType::bool_lit)
        {
            term.term = bool_lit{tryConsume(TokenType::bool_lit).value};
        }
        else if (peek().type == TokenType::custom)
        {
            term.term = ident{.ident = tryConsume(TokenType::custom).value};
        }
        else if (peek().type == TokenType::open_paren)
        {
            tryConsume(TokenType::open_paren);
            term.term = new nodeExpr(parseExpr());
            tryConsume(TokenType::close_paren);
        }
        else
        {
            errorHandler::error(string("Invalid term at token ") + to_string(index) + ", didn't expect token type " + lexer::tokenNames.at(peek().type));
        }
        return term;
    }

    // default value is in function declaration
    nodeExpr parseExpr(int minPrec)
    {
        nodeExpr lhs = nodeExpr{.expr = parseTerm()};

        while (true)
        {
            nodeBinExpr binExpr;

            int prec = getPrecedence(peek().type);

            if (prec < minPrec)
                break;

            binExpr.op = consume().type;
            nodeExpr *rhs = new nodeExpr(parseExpr(prec + 1));
            binExpr.rhs = rhs;
            binExpr.lhs = new nodeExpr(lhs);

            lhs.expr = binExpr;
        }
        return lhs;
    }

    nodeScope parseScope()
    {
        nodeScope scope = nodeScope();

        tryConsume(TokenType::open_curly);
        while (peek().type != TokenType::close_curly)
        {
            // parse stmt
            if (peek().type == TokenType::custom && peek(2).type == TokenType::custom && peek(3).type == TokenType::semicolon)
            {
                // nodeVarDecl
                nodeVarDecl decl;
                decl.varType = consume().value;
                decl.varName = consume().value;
                tryConsume(TokenType::semicolon);
                scope.body.push_back(decl);
            }
            else if (peek().type == TokenType::custom && peek(2).type == TokenType::custom && peek(3).type == TokenType::equal)
            {
                // nodeVarDef
                nodeVarDef def;
                def.varType = consume().value;
                def.varName = consume().value;
                tryConsume(TokenType::equal);
                def.expr = parseExpr();
                tryConsume(TokenType::semicolon);
                scope.body.push_back(def);
            }
            else if (peek().type == TokenType::custom && peek(2).type == TokenType::equal)
            {
                // nodeVarAssign
                nodeVarAssign assign;
                assign.varName = consume().value;
                tryConsume(TokenType::equal);
                assign.expr = parseExpr();
                tryConsume(TokenType::semicolon);
                scope.body.push_back(assign);
            }
            else if (peek().type == TokenType::custom && peek(2).type == TokenType::open_paren)
            {
                // nodeFuncCall
                nodeFuncCall call;
                call.funcName = consume().value;
                tryConsume(TokenType::open_paren);

                // Parse parameters
                while (peek().type != TokenType::close_paren)
                {
                    call.params.push_back(parseExpr());

                    if (peek().type == TokenType::close_paren)
                    {
                        break;
                    };
                    tryConsume(TokenType::comma);
                };

                tryConsume(TokenType::close_paren);
                tryConsume(TokenType::semicolon);
                scope.body.push_back(call);
            }
            else if (peek().type == TokenType::_if)
            {
                nodeStmtIf stmt;
                tryConsume(TokenType::_if);
                tryConsume(TokenType::open_paren);
                stmt.expr = parseExpr();
                tryConsume(close_paren);
                stmt.scope = new nodeScope(parseScope());
                scope.body.push_back(stmt);
            }
            else
            {
                errorHandler::error("Invalid statement in scope at token " + to_string(index + 1));
            }
            // scope.body.push_back(stmt);
        }
        consume();

        return scope;
    }

    nodeFuncDef parseFuncDef()
    {
        nodeFuncDef funcDef;
        funcDef.returnType = consume().value;
        funcDef.funcName = consume().value;
        tryConsume(TokenType::open_paren);

        while (peek().type != TokenType::close_paren)
        {
            string paramType = consume().value;
            string paramName = consume().value;

            nodeVarDecl decl;
            decl.varType = paramType;
            decl.varName = paramName;
            funcDef.params.push_back(decl);

            if (peek().type == TokenType::close_paren)
            {
                break;
            };
            tryConsume(TokenType::comma);
        };
        consume();
        funcDef.body = parseScope();

        return funcDef;
    }

    nodeProg parse(vector<Token> *token_list)
    {
        tokens = token_list;
        index = -1;

        nodeProg prog = nodeProg();

        while (tokensLeft())
        {
            if (peek().type == TokenType::_struct)
            {
            }
            else if (peek(3).type == TokenType::open_paren)
            {
                prog.stmts.push_back(parseFuncDef());
            }
            else
            {
                errorHandler::error("Invalid statement in prog at token " + index);
            }
        }

        return prog;
    }
}