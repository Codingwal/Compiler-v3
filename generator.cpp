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
        size_t size;
    };
    struct funcData
    {
        string funcName;
        vector<typeData *> params;
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
            {"byte4", {.typeName = "byte4", .size = 8}},
            {"byte1", {.typeName = "byte1", .size = 8}},
            {"void", {.typeName = "void"}},
    };
    map<string, funcData> funcs =
        {
            {"exit", {.funcName = "exit", .params = {&types["byte4"]}, .returnType = &types["void"]}},
    };
    vector<varData> vars;

    int ifStmtCount = 0;
    int forLoopCount = 0;
    int whileLoopCount = 0;

    int stackPointer;

    stringstream output;

    string getReg(string reg, int size)
    {
        switch (size)
        {
        case 1:
            return reg + 'l';
        case 2:
            return reg + 'x';
        case 4:
            return "e" + reg + 'x';
        case 8:
            return "r" + reg + 'x';
        default:
            throw;
        }
    }
    void push(string reg, int size)
    {
        output << "    push " << reg << endl;
        stackPointer -= size;
    }
    void push(char reg, int size)
    {
        push(getReg({reg}, size), size);
    }
    void pop(string reg, int size)
    {
        output << "    pop " << reg << endl;
        stackPointer += size;
    }
    void pop(char reg, int size)
    {
        pop(getReg({reg}, size), size);
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

    void compTypes(typeData *a, typeData *b)
    {
        if (a != b)
        {
            errorHandler::error("Types '" + a->typeName + "' and '" + b->typeName + "' do not match.");
        }
    }

    string getWord(int size)
    {
        switch (size)
        {
        case 1:
            return "BYTE";
        case 2:
            return "WORD";
        case 4:
            return "DWORD";
        case 8:
            return "QWORD";
        default:
            throw;
        }
    }

    typeData *generateExpr(nodeExpr expr);
    typeData *generateFuncCall(nodeFuncCall call, bool returnValue);

    typeData *generateTerm(nodeTerm term)
    {
        if (std::holds_alternative<parser::int_lit>(term.term))
        {
            push("QWORD " + get<parser::int_lit>(term.term).value, 8);
            return getType("byte4");
        }
        if (std::holds_alternative<parser::bool_lit>(term.term))
        {
            string value = get<parser::bool_lit>(term.term).value;
            int size = types["byte1"].size;
            if (value == "true")
            {
                push(getWord(size) + " -1", size);
            }
            else
            {
                push(getWord(size) + " 0", size);
            }
            return getType("byte1");
        }
        else if (std::holds_alternative<nodeFuncCall *>(term.term))
        {
            return generateFuncCall(*get<nodeFuncCall *>(term.term), true);
        }
        else if (std::holds_alternative<ident>(term.term))
        {
            varData *data = getVar(get<ident>(term.term).ident);
            int varStackPos = data->stackPos;
            push(getWord(data->varType->size) + " [rsp + " + to_string(varStackPos - stackPointer) + "]", data->varType->size);
            return data->varType;
        }
        else if (std::holds_alternative<nodeExpr *>(term.term))
        {
            typeData *type = generateExpr(*get<nodeExpr *>(term.term));
            free(get<nodeExpr *>(term.term));
            return type;
        }
        else
        {
            throw;
        }
    }

    typeData *generateExpr(nodeExpr expr)
    {
        if (holds_alternative<nodeTerm>(expr.expr))
        {
            return generateTerm(get<nodeTerm>(expr.expr));
        }
        else if (holds_alternative<nodeBinExpr>(expr.expr))
        {
            typeData *type;

            nodeBinExpr binExpr = get<nodeBinExpr>(expr.expr);

            typeData *lhsType = generateExpr(*binExpr.rhs);
            typeData *rhsType = generateExpr(*binExpr.lhs);

            free(binExpr.rhs);
            free(binExpr.lhs);

            // binExpr
            switch (binExpr.op)
            {
            case TokenType::plus:
                pop('a', 8);
                pop('b', 8);

                output << "    add eax, ebx\n";

                push('a', 8);

                compTypes(lhsType, getType("byte4"));
                compTypes(rhsType, getType("byte4"));
                type = getType("byte4");
                break;
            case TokenType::minus:
                pop('a', 8);
                pop('b', 8);

                output << "    sub eax, ebx\n";

                push('a', 8);

                type = getType("byte4");
                break;
            case TokenType::star:
                pop('a', 8);
                pop('b', 8);

                output << "    imul ebx\n";

                push('a', 8);

                compTypes(lhsType, getType("byte4"));
                compTypes(rhsType, getType("byte4"));
                type = getType("byte4");
                break;
            case TokenType::backslash:
                pop('a', 8);
                pop('b', 8);

                output << "    xor rdx, rdx\n";
                output << "    div ebx\n";

                push('a', 8);

                compTypes(lhsType, getType("byte4"));
                compTypes(rhsType, getType("byte4"));
                type = getType("byte4");
                break;
            case TokenType::is_equal:
                pop('a', 8);
                pop('b', 8);

                output << "    cmp eax, ebx\n";
                output << "    setne al\n";
                output << "    dec al\n";

                push('a', 8);

                type = getType("byte1");
                break;
            case TokenType::not_equal:
                pop('a', 8);
                pop('b', 8);

                output << "    cmp eax, ebx\n";
                output << "    sete al\n";
                output << "    dec al\n";

                push('a', 8);

                type = getType("byte1");
                break;
            case TokenType::less_than:
                pop('a', 8);
                pop('b', 8);

                output << "    cmp eax, ebx\n";
                output << "    setge al\n";
                output << "    dec al\n";

                push('a', 8);

                compTypes(lhsType, getType("byte4"));
                compTypes(rhsType, getType("byte4"));
                type = getType("byte1");
                break;
            case TokenType::greater_than:
                pop('a', 8);
                pop('b', 8);

                output << "    cmp eax, ebx\n";
                output << "    setle al\n";
                output << "    dec al\n";

                push('a', 8);

                compTypes(lhsType, getType("byte4"));
                compTypes(rhsType, getType("byte4"));
                type = getType("byte1");
                break;
            case TokenType::less_or_equal:
                pop('a', 8);
                pop('b', 8);

                output << "    cmp eax, ebx\n";
                output << "    setg al\n";
                output << "    dec al\n";

                push('a', 8);

                compTypes(lhsType, getType("byte4"));
                compTypes(rhsType, getType("byte4"));
                type = getType("byte1");
                break;
            case TokenType::greater_or_equal:
                pop('a', 8);
                pop('b', 8);

                output << "    cmp eax, ebx\n";
                output << "    setl al\n";
                output << "    dec al\n";

                push('a', 8);

                compTypes(lhsType, getType("byte4"));
                compTypes(rhsType, getType("byte4"));
                type = getType("byte1");
                break;
            case TokenType::logical_or:
                pop('a', 8);
                pop('b', 8);

                output << "    or al, bl\n";

                push('a', 8);

                compTypes(lhsType, getType("byte1"));
                compTypes(rhsType, getType("byte1"));
                type = getType("byte1");
                break;
            case TokenType::logical_and:
                pop('a', 8);
                pop('b', 8);

                output << "    and al, bl\n";

                push('a', 8);

                compTypes(lhsType, getType("byte1"));
                compTypes(rhsType, getType("byte1"));
                type = getType("byte1");
                break;
            case TokenType::logical_xor:
                pop('a', 8);
                pop('b', 8);

                output << "    xor al, bl\n";

                push('a', 8);

                compTypes(lhsType, getType("byte1"));
                compTypes(rhsType, getType("byte1"));
                type = getType("byte1");
                break;
            default:
                errorHandler::error("Invalid operator " + lexer::tokenNames.at(binExpr.op));
            }
            return type;
        }
        else
        {
            errorHandler::error("WTF");
            return NULL;
        }
    }
    void generateVarDecl(nodeVarDecl decl)
    {
        // Reserve space for var
        output << "    add rsp, " << getType(decl.varType)->size << "\n";
        stackPointer += getType(decl.varType)->size;

        createVar(decl.varName, decl.varType, stackPointer);

        return;
    }
    void generateVarDef(nodeVarDef def)
    {
        compTypes(generateExpr(def.expr), getType(def.varType));

        createVar(def.varName, def.varType, stackPointer);

        return;
    }
    void generateVarAssign(nodeVarAssign assign)
    {
        varData *varData = getVar(assign.varName);

        compTypes(generateExpr(assign.expr), varData->varType);

        pop(getReg("a", varData->varType->size), varData->varType->size);

        int varOffset = varData->stackPos - stackPointer;

        output << "    mov [rsp + " << varOffset << "], " << getReg("a", varData->varType->size) << "\n";
    }
    typeData *generateFuncCall(nodeFuncCall call, bool returnValue)
    {
        if (funcs.count(call.funcName) == 0)
        {
            errorHandler::error("Function " + call.funcName + " was not declared.");
        }
        funcData *data = &funcs.at(call.funcName);

        if (call.params.size() < data->params.size())
        {
            errorHandler::error("Too few arguments in function call '" + call.funcName + "'.");
        }

        output << "    sub rsp, 8\n";
        stackPointer -= 8;

        for (int i = 0; i < data->params.size(); i++)
        {
            compTypes(generateExpr(call.params.at(i)), data->params[i]);
        }

        output << "    call " << call.funcName << endl;

        int popSize = 0;
        for (int i = 0; i < call.params.size(); i++)
        {
            popSize += 8;
        }
        if (!returnValue)
        {
            popSize += data->returnType->size;
        }
        output << "    add rsp, " << popSize << "\n";
        stackPointer += popSize;

        return data->returnType;
    }

    void generateScope(nodeScope scope);

    void generateStmtReturn(nodeStmtReturn stmt)
    {
        generateVarAssign({.varName = "_returnVal", .expr = stmt.expr});
        output << "    ret\n";
    }

    void generateStmtIf(nodeStmtIf stmt)
    {
        compTypes(generateExpr(stmt.expr), getType("byte1"));

        int ifStmtNum = ifStmtCount;
        ifStmtCount++;

        pop('a', 8);
        output << "    or al, al\n";
        output << "    jz if_stmt_end" << ifStmtNum << "\n";
        generateScope(*stmt.scope);
        output << "if_stmt_end" << ifStmtNum << ":\n";
    }
    void generateStmtFor(nodeStmtFor stmt)
    {
        int forLoopNum = forLoopCount;
        forLoopCount++;

        generateVarDef(stmt.def);

        output << "for_loop_start" << forLoopNum << ":\n";
        compTypes(generateExpr(stmt.expr), getType("byte1"));
        pop('a', 8);

        output << "    or al, al\n";
        output << "    jz for_loop_end" << forLoopNum << "\n";

        generateVarAssign(stmt.assign);

        generateScope(*stmt.scope);

        output << "    jmp for_loop_start" << forLoopNum << "\n";
        output << "for_loop_end" << forLoopNum << ":\n";
        pop("eax", 8);
    }
    void generateStmtWhile(nodeStmtWhile stmt)
    {
        int whileLoopNum = whileLoopCount;
        whileLoopCount++;

        output << "while_loop_start" << whileLoopNum << ":\n";
        compTypes(generateExpr(stmt.expr), getType("byte1"));
        pop('a', 8);

        output << "    or al, al\n";
        output << "    jz while_loop_end" << whileLoopNum << "\n";

        generateScope(*stmt.scope);

        output << "    jmp while_loop_start" << whileLoopNum << "\n";
        output << "while_loop_end" << whileLoopNum << ":\n";
    }

    void generateScope(nodeScope scope)
    {
        int varCount = vars.size();
        for (auto stmt : scope.body)
        {
            if (holds_alternative<nodeVarDecl>(stmt))
            {
                generateVarDecl(get<nodeVarDecl>(stmt));
            }
            else if (holds_alternative<nodeVarDef>(stmt))
            {
                generateVarDef(get<nodeVarDef>(stmt));
            }
            else if (holds_alternative<nodeVarAssign>(stmt))
            {
                generateVarAssign(get<nodeVarAssign>(stmt));
            }
            else if (holds_alternative<nodeFuncCall>(stmt))
            {
                generateFuncCall(get<nodeFuncCall>(stmt), false);
            }
            else if (holds_alternative<nodeStmtReturn>(stmt))
            {
                generateStmtReturn(get<nodeStmtReturn>(stmt));
            }
            else if (holds_alternative<nodeStmtIf>(stmt))
            {
                generateStmtIf(get<nodeStmtIf>(stmt));
            }
            else if (holds_alternative<nodeStmtFor>(stmt))
            {
                generateStmtFor(get<nodeStmtFor>(stmt));
            }
            else if (holds_alternative<nodeStmtWhile>(stmt))
            {
                generateStmtWhile(get<nodeStmtWhile>(stmt));
            }
            else
            {
                throw;
            }
        }
        int popSize = 0;
        while (vars.size() != varCount)
        {
            varData data = vars.back();
            vars.pop_back();

            popSize += 8;
        }
        output << "    add rsp, " << popSize << "\n";
        stackPointer += popSize;
    }
    void generateFuncDef(nodeFuncDef def)
    {
        funcData data = {.funcName = def.funcName, .returnType = getType(def.returnType)};

        createVar("_returnVal", def.returnType, stackPointer + 8 + def.params.size() * 8);

        output << def.funcName << ":\n";
        for (int i = 0; i < def.params.size(); i++)
        {
            nodeVarDecl param = def.params[i];
            createVar(param.varName, param.varType, stackPointer + (def.params.size() - i) * 8); // + 1 for return value
            data.params.push_back(getType(param.varType));
        }

        funcs.insert({def.funcName, data});

        generateScope(def.body);

        for (int i = 0; i < def.params.size() + 1; i++) // params.size() + 1 to pop all params and the return val
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