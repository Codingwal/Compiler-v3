#include <iostream>
using namespace std;

#include "parser.cpp"
using namespace parser;

namespace generator
{
    void generateExpr(nodeExpr expr)
    {
        cout << "expr: " << expr.int_lit << endl;
    }
    void generateVarDecl(nodeVarDecl decl)
    {
        cout << "varDecl: name: " << decl.varName << ", type: " << decl.varType << endl;
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