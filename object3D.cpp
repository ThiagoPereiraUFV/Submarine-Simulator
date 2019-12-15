#include "object3D.h"

object3D::object3D(const vector<GLdb3> &_v_list, const vector<GLdb3> &_vn_list, const vector<GLdb3> &_vt_list, const vector<GLint3> &_f_list, const vector<GLint3> &_n_list, const vector<GLint3> &_t_list)
{
    V = _v_list;
    VN = _vn_list;
    VT = _vt_list;
    F = _f_list;
    N = _n_list;
    T = _t_list;
    pos.x = pos.y = pos.z = 0;
    color.x = color.y = color.z = 0;
    rot = 0.0;
};

object3D::object3D(const object3D & obj)
{
    V = vector<GLdb3>();
    VN = vector<GLdb3>();
    VT = vector<GLdb3>();
    F = vector<GLint3>();
    N = vector<GLint3>();
    T = vector<GLint3>();
    pos.x = pos.y = pos.z = 0;
    color.x = color.y = color.z = 0;
    rot = 0.0;
    *this = obj;
}

object3D & object3D::operator=(const object3D & obj)
{
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

vector<GLdb3> object3D::getVertices() const
{
    return V;
}

vector<GLdb3> object3D::getVNormals() const
{
    return VN;
}

vector<GLdb3> object3D::getVTexture() const
{
    return VT;
}

vector<GLint3> object3D::getFaces() const
{
    return F;
}

vector<GLint3> object3D::getNormals() const
{
    return N;
}

vector<GLint3> object3D::getTexture() const
{
    return T;
}

void object3D::draw() const
{
    glPushMatrix();
        glColor3f(color.x, color.y, color.z);
        glTranslated(pos.x,pos.y,pos.z);
        glRotated(-rot,0.0,1.0,0.0);
        
        for(unsigned long i = 0; i < F.size(); i++)
        {
            glBegin(GL_TRIANGLES);
                glNormal3d(VN[N[i].x].x, VN[N[i].x].y, VN[N[i].x].z);
                glVertex3d(V[F[i].x].x, V[F[i].x].y, V[F[i].x].z);
                glNormal3d(VN[N[i].y].x, VN[N[i].y].y, VN[N[i].y].z);
                glVertex3d(V[F[i].y].x, V[F[i].y].y, V[F[i].y].z);
                glNormal3d(VN[N[i].z].x, VN[N[i].z].y, VN[N[i].z].z);
                glVertex3d(V[F[i].z].x, V[F[i].z].y, V[F[i].z].z);
            glEnd();
        }
    glPopMatrix();
	
};


void object3D::justDraw() const
{
    //glPushMatrix();
        //glBindTexture(GL_TEXTURE_2D, tex);
        glColor3f(color.x, color.y, color.z);
        glTranslated(pos.x,pos.y,pos.z);
        glRotated(-rot,0.0,1.0,0.0);
        
        for(unsigned long i = 0; i < F.size(); i++)
        {
            glBegin(GL_TRIANGLES);
                glNormal3d(VN[N[i].x].x, VN[N[i].x].y, VN[N[i].x].z);
                glTexCoord3d(VT[T[i].x].x, VT[T[i].x].y, VT[T[i].x].z);
                glVertex3d(V[F[i].x].x, V[F[i].x].y, V[F[i].x].z);
                
                glNormal3d(VN[N[i].y].x, VN[N[i].y].y, VN[N[i].y].z);
                glTexCoord3d(VT[T[i].y].x, VT[T[i].y].y, VT[T[i].y].z);
                glVertex3d(V[F[i].y].x, V[F[i].y].y, V[F[i].y].z);
                
                glNormal3d(VN[N[i].z].x, VN[N[i].z].y, VN[N[i].z].z);
                glTexCoord3d(VT[T[i].z].x, VT[T[i].z].y, VT[T[i].z].z);
                glVertex3d(V[F[i].z].x, V[F[i].z].y, V[F[i].z].z);
            glEnd();
        }
    
	//glPopMatrix();
};

GLdb3 object3D::getPos() const
{
    return pos;
};

void object3D::setPos(const GLdb3 & pos)
{
    this->pos.x = pos.x;
    this->pos.y = pos.y;
    this->pos.z = pos.z;
    
    return;
};

void object3D::setRot(const GLdouble & rot)
{
    this->rot = rot;
    return;
};

GLdouble object3D::getRot() const
{
    return rot;
};


void object3D::setColor(const GLdb3 & color)
{
    this->color.x = color.x;
    this->color.y = color.y;
    this->color.z = color.z;
    return;
};
