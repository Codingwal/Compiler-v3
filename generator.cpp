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

    map<string, typeData> types;
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
    void generateExpr(nodeExpr expr)
    {
        cout << "expr: " << expr.int_lit << endl;
    }
    void generateVarDecl(nodeVarDecl decl)
    {
        // Reserve space for var
        push("0");
        varData var;
        var.varName = decl.varName;
        var.varType = getType(decl.varType);
        var.stackPos = stackPointer;
        vars.push_back(var);
        return;
    }
    void generateVarDef(nodeVarDef def)
    {
        generateExpr(def.expr);
        cout << "varDef: name: " << def.varName << ", type: " << def.varType << endl;
    }
    void generateVarAssign(nodeVarAssign assign)
    {
        generateExpr(assign.expr);
        cout << "varAssign: name: " << assign.varName << endl;
    }
    void generateFuncCall(nodeFuncCall call)
    {
        cout << "funcCall: name: " << call.funcName << endl;
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
        cout << "scope" << endl;
    }
    void generateFuncDef(nodeFuncDef def)
    {
        generateScope(def.body);

        cout << "funcDef: name: " << def.funcName << ", returnType: " << def.returnType << endl;
    }
    void generate(nodeProg prog)
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
    }
}