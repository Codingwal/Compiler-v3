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

    void generateExpr(nodeExpr expr)
    {
        output << "    ; expr\n";
        switch (static_cast<int>(expr.expr.index()))
        {
        case 0:
            // int_lit
            push("qword " + get<string>(expr.expr));
            break;
        case 1:
        {
            // ident
            int varStackPos = getVar(get<ident>(expr.expr).ident)->stackPos;
            push(string("qword [rsp + ") + to_string(varStackPos - stackPointer) + "]");
            break;
        }
        case 2:
            // bin_expr
            errorHandler::error("Not implemented!");
            break;
        default:
            break;
        }
    }
    void generateVarDecl(nodeVarDecl decl)
    {
        // Reserve space for var
        output << "    ; varDecl\n";
        push("0");

        createVar(decl.varName, decl.varType, stackPointer);

        return;
    }
    void generateVarDef(nodeVarDef def)
    {
        generateExpr(def.expr);

        output << "    ; varDef\n";

        createVar(def.varName, def.varType, stackPointer);

        return;
    }
    void generateVarAssign(nodeVarAssign assign)
    {
        generateExpr(assign.expr);

        output << "    ; varAssign\n";

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
        output << "    ; funcCall\n";
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
        output << "\n; funcDef\n";
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