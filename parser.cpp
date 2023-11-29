#pragma once

#include <iostream>
#include <vector>
#include <optional>
using namespace std;

#include "nodes.cpp"
#include "lexer.cpp"
using namespace lexer;

namespace parser
{
    vector<Token> *tokens;
    int index;

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
            std::cout << "[ERROR]: Unexpected end of file" << endl;
            throw;
        }
        return tokens->at(index + ahead);
    }

    Token consume()
    {
        if (!tokensLeft())
        {
            cout << "[ERROR]: Unexpected end of file" << endl;
            throw;
        }
        index++;
        return tokens->at(index);
    }

    void tryConsume(TokenType type)
    {
        TokenType foundToken = consume().type;
        if (foundToken != type)
        {
            cout << "Expected token of type '" << lexer::tokenNames[type] << "', found token of type '" << lexer::tokenNames[foundToken] << "'" << endl;
            throw;
        }
    }

    nodeExpr parseExpr()
    {
        nodeExpr expr;
        return expr;
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
                consume();
                scope.body.push_back(&decl);
            }
            else if (peek().type == TokenType::custom && peek(2).type == TokenType::custom && peek(3).type == TokenType::equal)
            {
                // nodeVarDef
                nodeVarDef def;
                def.varType = consume().value;
                def.varName = consume().value;
                consume();
                nodeExpr expr = parseExpr();
                def.expr = &expr;
                tryConsume(TokenType::semicolon);
                scope.body.push_back(&def);
            }
            else if (peek().type == TokenType::custom && peek(2).type == TokenType::equal)
            {
                // nodeVarAssign
            }
            else if (peek().type == TokenType::custom && peek(2).type == TokenType::open_paren)
            {
                // nodeFuncCall
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
            funcDef.params.push_back(&decl);

            if (peek().type == TokenType::close_paren)
            {   
                break;
            };
            tryConsume(TokenType::comma);
        };
        consume();
        nodeScope scope = parseScope();
        funcDef.body = &scope;

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
                prog.stmts.push_back(parseFuncDef());sadsd
            }
            else
            {
                cout << "[ERROR]: Invalid statement in prog at token " << index << endl;
                exit(EXIT_FAILURE);
            }
        }

        return prog;
    }
}