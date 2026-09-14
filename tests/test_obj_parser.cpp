#include <sstream>
#include <stdexcept>
#include "doctest.h"
#include "ObjParser.h"

TEST_CASE("parse ignores comments, o/mtllib/s/usemtl lines and blank lines") {
	std::istringstream in("# a comment\n"
						  "\n"
						  "o mesh\n"
						  "mtllib mesh.mtl\n"
						  "usemtl Material\n"
						  "v 1 2 3\n"
						  "v 4 5 6\n"
						  "v 7 8 9\n"
						  "s off\n"
						  "f 1 2 3\n");

	Object3D obj = obj::parse(in);

	REQUIRE(obj.vertices().size() == 3);
	CHECK(obj.vertices()[0] == Vec3{1, 2, 3});
	REQUIRE(obj.faces().size() == 1);
	CHECK(obj.faces()[0] == Face{0, 1, 2});
}

TEST_CASE("parse handles the f a/b/c form (vertex/texcoord/normal)") {
	std::istringstream in("v 1 0 0\n"
						  "v 0 1 0\n"
						  "v 0 0 1\n"
						  "vt 0 0 0\n"
						  "vt 1 0 0\n"
						  "vt 0 1 0\n"
						  "vn 0 0 1\n"
						  "vn 0 0 1\n"
						  "vn 0 0 1\n"
						  "f 1/1/1 2/2/2 3/3/3\n");

	Object3D obj = obj::parse(in);

	CHECK(obj.faces()[0] == Face{0, 1, 2});
	REQUIRE(obj.faceTexcoords().size() == 1);
	CHECK(obj.faceTexcoords()[0] == Face{0, 1, 2});
	REQUIRE(obj.faceNormals().size() == 1);
	CHECK(obj.faceNormals()[0] == Face{0, 1, 2});
}

TEST_CASE("parse handles the f a//c form (vertex//normal, no texcoord)") {
	std::istringstream in("v 1 0 0\n"
						  "v 0 1 0\n"
						  "v 0 0 1\n"
						  "vn 0 0 1\n"
						  "vn 0 0 1\n"
						  "vn 0 0 1\n"
						  "f 1//1 2//2 3//3\n");

	Object3D obj = obj::parse(in);

	CHECK(obj.faces()[0] == Face{0, 1, 2});
	CHECK(obj.faceTexcoords().empty());
	REQUIRE(obj.faceNormals().size() == 1);
	CHECK(obj.faceNormals()[0] == Face{0, 1, 2});
}

TEST_CASE("a face's texcoord triple is only pushed when all three refs carry it") {
	std::istringstream in("v 1 0 0\n"
						  "v 0 1 0\n"
						  "v 0 0 1\n"
						  "vt 0 0 0\n"
						  "vn 0 0 1\n"
						  "f 1/1 2 3\n");

	Object3D obj = obj::parse(in);

	CHECK(obj.faceTexcoords().empty());
	CHECK(obj.faceNormals().empty());
}

TEST_CASE("v/vn/vt lines with fewer than 3 numeric tokens throw") {
	std::istringstream in("v 1 2\n");
	CHECK_THROWS_AS(obj::parse(in), std::runtime_error);
}

TEST_CASE("a v line with a non-numeric token throws") {
	std::istringstream in("v 1 abc 3\n");
	CHECK_THROWS_AS(obj::parse(in), std::runtime_error);
}

TEST_CASE("a v line with a token that has trailing garbage after the number throws") {
	std::istringstream in("v 1 2x 3\n");
	CHECK_THROWS_AS(obj::parse(in), std::runtime_error);
}

TEST_CASE("a face ref with a non-numeric vertex index throws") {
	std::istringstream in("v 1 2 3\nv 1 2 3\nv 1 2 3\nf a 2 3\n");
	CHECK_THROWS_AS(obj::parse(in), std::runtime_error);
}

TEST_CASE("a face ref with trailing garbage after the vertex index throws") {
	std::istringstream in("v 1 2 3\nv 1 2 3\nv 1 2 3\nf 1x 2 3\n");
	CHECK_THROWS_AS(obj::parse(in), std::runtime_error);
}

TEST_CASE("a face ref with an empty vertex index (e.g. \"/1/2\") throws") {
	std::istringstream in("v 1 2 3\nv 1 2 3\nv 1 2 3\nf /1/2 2 3\n");
	CHECK_THROWS_AS(obj::parse(in), std::runtime_error);
}

TEST_CASE("vn with fewer than 3 numeric tokens throws") {
	std::istringstream in("vn 1 2\n");
	CHECK_THROWS_AS(obj::parse(in), std::runtime_error);
}

TEST_CASE("vt requires at least u,v (w defaults to 0, matching real models)") {
	std::istringstream in("vt 1 2\n");
	Object3D obj = obj::parse(in);
	CHECK(obj.texcoords()[0] == Vec3{1, 2, 0});
}

TEST_CASE("vt with fewer than 2 numeric tokens throws") {
	std::istringstream in("vt 1\n");
	CHECK_THROWS_AS(obj::parse(in), std::runtime_error);
}

TEST_CASE("f with fewer than 3 refs throws") {
	std::istringstream in("v 1 2 3\nv 1 2 3\nf 1 2\n");
	CHECK_THROWS_AS(obj::parse(in), std::runtime_error);
}

TEST_CASE("f with more than 3 refs throws") {
	std::istringstream in("v 1 2 3\nv 1 2 3\nv 1 2 3\nv 1 2 3\nf 1 2 3 4\n");
	CHECK_THROWS_AS(obj::parse(in), std::runtime_error);
}

TEST_CASE("parseFile loads real models with the expected vertex/normal/face counts") {
	Object3D fish = obj::parseFile("models/fish1.obj");
	CHECK(fish.vertices().size() == 560);
	CHECK(fish.normals().size() == 162);
	CHECK(fish.faces().size() == 537);
	CHECK(fish.texcoords().empty());

	Object3D ship = obj::parseFile("models/ship1.obj");
	CHECK(ship.vertices().size() == 6594);
	CHECK(ship.normals().size() == 4923);
	CHECK(ship.texcoords().size() == 6606);
	CHECK(ship.faces().size() == 12988);
}

TEST_CASE("parseFile throws with the path in the message when the file cannot be opened") {
	try {
		obj::parseFile("models/does-not-exist.obj");
		FAIL("expected std::runtime_error");
	} catch (const std::runtime_error& e) {
		CHECK(std::string(e.what()) == "cannot open models/does-not-exist.obj");
	}
}
