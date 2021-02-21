#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <sstream>
#include <string>
#include "Object3D.h"
#include "util.cpp"

using std::string;
using std::fstream;
using std::stringstream;

class parserOBJ {
    public:
        static Object3D parse(const string&);
    private:
        parserOBJ();
};

#endif