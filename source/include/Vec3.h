#ifndef VEC3_H
#define VEC3_H

// Plain point/vector type shared by the domain and the renderer seam. Kept
// free of GL headers so the domain (Object3D, ObjParser, Simulation) never
// depends on OpenGL.
struct Vec3 {
	double x = 0.0, y = 0.0, z = 0.0;
};

inline bool operator==(const Vec3& a, const Vec3& b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

// A triangle's three 0-based indices into a Vec3 array (vertices, normals or texcoords).
struct Face {
	int a = 0, b = 0, c = 0;
};

inline bool operator==(const Face& lhs, const Face& rhs) {
	return lhs.a == rhs.a && lhs.b == rhs.b && lhs.c == rhs.c;
}

#endif
