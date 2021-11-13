#include <cmath>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "Cone.h"

using namespace std;

Cone::Cone() {
	init(48, 2);
}

Cone::Cone(int prec) {
	init(prec, 2);
}
Cone::Cone(int prec, int hg) {
	init(prec, hg);
}

float Cone::toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

void Cone::init(int prec, int hg) {
    h = hg;
	numVertices = prec * 2 + 1;
	numIndices = prec * 6;
	for (int i = 0; i < numVertices; i++) { vertices.push_back(glm::vec3()); }
	for (int i = 0; i < numVertices; i++) { normals.push_back(glm::vec3()); }
	for (int i = 0; i < numVertices; i++) { tangents.push_back(glm::vec3()); }
	for (int i = 0; i < numIndices; i++) { indices.push_back(0); }

    vertices[0] = glm::vec3(0.0f, 0.0f, 0.0f);
    
    glm::vec3 P1, P2, P3;
    
	int i = 1;
    
	for (int j = 1; j <= prec; j++) {
        
        float theta1 = toRadians( j * 360.0f / (prec + 0.0f) );
        float theta0 = toRadians( (j - 1) * 360.0f / (prec + 0.0f) );
        
        P1 = glm::vec3( cos(theta1), 0, -1*sin(theta1) );
        P2 = glm::vec3( cos(theta0), 0, -1*sin(theta0) );        
        P3 = glm::vec3(0, h, 0);
        
        normals[i]      = glm::cross(P3 - P1, P2 - P1);
        normals[i + 1]  = glm::cross(P2 - P3, P1 - P3);
        
        vertices[i]     = P1;
        vertices[i + 1] = P3;
        
        i += 2;
	}
    int k = 0;
	for (int i = 1; i <= prec; i++) {
        if (i == 1) {
            indices[k] = 2*prec - 1;
            indices[k+1] = 2*i - 1;
            indices[k+2] = 2*i;
            indices[k+3] = 2*prec - 1;
            indices[k+4] = 2*i - 1;
            indices[k+5] = 0;
        } else {
            indices[k] = 2*i - 3;
            indices[k+1] = 2*i - 1;
            indices[k+2] = 2*i;
            indices[k+3] = 2*i - 3;
            indices[k+4] = 2*i - 1;
            indices[k+5] = 0;
        }
        
        k += 6;
	}
}

int Cone::getNumVertices() { return numVertices; }
int Cone::getNumIndices() { return numIndices; }
std::vector<int> Cone::getIndices() { return indices; }
std::vector<glm::vec3> Cone::getVertices() { return vertices; }
std::vector<glm::vec2> Cone::getTexCoords() { return texCoords; }
std::vector<glm::vec3> Cone::getNormals() { return normals; }
std::vector<glm::vec3> Cone::getTangents() { return tangents; }
