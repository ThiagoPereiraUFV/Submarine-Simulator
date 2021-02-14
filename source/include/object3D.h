#ifndef OBJ_H
#define OBJ_H

#include <vector>
#include "util.cpp"

using namespace std;

class object3D
{
    public:
        GLdouble getRot() const;
        void setRot(const GLdouble & rot);
        GLdb3 getPos() const;
        void setPos(const GLdb3 & pos);
        void setColor(const GLdb3 & color);
        void draw() const;
        void justDraw() const; //desenha sem se preocupar com as trasnformações que vem antes
        void draw(const unsigned int tex) const;
        vector<GLdb3> getVertices() const;
        vector<GLdb3> getVNormals() const;
        vector<GLdb3> getVTexture() const;
        vector<GLint3> getFaces() const;
        vector<GLint3> getNormals() const;
        vector<GLint3> getTexture() const;
        object3D(){};
        object3D(const object3D & obj);
        object3D(const vector<GLdb3> &_v_list, const vector<GLdb3> &_vn_list, const vector<GLdb3> &_vt_list, const vector<GLint3> &_f_list, const vector<GLint3> &_n_list, const vector<GLint3> &_t_list);
        object3D & operator=(const object3D & obj);
    private:
        vector<GLdb3> V, VN, VT; //lista de vertices ,vertices normais e vertices textura
        vector<GLint3> F, N, T; //lista de faces, normais e textura
        GLdb3 pos; //posição central do objeto no espaço
        GLdouble rot; //rotação atual do objeto 
        GLdb3 color; //cor do objeto 
};



#endif
