#include <vector>
#include <variant>
#include <string>
using namespace std;

#include "lexer.cpp"

namespace parser
{
    struct int_lit
    {
        string value;
    };
    struct bool_lit
    {
        string value;
    };

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
    struct nodeFuncCall;
    struct nodeTerm
    {
        variant<int_lit, bool_lit, nodeFuncCall *, ident, nodeExpr *> term;
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

    struct nodeStmtReturn
    {
        nodeExpr expr;
    };
    struct nodeStmtIf
    {
        nodeExpr expr;
        nodeScope *scope;
    };
    struct nodeStmtFor
    {
        nodeVarDef def;
        nodeExpr expr;
        nodeVarAssign assign;
        nodeScope *scope;
    };
    struct nodeStmtWhile
    {
        nodeExpr expr;
        nodeScope *scope;
    };

    struct nodeScope
    {
        vector<variant<nodeVarDecl, nodeVarDef, nodeVarAssign, nodeFuncCall, nodeStmtReturn, nodeStmtIf, nodeStmtFor, nodeStmtWhile>> body;
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