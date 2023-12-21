#pragma once

#include <string>
#include <map>

#include "errorHandler.cpp"

namespace lexer
{
    enum TokenType
    {
        custom,
        int_lit,

        open_paren,
        close_paren,
        open_curly,
        close_curly,
        semicolon,
        comma,
        equal,

        plus,
        minus,
        star,
        backslash,

        _struct,
        _if,
        _while,
        _for
    };
    struct Token
    {
        TokenType type;
        string value;

        Token(TokenType type, string value)
        {
            this->type = type;
            this->value = value;
        }
    };

    map<TokenType, string> tokenNames =
        {
            {TokenType::custom, "custom"},
            {TokenType::int_lit, "int_lit"},
            {TokenType::open_paren, "open_paren"},
            {TokenType::close_paren, "close_paren"},
            {TokenType::open_curly, "open_curly"},
            {TokenType::close_curly, "close_curly"},
            {TokenType::semicolon, "semicolon"},
            {TokenType::comma, "comma"},
            {TokenType::equal, "equal"},
            {TokenType::_struct, "_struct"},
            {TokenType::_if, "_if"},
            {TokenType::_while, "_while"},
            {TokenType::_for, "_for"},

    };

    map<string, TokenType>
        keywords =
            {
                {"struct", TokenType::_struct},
                {"if", TokenType::_if},
                {"while", TokenType::_while},
                {"for", TokenType::_for},
    };

    map<string, TokenType> symbols =
        {
            {"(", TokenType::open_paren},
            {")", TokenType::close_paren},
            {"{", TokenType::open_curly},
            {"}", TokenType::close_curly},

            {";", TokenType::semicolon},
            {",", TokenType::comma},
            {"=", TokenType::equal},

            {"+", TokenType::plus},
            {"-", TokenType::minus},
            {"*", TokenType::star},
            {"/", TokenType::backslash},
    };

    int lineIndex;
    string *line;

    bool charsLeft()
    {
        return lineIndex + 1 < line->size();
    }

    char tryPeek()
    {
        char c;
        if (charsLeft())
        {
            c = line->at(lineIndex + 1);
        }
        else
        {
            c = ' ';
        }
        return c;
    }

    char peek()
    {
        return line->at(lineIndex + 1);
    }

    char consume()
    {
        if (!charsLeft())
        {
            errorHandler::error("Unexpected end of line.");
        }
        lineIndex++;
        char c = line->at(lineIndex);
        return c;
    }

    Token tokenizeBuffer(string *buffer)
    {
        if (keywords.count(*buffer) == 0)
        {
            return Token(TokenType::custom, *buffer);
        }
        else
        {
            const TokenType &keyword = keywords[*buffer];
            return Token(keyword, "");
        }
    }

    bool isUsedSymbol(char c)
    {
        switch (c)
        {
        case '(':
        case ')':
        case '{':
        case '}':

        case ';':
        case ',':
        case '=':

        case '+':
        case '-':
        case '*':
        case '/':
            return true;
        default:
            return false;
        };
    }

    void tokenizeLine(vector<Token> *tokenList, string currentLine)
    {
        lineIndex = -1;
        line = &currentLine;

        string buffer = "";
        while (charsLeft())
        {
            char c = peek();

            if (c == ' ')
            {
                consume();
            }
            else if (isalpha(c))
            {
                while (isalnum(tryPeek()))
                {
                    buffer += consume();
                }
                tokenList->push_back(tokenizeBuffer(&buffer));
                buffer.clear();
            }
            else if (isdigit(c))
            {
                while (isdigit(tryPeek()))
                {
                    buffer += consume();
                }
                tokenList->push_back(Token(TokenType::int_lit, buffer));
                buffer.clear();
            }
            else if (isUsedSymbol(c))
            {
                do
                {
                    buffer += consume();
                } while (isUsedSymbol(tryPeek()));
                if (symbols.count(buffer) == 1)
                {
                    tokenList->push_back(Token(symbols[buffer], ""));
                }
                else
                {
                    for (char c : buffer)
                    {
                        if (symbols.count(string(1, c)) == 1)
                        {
                            tokenList->push_back(Token(symbols[string(1, c)], ""));
                        }
                        else
                        {
                            errorHandler::error(string("Invalid symbol '") + c + "'.");
                        }
                    }
                }
                buffer.clear();
            }
            else
            {
                errorHandler::error(string("Invalid character '") + c + "'.");
            }
        }
    }
}