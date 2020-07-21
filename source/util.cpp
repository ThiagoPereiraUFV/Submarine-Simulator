#ifndef UTIL_H
#define UTIL_H
#include <GL/glut.h>
//tipos uteis

struct GLdb3
{
    GLdouble x,y,z;
};

struct GLf3
{
    GLfloat x,y,z;
};

struct GLint3
{
    GLint x,y,z;
    GLint3 &operator=(const GLint3 & obj) {
        this->x = obj.x;
        this->y = obj.y;
        this->z = obj.z;
    }

};

#endif