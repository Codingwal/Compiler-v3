#pragma once

#include <stdlib.h>
#include <sstream>
using namespace std;

namespace errorHandler
{
    void error(string msg)
    {
        cout << "[ERROR]: " << msg << endl;
        exit(EXIT_FAILURE);
    }
}