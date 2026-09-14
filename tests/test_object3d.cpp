#include "doctest.h"
#include "Object3D.h"

TEST_CASE("Object3D stores mesh data passed to its constructor") {
	std::vector<Vec3> vertices{{1, 2, 3}, {4, 5, 6}};
	std::vector<Vec3> normals{{0, 1, 0}};
	std::vector<Vec3> texcoords{{0.5, 0.5, 0}};
	std::vector<Face> faces{{0, 1, 0}};
	std::vector<Face> faceNormals{{0, 0, 0}};
	std::vector<Face> faceTexcoords{{0, 0, 0}};

	Object3D obj(vertices, normals, texcoords, faces, faceNormals, faceTexcoords);

	CHECK(obj.vertices() == vertices);
	CHECK(obj.normals() == normals);
	CHECK(obj.texcoords() == texcoords);
	CHECK(obj.faces() == faces);
	CHECK(obj.faceNormals() == faceNormals);
	CHECK(obj.faceTexcoords() == faceTexcoords);
}

TEST_CASE("Object3D defaults position, color and rotations to zero") {
	Object3D obj;

	CHECK(obj.position() == Vec3{0, 0, 0});
	CHECK(obj.color() == Vec3{0, 0, 0});
	CHECK(obj.rotY() == 0.0);
	CHECK(obj.rotZ() == 0.0);
	CHECK(obj.vertices().empty());
	CHECK(obj.faces().empty());
}

TEST_CASE("setRotY wraps outside [0, 360] and keeps values inside it") {
	Object3D obj;

	obj.setRotY(361.0);
	CHECK(obj.rotY() == 0.0);

	obj.setRotY(-1.0);
	CHECK(obj.rotY() == 360.0);

	obj.setRotY(45.0);
	CHECK(obj.rotY() == 45.0);
}

TEST_CASE("setPosition, setColor and setRotZ store their argument verbatim") {
	Object3D obj;

	obj.setPosition({1, 2, 3});
	obj.setColor({0.1, 0.2, 0.3});
	obj.setRotZ(90.0);

	CHECK(obj.position() == Vec3{1, 2, 3});
	CHECK(obj.color() == Vec3{0.1, 0.2, 0.3});
	CHECK(obj.rotZ() == 90.0);
}

TEST_CASE("copies are independent from the original") {
	std::vector<Vec3> vertices{{1, 2, 3}};
	Object3D original(vertices, {}, {}, {}, {}, {});
	original.setPosition({7, 8, 9});

	Object3D copy = original;
	copy.setPosition({0, 0, 0});

	CHECK(original.position() == Vec3{7, 8, 9});
	CHECK(copy.position() == Vec3{0, 0, 0});
	CHECK(copy.vertices() == vertices);
}
