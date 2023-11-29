#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

#include "lexer.cpp"
#include "parser.cpp"
#include "generator.cpp"

int main()
{
    cout << "Tokenizing..." << endl;

    vector<lexer::Token> tokenList;

    fstream code;
    code.open("code.floh", ios::in);
    if (code.is_open())
    {
        string line;
        while (getline(code, line))
        {
            lexer::tokenizeLine(&tokenList, line);
        }
        code.close();
    }

    cout << "Parsing..." << endl;
    nodeProg prog = parser::parse(&tokenList);

    cout << "Generating..." << endl;
    generator::generate(prog);
    
    return 0;
}
