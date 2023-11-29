#include <vector>
#include <variant>
#include <string>

using namespace std;

namespace parser
{   
    struct nodeExpr
    {
        string int_lit;
    };
    struct nodeVarDecl
    {
        string varType;
        string varName;
    };
    struct nodeVarDef
    {
        string varType;
        string varName;
        nodeExpr* expr;;
    };
    struct nodeVarAssign
    {
        string varName;
        nodeExpr* expr;;
    };
    struct nodeFuncCall
    {
        string funcName;
        vector<nodeExpr*> params;
    };
    struct nodeScope
    {
        vector<variant<nodeVarDecl*, nodeVarDef*, nodeVarAssign*, nodeFuncCall*>> body;
    };
    
    struct nodeStructDef
    {

    };
    struct nodeFuncDef
    {
        string returnType;
        string funcName;
        vector<nodeVarDecl*> params;
        nodeScope* body;
    };
    struct nodeProg
    {
        vector<variant<nodeFuncDef*, nodeStructDef*>> stmts;
    };
}