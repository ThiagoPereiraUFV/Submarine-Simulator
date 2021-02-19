#include "Object3D.h"

Object3D::Object3D(
    const vector<GLdb3>& V, const vector<GLdb3>& VN, const vector<GLdb3>& VT,
    const vector<GLint3>& F, const vector<GLint3>& N, const vector<GLint3>& T
) {
    this->V = V;
    this->VN = VN;
    this->VT = VT;
    this->F = F;
    this->N = N;
    this->T = T;
    pos.x = pos.y = pos.z = 0;
    color.x = color.y = color.z = 0;
    rot = 0.0;
};

Object3D::Object3D(const Object3D& obj) {
    *this = obj;
}

Object3D& Object3D::operator=(const Object3D& obj) {
    if(this == &obj) {
        return *this;
    }

    V = obj.getVertices();
    VN = obj.getVNormals();
    VT = obj.getVTexture();
    F = obj.getFaces();
    T = obj.getTexture();
    N = obj.getNormals();
    pos = obj.getPos();
    rot = obj.getRot();
    color.x = obj.color.x;
    color.y = obj.color.y;
    color.z = obj.color.z;
};

void Object3D::draw() const {
    glPushMatrix();
    glColor3f(color.x, color.y, color.z);
    glTranslated(pos.x, pos.y, pos.z);
    glRotated(-rot, 0.0, 1.0, 0.0);

    for(unsigned long i = 0; i < F.size(); i++) {
        glBegin(GL_TRIANGLES);
            if(!V.empty() && !F.empty()) {
                glVertex3d(V[F[i].x].x, V[F[i].x].y, V[F[i].x].z);
                glVertex3d(V[F[i].y].x, V[F[i].y].y, V[F[i].y].z);
                glVertex3d(V[F[i].z].x, V[F[i].z].y, V[F[i].z].z);

                if(!VN.empty() && !N.empty()) {
                    glNormal3d(VN[N[i].x].x, VN[N[i].x].y, VN[N[i].x].z);
                    glNormal3d(VN[N[i].y].x, VN[N[i].y].y, VN[N[i].y].z);
                    glNormal3d(VN[N[i].z].x, VN[N[i].z].y, VN[N[i].z].z);
                }
                if(!VT.empty() && !T.empty()) {
                    glTexCoord3d(VT[T[i].x].x, VT[T[i].x].y, VT[T[i].x].z);
                    glTexCoord3d(VT[T[i].y].x, VT[T[i].y].y, VT[T[i].y].z);
                    glTexCoord3d(VT[T[i].z].x, VT[T[i].z].y, VT[T[i].z].z);
                }
            }
        glEnd();
    }

	glPopMatrix();
};

void Object3D::setPos(const GLdb3& pos) {
    this->pos.x = pos.x;
    this->pos.y = pos.y;
    this->pos.z = pos.z;
};

void Object3D::setRot(const GLdouble& rot) {
    this->rot = rot;
};

void Object3D::setColor(const GLdb3& color) {
    this->color.x = color.x;
    this->color.y = color.y;
    this->color.z = color.z;
};