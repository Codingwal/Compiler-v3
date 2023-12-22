#include <iostream>
#include <sstream>
#include <vector>
#include <map>
using namespace std;

#include "errorHandler.cpp"
#include "parser.cpp"
using namespace parser;

namespace generator
{
    struct typeData
    {
        string typeName;
        size_t size;
    };
    struct funcData
    {
        string funcName;
        vector<typeData *> params;
        typeData *returnType;
    };
    struct varData
    {
        string varName;
        typeData *varType;
        int stackPos;
    };

    map<string, typeData> types =
        {
            {"byte4", {.typeName = "byte4", .size = 4}},
            {"byte1", {.typeName = "byte1", .size = 1}},
            {"void", {.typeName = "void"}},
    };
    map<string, funcData> funcs =
        {
            {"exit", {.funcName = "exit", .params = {&types["byte4"]}, .returnType = &types["void"]}},
    };
    vector<varData> vars;

    int ifStmtCount = 0;

    int stackPointer;

    stringstream output;

    void push(string reg)
    {
        output << "    push " << reg << endl;
        stackPointer -= 8;
    }
    void pop(string reg)
    {
        output << "    pop " << reg << endl;
        stackPointer += 8;
    }
    typeData *getType(string varType)
    {
        if (types.count(varType) == 0)
        {
            errorHandler::error("Type '" + varType + "' was not defined.");
        }
        return &types[varType];
    }
    varData *getVar(string varName)
    {
        for (int i = 0; i < vars.size(); i++)
        {
            varData *var = &vars[i];
            if (var->varName == varName)
            {
                return var;
            }
        }
        errorHandler::error("Variable '" + varName + "' was not defined in this scope.");
        return NULL;
    }
    void createVar(string varName, string varType, int stackPos)
    {
        vars.push_back({.varName = varName, .varType = getType(varType), .stackPos = stackPos});
    }

    void compTypes(typeData *a, typeData *b)
    {
        if (a != b)
        {
            errorHandler::error("Types '" + a->typeName + "' and '" + b->typeName + "' do not match.");
        }
    }

    typeData *generateExpr(nodeExpr expr);

    typeData *generateTerm(nodeTerm term)
    {
        if (std::holds_alternative<parser::int_lit>(term.term))
        {
            push("QWORD " + get<parser::int_lit>(term.term).value);
            return getType("byte4");
        }
        if (std::holds_alternative<parser::bool_lit>(term.term))
        {
            string value = get<parser::bool_lit>(term.term).value;
            if (value == "true")
            {
                push("QWORD -1");
            }
            else
            {
                push("QWORD 0");
            }
            return getType("byte1");
        }
        else if (std::holds_alternative<ident>(term.term))
        {
            varData *data = getVar(get<ident>(term.term).ident);
            int varStackPos = data->stackPos;
            push(string("QWORD [rsp + ") + to_string(varStackPos - stackPointer) + "]");
            return data->varType;
        }
        else if (std::holds_alternative<nodeExpr *>(term.term))
        {
            typeData *type = generateExpr(*get<nodeExpr *>(term.term));
            free(get<nodeExpr *>(term.term));
            return type;
        }
        else
        {
            throw;
        }
    }

    typeData *generateExpr(nodeExpr expr)
    {
        if (holds_alternative<nodeTerm>(expr.expr))
        {
            return generateTerm(get<nodeTerm>(expr.expr));
        }
        else if (holds_alternative<nodeBinExpr>(expr.expr))
        {
            typeData *type;

            nodeBinExpr binExpr = get<nodeBinExpr>(expr.expr);

            typeData *lhsType = generateExpr(*binExpr.rhs);
            typeData *rhsType = generateExpr(*binExpr.lhs);

            free(binExpr.rhs);
            free(binExpr.lhs);

            // binExpr
            pop("rax");
            pop("rbx");

            switch (binExpr.op)
            {
            case TokenType::plus:
                output << "    add eax, ebx\n";

                compTypes(lhsType, getType("byte4"));
                compTypes(rhsType, getType("byte4"));
                type = getType("byte4");
                break;
            case TokenType::minus:
                output << "    sub eax, ebx\n";

                type = getType("byte4");
                break;
            case TokenType::star:
                output << "    imul ebx\n";

                compTypes(lhsType, getType("byte4"));
                compTypes(rhsType, getType("byte4"));
                type = getType("byte4");
                break;
            case TokenType::backslash:
                output << "    xor rdx, rdx\n";
                output << "    div ebx\n";

                compTypes(lhsType, getType("byte4"));
                compTypes(rhsType, getType("byte4"));
                type = getType("byte4");
                break;
            case TokenType::is_equal:
                output << "    cmp eax, ebx\n";
                output << "    setne al\n";
                output << "    dec al\n";

                type = getType("byte1");
                break;
            case TokenType::less_than:
                output << "    cmp eax, ebx\n";
                output << "    setge al\n";
                output << "    dec al\n";

                compTypes(lhsType, getType("byte4"));
                compTypes(rhsType, getType("byte4"));
                type = getType("byte1");
                break;
            case TokenType::greater_than:
                output << "    cmp eax, ebx\n";
                output << "    setle al\n";
                output << "    dec al\n";

                compTypes(lhsType, getType("byte4"));
                compTypes(rhsType, getType("byte4"));
                type = getType("byte1");
                break;
            case TokenType::logical_or:
                output << "    or al, bl\n";

                compTypes(lhsType, getType("byte1"));
                compTypes(rhsType, getType("byte1"));
                type = getType("byte1");
                break;
            case TokenType::logical_and:
                output << "    and al, bl\n";

                compTypes(lhsType, getType("byte1"));
                compTypes(rhsType, getType("byte1"));
                type = getType("byte1");
                break;
            case TokenType::logical_xor:
                output << "    xor al, bl\n";

                compTypes(lhsType, getType("byte1"));
                compTypes(rhsType, getType("byte1"));
                type = getType("byte1");
                break;
            default:
                errorHandler::error("Invalid operator " + lexer::tokenNames.at(binExpr.op));
            }

            push("rax");

            return type;
        }
        else
        {
            errorHandler::error("WTF");
            return NULL;
        }
    }
    void generateVarDecl(nodeVarDecl decl)
    {
        // Reserve space for var
        push("0");

        createVar(decl.varName, decl.varType, stackPointer);

        return;
    }
    void generateVarDef(nodeVarDef def)
    {
        compTypes(generateExpr(def.expr), getType(def.varType));

        createVar(def.varName, def.varType, stackPointer);

        return;
    }
    void generateVarAssign(nodeVarAssign assign)
    {
        varData *varData = getVar(assign.varName);

        compTypes(generateExpr(assign.expr), varData->varType);

        pop("rax");

        int varOffset = varData->stackPos - stackPointer;
        output << "    mov [rsp + " << varOffset << "], rax\n";
    }
    typeData *generateFuncCall(nodeFuncCall call)
    {
        if (funcs.count(call.funcName) == 0)
        {
            errorHandler::error("Function " + call.funcName + " was not declared.");
        }
        funcData *data = &funcs.at(call.funcName);

        if (call.params.size() < data->params.size())
        {
            errorHandler::error("Too few arguments in function call '" + call.funcName + "'.");
        }
        for (int i = 0; i < data->params.size(); i++)
        {
            compTypes(generateExpr(call.params.at(i)), data->params[i]);
        }

        output << "    call " << call.funcName << endl;

        return data->returnType;
    }

    void generateScope(nodeScope scope);

    void generateStmtIf(nodeStmtIf stmt)
    {
        compTypes(generateExpr(stmt.expr), getType("byte1"));

        int ifStmtNum = ifStmtCount;

        pop("rax");
        output << "    or al, al\n";
        output << "    jz if_stmt_end" << ifStmtNum << "\n";
        generateScope(*stmt.scope);
        output << "if_stmt_end" << ifStmtNum << ":\n";
    }
    void generateScope(nodeScope scope)
    {
        for (auto stmt : scope.body)
        {
            if (holds_alternative<nodeVarDecl>(stmt))
            {
                generateVarDecl(get<nodeVarDecl>(stmt));
            }
            else if (holds_alternative<nodeVarDef>(stmt))
            {
                generateVarDef(get<nodeVarDef>(stmt));
            }
            else if (holds_alternative<nodeVarAssign>(stmt))
            {
                generateVarAssign(get<nodeVarAssign>(stmt));
            }
            else if (holds_alternative<nodeFuncCall>(stmt))
            {
                generateFuncCall(get<nodeFuncCall>(stmt));
            }
            else if (holds_alternative<nodeStmtIf>(stmt))
            {
                generateStmtIf(get<nodeStmtIf>(stmt));
            }
            else
            {
                throw;
            }
        }
    }
    void generateFuncDef(nodeFuncDef def)
    {
        funcData data = {.funcName = def.funcName, .returnType = getType(def.returnType)};
        funcs.insert({def.funcName, data});
        output << def.funcName << ":\n";
        for (int i = 0; i < def.params.size(); i++)
        {
            nodeVarDecl param = def.params[i];
            createVar(param.varName, param.varType, stackPointer + (def.params.size() - i) * 8);
        }

        generateScope(def.body);

        for (int i = 0; i < def.params.size(); i++)
        {
            vars.pop_back();
        }
    }
    string generate(nodeProg prog)
    {
        for (auto stmt : prog.stmts)
        {
            switch ((int)stmt.index())
            {
            case 0:
                generateFuncDef(get<nodeFuncDef>(stmt));
                break;
            default:
                throw;
            }
        }

        return output.str();
    }
}