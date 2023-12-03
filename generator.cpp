#include <iostream>
#include <sstream>
#include <vector>
#include <map>
using namespace std;

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
    map<string, funcData> funcs;
    vector<varData> vars;

    int stackPointer;

    stringstream output;

    void push(string reg)
    {
        output << "    push " << reg << endl;
        stackPointer--;
    }
    void pop(string reg)
    {
        output << "    pop " << reg << endl;
        stackPointer++;
    }
    typeData* getType(string varType)
    {
        if (types.count(varType) == 0)
        {
            cout << "[ERROR]: Type '" << varType << "' was not defined." << endl;
            throw;
        }
        return &types[varType];
    }
    varData* getVar(string varName)
    {
        for (int i = 0; i < vars.size(); i++)
        {
            varData* var = &vars[i];
            if (var->varName == varName)
            {
                return var;
            }
        }
        cout << "[ERROR]: Variable '" << varName << "' was not defined in this scope." << endl;
        throw;
    }
    void createVar(string varName, string varType)
    {
        vars.push_back({.varName = varName, .varType = getType(varType), .stackPos = stackPointer});
    }
    void generateExpr(nodeExpr expr)
    {
        output << "    ; expr\n";
    }
    void generateVarDecl(nodeVarDecl decl)
    {
        // Reserve space for var
        output << "    ; varDecl\n";
        push("0");

        createVar(decl.varName, decl.varType);

        return;
    }
    void generateVarDef(nodeVarDef def)
    {
        output << "    ; varDef\n";
        generateExpr(def.expr);

        createVar(def.varName, def.varType);

        return;
    }
    void generateVarAssign(nodeVarAssign assign)
    {
        output << "    ; varAssign\n";
        generateExpr(assign.expr);

        varData *varData = getVar(assign.varName);
        pop("rax");

        int varOffset = varData->stackPos - stackPointer;
        output << "    mov [rsp + " << varOffset << "], rax\n";
    }
    void generateFuncCall(nodeFuncCall call)
    {
        output << "    ; funcCall\n";
    }
    void generateScope(nodeScope scope)
    {
        output << "    ; scope\n";
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
        output << "    ; funcDef\n";
        generateScope(def.body);
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