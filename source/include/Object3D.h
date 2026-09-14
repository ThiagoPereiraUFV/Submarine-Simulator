#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <vector>
#include "Vec3.h"

// A parsed OBJ mesh plus the placement (position/color/rotation) the game
// gives it. Pure data: drawing lives in the renderer, not here, so this class
// never depends on GL.
class Object3D {
public:
	Object3D() = default;
	Object3D(std::vector<Vec3> vertices, std::vector<Vec3> normals, std::vector<Vec3> texcoords,
			 std::vector<Face> faces, std::vector<Face> faceNormals,
			 std::vector<Face> faceTexcoords);
	// Copy and move are left implicit on purpose: declaring the copy operations
	// (even as `= default`) suppresses the move constructor, which would turn
	// every `std::move` of a 20k-vertex mesh into a silent deep copy.

	const std::vector<Vec3>& vertices() const { return vertices_; }
	const std::vector<Vec3>& normals() const { return normals_; }
	const std::vector<Vec3>& texcoords() const { return texcoords_; }
	const std::vector<Face>& faces() const { return faces_; }
	const std::vector<Face>& faceNormals() const { return faceNormals_; }
	const std::vector<Face>& faceTexcoords() const { return faceTexcoords_; }

	Vec3 position() const { return position_; }
	Vec3 color() const { return color_; }
	double rotY() const { return rotY_; }
	double rotZ() const { return rotZ_; }

	void setPosition(const Vec3&);
	void setColor(const Vec3&);
	// Wraps outside [0, 360]: this is the submarine's steering rule, not a defect.
	void setRotY(double);
	void setRotZ(double);

private:
	std::vector<Vec3> vertices_, normals_, texcoords_;
	std::vector<Face> faces_, faceNormals_, faceTexcoords_;
	Vec3 position_, color_;
	double rotY_ = 0.0, rotZ_ = 0.0;
};

#endif
