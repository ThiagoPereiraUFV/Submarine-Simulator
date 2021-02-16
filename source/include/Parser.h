#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Object3D.h"
#include "util.cpp" //contém declarações de tipos uteis : GLdb3, GLint3, etc

using namespace std;
class parserOBJ
{
    public:
        static Object3D parse(string filename);
    private:
        parserOBJ();
};

#endif