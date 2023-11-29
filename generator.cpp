#include <iostream>
using namespace std;

#include "parser.cpp"
using namespace parser;

namespace generator
{
    void generateScope(nodeScope *scope)
    {
        cout << "scope" << endl;
    }
    void generateFuncDef(nodeFuncDef *def)
    {
        generateScope(def->body);

        cout << "funcDef: name: " << def->funcName << ", returnType: " << def->returnType << endl;
    }
    void generate(nodeProg *prog)
    {
        cout << prog->stmts.size() << endl;
        for (auto stmt : prog->stmts)
        {
            cout << stmt.index() << endl;
            switch ((int)stmt.index())
            {
            case 0:
                generateFuncDef(get<nodeFuncDef*>(stmt));
                break;
            default:
                throw;
            }
        }
    }
}