#include "ObjParser.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace {

std::vector<std::string> splitOnWhitespace(const std::string& line) {
	std::istringstream ss(line);
	std::vector<std::string> tokens;
	std::string token;
	while (ss >> token)
		tokens.push_back(token);
	return tokens;
}

double parseNumber(const std::string& token, const std::string& line) {
	try {
		std::size_t consumed = 0;
		double value = std::stod(token, &consumed);
		if (consumed != token.size())
			throw std::invalid_argument("trailing characters");
		return value;
	} catch (const std::exception&) {
		throw std::runtime_error("malformed line: " + line);
	}
}

// v/vn always carry x,y,z in these models; vt carries only u,v (w defaults to 0,
// matching every vt line actually found in models/*.obj).
Vec3 parseVec3(const std::vector<std::string>& tokens, const std::string& line,
			   std::size_t minComponents) {
	if (tokens.size() < 1 + minComponents)
		throw std::runtime_error("malformed line: " + line);
	double x = parseNumber(tokens[1], line);
	double y = parseNumber(tokens[2], line);
	double z = (tokens.size() >= 4) ? parseNumber(tokens[3], line) : 0.0;
	return {x, y, z};
}

// A single "a", "a/b", "a//c" or "a/b/c" face reference, still 1-based as read from the file.
struct FaceRef {
	int vertex = 0;
	bool hasTexcoord = false;
	int texcoord = 0;
	bool hasNormal = false;
	int normal = 0;
};

int parseIndex(const std::string& token, const std::string& line) {
	try {
		std::size_t consumed = 0;
		int value = std::stoi(token, &consumed);
		if (consumed != token.size())
			throw std::invalid_argument("trailing characters");
		return value;
	} catch (const std::exception&) {
		throw std::runtime_error("malformed line: " + line);
	}
}

FaceRef parseFaceRef(const std::string& ref, const std::string& line) {
	std::vector<std::string> parts;
	std::size_t start = 0;
	for (;;) {
		std::size_t slash = ref.find('/', start);
		parts.push_back(ref.substr(start, slash - start));
		if (slash == std::string::npos)
			break;
		start = slash + 1;
	}
	if (parts.empty() || parts[0].empty())
		throw std::runtime_error("malformed line: " + line);

	FaceRef out;
	out.vertex = parseIndex(parts[0], line) - 1;
	if (parts.size() >= 2 && !parts[1].empty()) {
		out.hasTexcoord = true;
		out.texcoord = parseIndex(parts[1], line) - 1;
	}
	if (parts.size() == 3 && !parts[2].empty()) {
		out.hasNormal = true;
		out.normal = parseIndex(parts[2], line) - 1;
	}
	return out;
}

} // namespace

Object3D obj::parse(std::istream& in) {
	std::vector<Vec3> vertices, normals, texcoords;
	std::vector<Face> faces, faceNormals, faceTexcoords;

	std::string line;
	while (std::getline(in, line)) {
		std::vector<std::string> tokens = splitOnWhitespace(line);
		if (tokens.empty())
			continue;

		const std::string& keyword = tokens[0];
		if (keyword == "v") {
			vertices.push_back(parseVec3(tokens, line, 3));
		} else if (keyword == "vn") {
			normals.push_back(parseVec3(tokens, line, 3));
		} else if (keyword == "vt") {
			texcoords.push_back(parseVec3(tokens, line, 2));
		} else if (keyword == "f") {
			if (tokens.size() != 4)
				throw std::runtime_error("malformed line: " + line);

			FaceRef refs[3] = {parseFaceRef(tokens[1], line), parseFaceRef(tokens[2], line),
							   parseFaceRef(tokens[3], line)};

			faces.push_back({refs[0].vertex, refs[1].vertex, refs[2].vertex});

			if (refs[0].hasTexcoord && refs[1].hasTexcoord && refs[2].hasTexcoord)
				faceTexcoords.push_back({refs[0].texcoord, refs[1].texcoord, refs[2].texcoord});

			if (refs[0].hasNormal && refs[1].hasNormal && refs[2].hasNormal)
				faceNormals.push_back({refs[0].normal, refs[1].normal, refs[2].normal});
		}
		// Every other keyword (#, o, g, s, usemtl, mtllib, ...) is ignored.
	}

	return Object3D(vertices, normals, texcoords, faces, faceNormals, faceTexcoords);
}

Object3D obj::parseFile(const std::string& path) {
	std::ifstream file(path);
	if (!file)
		throw std::runtime_error("cannot open " + path);
	return parse(file);
}
