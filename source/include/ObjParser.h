#ifndef OBJPARSER_H
#define OBJPARSER_H

#include <istream>
#include <string>
#include "Object3D.h"

// Minimal Wavefront OBJ reader for the triangle-only, quad-free models this
// game ships (see models/*.obj): only v/vn/vt/f lines matter, every other
// line (comments, o/g/s/usemtl/mtllib, blank lines) is ignored.
namespace obj {
// Throws std::runtime_error on any v/vn/vt line without 3 numeric
// tokens, or any f line without exactly 3 vertex references.
Object3D parse(std::istream& in);

// Throws std::runtime_error("cannot open " + path) if the file can't be opened.
Object3D parseFile(const std::string& path);
} // namespace obj

#endif
