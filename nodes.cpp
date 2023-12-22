#include <vector>
#include <variant>
#include <string>
using namespace std;

#include "lexer.cpp"

namespace parser
{
    struct nodeExpr;
    struct nodeBinExpr
    {
        lexer::TokenType op;
        nodeExpr *lhs;
        nodeExpr *rhs;
    };
    struct ident
    {
        string ident;
    };
    struct nodeTerm
    {
        variant<string, ident, nodeExpr *> term;
    };
    struct nodeExpr
    {
        variant<nodeTerm, nodeBinExpr> expr;
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
        nodeExpr expr;
        ;
    };
    struct nodeVarAssign
    {
        string varName;
        nodeExpr expr;
        ;
    };
    struct nodeFuncCall
    {
        string funcName;
        vector<nodeExpr> params;
    };

    struct nodeScope;

    struct nodeStmtIf
    {
        nodeExpr expr;
        nodeScope *scope;
    };
    struct nodeScope
    {
        vector<variant<nodeVarDecl, nodeVarDef, nodeVarAssign, nodeFuncCall, nodeStmtIf>> body;
    };

    struct nodeStructDef
    {
    };
    struct nodeFuncDef
    {
        string returnType;
        string funcName;
        vector<nodeVarDecl> params;
        nodeScope body;
    };

    struct nodeProg
    {
        vector<variant<nodeFuncDef, nodeStructDef>> stmts;
    };
}