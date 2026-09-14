#include "Object3D.h"

Object3D::Object3D(std::vector<Vec3> vertices, std::vector<Vec3> normals,
				   std::vector<Vec3> texcoords, std::vector<Face> faces,
				   std::vector<Face> faceNormals, std::vector<Face> faceTexcoords)
	: vertices_(std::move(vertices)), normals_(std::move(normals)),
	  texcoords_(std::move(texcoords)), faces_(std::move(faces)),
	  faceNormals_(std::move(faceNormals)), faceTexcoords_(std::move(faceTexcoords)) {}

void Object3D::setPosition(const Vec3& position) {
	position_ = position;
}

void Object3D::setColor(const Vec3& color) {
	color_ = color;
}

void Object3D::setRotY(double rotY) {
	rotY_ = (rotY > 360.0) ? 0.0 : (rotY < 0.0) ? 360.0 : rotY;
}

void Object3D::setRotZ(double rotZ) {
	rotZ_ = rotZ;
}
