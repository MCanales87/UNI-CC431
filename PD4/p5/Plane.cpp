#include <cmath>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "Plane.h"

using namespace std;

Plane::Plane() {
    prec = 30;
    init();
}

Plane::Plane(int prec) {
    prec = prec;
    init();
}

void Plane::init() {
    numVertices = (prec + 1) * (prec + 1);
    numIndices = prec * prec * 6;
    
    for (int i = 0; i < numVertices; i++) { vertices.push_back(glm::vec3()); }
    for (int i = 0; i < numVertices; i++) { normals.push_back(glm::vec3()); }
    //for (int i = 0; i < numVertices; i++) { tangents.push_back(glm::vec3()); }
    for (int i = 0; i < numIndices; i++) { indices.push_back(0); }
  
    float x = 0;
    float y = 0;
    int idx = 0;
    for(int i=0; i<prec+1; i++) {
	x = 0;
	for(int j=0; j<prec+1; j++) {
	    vertices[idx++] = glm::vec3(x, y, 0.0f);
	    normals[idx]  = glm::vec3(0.0f,0.0f,1.0f);
	    x += 1/(prec+0.0);
	}
	y += 1/(prec+0.0);
    }
    int l=0;
    for(int i=0, idx=0; i<prec; i++, idx++) {
	for(int j=0; j<prec; j++) {
	    //1er triangulo
	    indices[l++] = idx;
	    indices[l++] = idx + 1;
	    indices[l++] = idx + prec + 1;
	    //2do triangulo
	    indices[l++] = idx + 1;
	    indices[l++] = idx + prec + 2;
	    indices[l++] = idx + prec + 1;
	    idx++;
	}
    }
}

int Plane::getNumVertices() { return numVertices; }
int Plane::getNumIndices() { return numIndices; }
std::vector<int> Plane::getIndices() { return indices; }
std::vector<glm::vec3> Plane::getVertices() { return vertices; }
std::vector<glm::vec2> Plane::getTexCoords() { return texCoords; }
std::vector<glm::vec3> Plane::getNormals() { return normals; }
std::vector<glm::vec3> Plane::getTangents() { return tangents; }
