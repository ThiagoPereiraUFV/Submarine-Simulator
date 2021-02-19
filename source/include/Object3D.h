#ifndef OBJ_H
#define OBJ_H

#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <vector>
#include "util.cpp"

using std::vector;

class Object3D {
    private:
        vector<GLdb3> V, VN, VT;    //  Vetices, normal vertices and texture vertices
        vector<GLint3> F, N, T; //  Faces, nomals and texture
        GLdb3 pos; //posição central do objeto no espaço
        GLdouble rot; //rotação atual do objeto
        GLdb3 color; //cor do objeto
    public:
        void draw() const;
        void setRot(const GLdouble&);
        void setPos(const GLdb3&);
        void setColor(const GLdb3&);
        GLdb3 getPos() const { return pos; }
        GLdouble getRot() const { return rot; }
        vector<GLdb3> getVertices() const { return V; }
        vector<GLdb3> getVNormals() const { return VN; }
        vector<GLdb3> getVTexture() const { return VT; }
        vector<GLint3> getFaces() const { return F; }
        vector<GLint3> getNormals() const { return N; }
        vector<GLint3> getTexture() const { return T; }
        Object3D(){};
        Object3D(const Object3D&);
        Object3D(
            const vector<GLdb3>&, const vector<GLdb3>&, const vector<GLdb3>&,
            const vector<GLint3>&, const vector<GLint3>&, const vector<GLint3>&
        );
        Object3D& operator=(const Object3D&);
};

#endif