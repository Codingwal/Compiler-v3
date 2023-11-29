#include <iostream>
using namespace std;

#include "parser.cpp"
using namespace parser;

namespace generator
{
    void generateVarDecl(nodeVarDecl decl)
    {
        cout << "varDecl" << endl;
    }
    void generateScope(nodeScope scope)
    {
        for (auto stmt : scope.body)
        {
            switch ((int)stmt.index())
            {
            case 0:
                generateVarDecl(get<nodeVarDecl>(stmt));
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