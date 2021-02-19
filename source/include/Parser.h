#ifndef PARSER_H
#define PARSER_H

#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
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
        static Object3D parse(string filename);
    private:
        parserOBJ();
};

#endif