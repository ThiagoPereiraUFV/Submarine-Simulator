#include "Parser.h"
#include <algorithm>

Object3D parserOBJ::parse(const string& filename) {
    string line;
    fstream fs;
    fs.open(filename.c_str());

    vector<GLdb3> vertices, vNormals, vTexture;
    vector<GLint3> faces, normals, textures;

    while(getline(fs, line)) {
        vector<string> t;

        stringstream ss(line);
        string temp;
        while (ss >> temp)
            t.push_back(temp);

        if(t[0] == "v") {
            GLdb3 vertice;
            vertice.x = atof(t[1].c_str()); vertice.y = atof(t[2].c_str()); vertice.z = atof(t[3].c_str());
            vertices.push_back(vertice);
        } else if(t[0] == "vn") {
            GLdb3 normal;
            normal.x = atof(t[1].c_str()); normal.y = atof(t[2].c_str()); normal.z = atof(t[3].c_str());
            vNormals.push_back(normal);
        } else if(t[0] == "vt") {
            GLdb3 texture;
            texture.x = atof(t[1].c_str()); texture.y = atof(t[2].c_str()); texture.z = atof(t[3].c_str());
            vTexture.push_back(texture);
        } else if(t[0] == "f") {
            GLint3 face, normal, texture;

            for(long unsigned int i = 1; i < t.size(); i++) {
                replace(t[i].begin(), t[i].end(), '/', ' ');

                stringstream ssaux(t[i]);
                vector<string> values;
                while(ssaux >> temp)
                    values.push_back(temp);

                if(values.size() == 3) {
                    if(i == 1) {
                        face.x = atoi(values[0].c_str())-1;
                        texture.x = atoi(values[1].c_str())-1;
                        normal.x = atoi(values[2].c_str())-1;
                    } else if(i == 2) {
                        face.y = atoi(values[0].c_str())-1;
                        texture.y = atoi(values[1].c_str())-1;
                        normal.y = atoi(values[2].c_str())-1;
                    } else {
                        face.z = atoi(values[0].c_str())-1;
                        texture.z = atoi(values[1].c_str())-1;
                        normal.z = atoi(values[2].c_str())-1;
                    }
                } else {
                    if(i == 1) {
                        face.x = atoi(values[0].c_str())-1;
                        normal.x = atoi(values[1].c_str())-1;
                    } else if(i == 2) {
                        face.y = atoi(values[0].c_str())-1;
                        normal.y = atoi(values[1].c_str())-1;
                    } else {
                        face.z = atoi(values[0].c_str())-1;
                        normal.z = atoi(values[1].c_str())-1;
                    }
                }
            }

            faces.push_back(face);
            normals.push_back(normal);
            textures.push_back(texture);
        }
    }
    fs.close();

    return Object3D(vertices, vNormals, vTexture, faces, normals, textures);
};