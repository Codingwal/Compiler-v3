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
    };
    struct funcData
    {
        string funcName;
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
            {"int", {.typeName = "int"}},
            {"void", {.typeName = "void"}},
    };
    map<string, funcData> funcs =
        {
            {"exit", {.funcName = "exit", .returnType = &types["void"]}},
    };
    vector<varData> vars;

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

    void generateTerm(nodeTerm term)
    {
        if (std::holds_alternative<string>(term.term))
        {
            push("QWORD " + get<string>(term.term));
        }
        else if (std::holds_alternative<ident>(term.term))
        {
            int varStackPos = getVar(get<ident>(term.term).ident)->stackPos;
            push(string("QWORD [rsp + ") + to_string(varStackPos - stackPointer) + "]");
        }
        else
        {
            errorHandler::error("WTF WHERE IS MY TERM");
        }
    }

    void generateExpr(nodeExpr expr)
    {
        if (holds_alternative<nodeTerm>(expr.expr))
        {
            generateTerm(get<nodeTerm>(expr.expr));
        }
        else if (holds_alternative<nodeBinExpr>(expr.expr))
        {
            nodeBinExpr binExpr = get<nodeBinExpr>(expr.expr);

            generateExpr(*binExpr.rhs);
            generateExpr(*binExpr.lhs);

            // binExpr
            pop("rax");
            pop("rbx");

            switch (binExpr.op)
            {
            case TokenType::plus:
                output << "    add rax, rbx\n";
                break;
            case TokenType::minus:
                output << "    sub rax, rbx\n";
                break;
            case TokenType::star:
                output << "    imul rbx\n";
                break;
            case TokenType::backslash:
                output << "    xor rdx, rdx\n";  
                output << "    div rbx\n";
                break;
            default:
                errorHandler::error("Invalid operator");
            }

            push("rax");
        }
        else
        {
            errorHandler::error("WTF");
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
        generateExpr(def.expr);

        createVar(def.varName, def.varType, stackPointer);

        return;
    }
    void generateVarAssign(nodeVarAssign assign)
    {
        generateExpr(assign.expr);

        varData *varData = getVar(assign.varName);
        pop("rax");

        int varOffset = varData->stackPos - stackPointer;
        output << "    mov [rsp + " << varOffset << "], rax\n";
    }
    void generateFuncCall(nodeFuncCall call)
    {
        if (funcs.count(call.funcName) == 0)
        {
            errorHandler::error("Function " + call.funcName + " was not declared.");
        }
        for (nodeExpr expr : call.params)
        {
            generateExpr(expr);
        }
        output << "    call " << call.funcName << endl;
    }
    void generateScope(nodeScope scope)
    {
        for (auto stmt : scope.body)
        {
            switch ((int)stmt.index())
            {
            case 0:
                generateVarDecl(get<nodeVarDecl>(stmt));
                break;
            case 1:
                generateVarDef(get<nodeVarDef>(stmt));
                break;
            case 2:
                generateVarAssign(get<nodeVarAssign>(stmt));
                break;
            case 3:
                generateFuncCall(get<nodeFuncCall>(stmt));
                break;
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