// Include GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <SOIL2/soil2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Torus.h"
#include "Cone.h"
#include "Plane.h"
#include "Utils.h"

using namespace std;

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 2
#define numVBOs 4
#define numVBOsc 3

float cameraX, cameraY, cameraZ;
float torLocX, torLocY, torLocZ;
float conLocX, conLocY, conLocZ;

glm::vec3 pln3Loc;
glm::vec3 pln2Loc;
glm::vec3 pln1Loc;

GLuint renderingProgram;
GLuint renderingProgram2;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

GLuint vaoc[numVAOs];
GLuint vboc[numVBOsc];

GLuint vaop1[numVAOs];
GLuint vbop1[numVBOsc];

GLuint vaop2[numVAOs];
GLuint vbop2[numVBOsc];

GLuint vaop3[numVAOs];
GLuint vbop3[numVBOsc];

GLuint vaolc[numVAOs];
GLuint vbolc[numVBOsc];

Cone myCone(100, 2);
Cone lightCone(40, 2);

Plane plane1;
Plane plane2;
Plane plane3;

Torus myTorus(0.5f, 0.2f, 48);
int numTorusVertices = myTorus.getNumVertices();
int numTorusIndices = myTorus.getNumIndices();

glm::vec3 lightLoc = glm::vec3(0.8f, 0.4f, 2.0f);
float amt = 0.0f;
float conScale;
float lightScale;
float plnScale;
// variable allocation for display
GLuint mvLoc, projLoc, nLoc;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc;
GLuint cutLoc, expLoc, locLoc, dirLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat, rMat;
glm::vec3 currentLightPos, transformed;
float lightPos[3];

// white light
float globalAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
// spot light
float spotLightAmbient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
float spotLightDiffuse[4] = { 0.5f, 1.0f, 0.5f, 1.0f };
float spotLightSpecular[4] = { 1.0f,1.0f, 1.0f, 1.0f };
float spotLightLocation[3] = { lightLoc.x, lightLoc.y, lightLoc.z };
float spotLightDirection[3] = { -1.0f*lightLoc.x, -1.0f*lightLoc.y, -1.0f*lightLoc.z };
float spotLightCutoff = glm::cos(glm::radians(10.5f));
float spotLightOutCutoff = glm::cos(glm::radians(15.5f));
float spotLightExponent = 10.0f;
//
// gold material
float* matAmb = Utils::goldAmbient();
float* matDif = Utils::goldDiffuse();
float* matSpe = Utils::goldSpecular();
float matShi = Utils::goldShininess();
// red material
float matAmbr[] = { 0.8283f, 0.1559f, 0.1661f, 1 };
float matDifr[] = { 0.9283f, 0.2559f, 0.1661f, 1 };
float matSper[] = { 0.8283f, 0.2559f, 0.1661f, 1 };
float matShir = 51;

float matAmbp[] = { 0.1559f, 0.6283f, 0.1661f, 1 };
float matDifp[] = { 0.5559f, 0.9283f, 0.2661f, 1 };
float matSpep[] = { 0.2559f, 0.8283f, 0.1661f, 1 };
float matShip = 51;

void installLights(glm::mat4 vMatrix) {
	transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	// get the locations of the light and material fields in the shader
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "globalAmbient"), 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "slight.ambient"), 1, spotLightAmbient);
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "slight.diffuse"), 1, spotLightDiffuse);
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "slight.specular"), 1, spotLightSpecular);
	glProgramUniform3fv(renderingProgram, glGetUniformLocation(renderingProgram, "slight.location"), 1, spotLightLocation);
	glProgramUniform3fv(renderingProgram, glGetUniformLocation(renderingProgram, "slight.direction"), 1, spotLightDirection);
	glProgramUniform1f(renderingProgram, glGetUniformLocation(renderingProgram, "slight.cutoff"), spotLightCutoff);
	glProgramUniform1f(renderingProgram, glGetUniformLocation(renderingProgram, "slight.outcutoff"), spotLightOutCutoff);
	glProgramUniform1f(renderingProgram, glGetUniformLocation(renderingProgram, "slight.exponent"), spotLightExponent);

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "material.ambient"), 1, matAmb);
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "material.diffuse"), 1, matDif);
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "material.specular"), 1, matSpe);
	glProgramUniform1f(renderingProgram, glGetUniformLocation(renderingProgram, "material.shininess"), matShi);
}
void installLightsCone(glm::mat4 vMatrix) {
	transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	// get the locations of the light and material fields in the shader
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "globalAmbient"), 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "slight.ambient"), 1, spotLightAmbient);
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "slight.diffuse"), 1, spotLightDiffuse);
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "slight.specular"), 1, spotLightSpecular);
	glProgramUniform3fv(renderingProgram, glGetUniformLocation(renderingProgram, "slight.location"), 1, spotLightLocation);
	glProgramUniform3fv(renderingProgram, glGetUniformLocation(renderingProgram, "slight.direction"), 1, spotLightDirection);
	glProgramUniform1f(renderingProgram, glGetUniformLocation(renderingProgram, "slight.cutoff"), spotLightCutoff);
	glProgramUniform1f(renderingProgram, glGetUniformLocation(renderingProgram, "slight.outcutoff"), spotLightOutCutoff);
	glProgramUniform1f(renderingProgram, glGetUniformLocation(renderingProgram, "slight.exponent"), spotLightExponent);

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "material.ambient"), 1, matAmbr);
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "material.diffuse"), 1, matDifr);
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "material.specular"), 1, matSper);
	glProgramUniform1f(renderingProgram, glGetUniformLocation(renderingProgram, "material.shininess"), matShir);
}
void installLightsPlane(glm::mat4 vMatrix) {
	transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "globalAmbient"), 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "slight.ambient"), 1, spotLightAmbient);
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "slight.diffuse"), 1, spotLightDiffuse);
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "slight.specular"), 1, spotLightSpecular);
	glProgramUniform3fv(renderingProgram, glGetUniformLocation(renderingProgram, "slight.location"), 1, spotLightLocation);
	glProgramUniform3fv(renderingProgram, glGetUniformLocation(renderingProgram, "slight.direction"), 1, spotLightDirection);
	glProgramUniform1f(renderingProgram, glGetUniformLocation(renderingProgram, "slight.cutoff"), spotLightCutoff);
	glProgramUniform1f(renderingProgram, glGetUniformLocation(renderingProgram, "slight.outcutoff"), spotLightOutCutoff);
	glProgramUniform1f(renderingProgram, glGetUniformLocation(renderingProgram, "slight.exponent"), spotLightExponent);
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "material.ambient"), 1, matAmbp);
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "material.diffuse"), 1, matDifp);
	glProgramUniform4fv(renderingProgram, glGetUniformLocation(renderingProgram, "material.specular"), 1, matSpep);
	glProgramUniform1f(renderingProgram, glGetUniformLocation(renderingProgram, "material.shininess"), matShip);
}

void setupVertices(void) {
	std::vector<int> ind = myTorus.getIndices();
	std::vector<glm::vec3> vert = myTorus.getVertices();
	std::vector<glm::vec2> tex = myTorus.getTexCoords();
	std::vector<glm::vec3> norm = myTorus.getNormals();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	for (int i = 0; i < myTorus.getNumVertices(); i++) {
		pvalues.push_back(vert[i].x);
		pvalues.push_back(vert[i].y);
		pvalues.push_back(vert[i].z);
		tvalues.push_back(tex[i].s);
		tvalues.push_back(tex[i].t);
		nvalues.push_back(norm[i].x);
		nvalues.push_back(norm[i].y);
		nvalues.push_back(norm[i].z);
	}
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);
}
void setupVerticesCone(void) {
	std::vector<int> ind = myCone.getIndices();
	std::vector<glm::vec3> vert = myCone.getVertices();
	std::vector<glm::vec3> norm = myCone.getNormals();

	std::vector<float> pvalues;
	std::vector<float> nvalues;

	for (int i = 0; i < myCone.getNumVertices(); i++) {
		pvalues.push_back(vert[i].x);
		pvalues.push_back(vert[i].y);
		pvalues.push_back(vert[i].z);
		nvalues.push_back(norm[i].x);
		nvalues.push_back(norm[i].y);
		nvalues.push_back(norm[i].z);
	}
	glGenVertexArrays(1, vaoc);
	glBindVertexArray(vaoc[0]);
	glGenBuffers(numVBOsc, vboc);

	glBindBuffer(GL_ARRAY_BUFFER, vboc[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vboc[1]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboc[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);
}
void setupVerticesLightCone(void) {
	std::vector<int> ind = lightCone.getIndices();
	std::vector<glm::vec3> vert = lightCone.getVertices();
	std::vector<glm::vec3> norm = lightCone.getNormals();

	std::vector<float> pvalues;
	std::vector<float> nvalues;

	for (int i = 0; i < lightCone.getNumVertices(); i++) {
		pvalues.push_back(vert[i].x);
		pvalues.push_back(vert[i].y);
		pvalues.push_back(vert[i].z);
		nvalues.push_back(norm[i].x);
		nvalues.push_back(norm[i].y);
		nvalues.push_back(norm[i].z);
	}
	glGenVertexArrays(1, vaolc);
	glBindVertexArray(vaolc[0]);
	glGenBuffers(numVBOsc, vbolc);

	glBindBuffer(GL_ARRAY_BUFFER, vbolc[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbolc[1]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbolc[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);
}
void setupVerticesPlane(void) {
	std::vector<int> ind = plane1.getIndices();
	std::vector<glm::vec3> vert = plane1.getVertices();
	std::vector<glm::vec3> norm = plane1.getNormals();

	std::vector<float> pvalues;
	std::vector<float> nvalues;

	for (int i = 0; i < plane1.getNumVertices(); i++) {
		pvalues.push_back(vert[i].x);
		pvalues.push_back(vert[i].y);
		pvalues.push_back(vert[i].z);
		nvalues.push_back(norm[i].x);
		nvalues.push_back(norm[i].y);
		nvalues.push_back(norm[i].z);
	}
	glGenVertexArrays(1, vaop1);
	glBindVertexArray(vaop1[0]);
	glGenBuffers(numVBOsc, vbop1);

	glBindBuffer(GL_ARRAY_BUFFER, vbop1[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbop1[1]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbop1[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);
}

void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("BlinnPhongShaders/vertShader.glsl", "BlinnPhongShaders/fragShader.glsl");
	renderingProgram2 = Utils::createShaderProgram("src/vshader.glsl", "src/fshader.glsl");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 8.0f;
	torLocX = -1.4f; torLocY = -2.0f; torLocZ = 0.0f;
	conLocX = -2.6f; conLocY = -2.0f; conLocZ = 1.6f;
	pln1Loc = glm::vec3(-10.0f, -10.0f,-14.0f);
	pln2Loc = glm::vec3(-4.0f,-5.0f,15.0f);
	pln3Loc = glm::vec3(-10.0f,-2.0f,5.0f);
	
	conScale = 0.3;
	plnScale = 60;
	lightScale = 0.2f;
	
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices();
	setupVerticesCone();
	setupVerticesPlane();
	setupVerticesLightCone();
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	//mMat *= glm::rotate(mMat, toRadians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	currentLightPos = lightLoc;

	installLights(vMat);
	/*toro*/
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, 		glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, 	glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, 		glm::value_ptr(invTrMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glDrawElements(GL_TRIANGLES, myTorus.getNumIndices(), GL_UNSIGNED_INT, 0);
	
	/*cone*/
	installLightsCone(vMat);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(conLocX, conLocY, conLocZ));
	//mMat *= glm::rotate(mMat, toRadians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(conScale));
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindBuffer(GL_ARRAY_BUFFER, vboc[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vboc[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboc[2]);
	glDrawElements(GL_TRIANGLES, myCone.getNumIndices(), GL_UNSIGNED_INT, 0);
	
	
	/*plane1*/
	//glUseProgram(renderingProgram);
	installLightsPlane(vMat);
	mMat = glm::translate(glm::mat4(1.0f), pln1Loc);
	//mMat *= glm::rotate(mMat, toRadians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(plnScale));
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindBuffer(GL_ARRAY_BUFFER, vbop1[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbop1[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbop1[2]);
	glDrawElements(GL_TRIANGLES, plane1.getNumIndices(), GL_UNSIGNED_INT, 0);

	/*plane2*/
	installLightsPlane(vMat);
	mMat = glm::translate(glm::mat4(1.0f), pln2Loc);
	mMat *= glm::rotate(mMat, toRadians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(plnScale));
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindBuffer(GL_ARRAY_BUFFER, vbop1[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbop1[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbop1[2]);
	glDrawElements(GL_TRIANGLES, plane1.getNumIndices(), GL_UNSIGNED_INT, 0);
	
	//plane3
	installLightsPlane(vMat);
	mMat = glm::translate(glm::mat4(1.0f), pln3Loc);
	mMat *= glm::rotate(mMat, toRadians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(plnScale));
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindBuffer(GL_ARRAY_BUFFER, vbop1[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbop1[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbop1[2]);
	glDrawElements(GL_TRIANGLES, plane1.getNumIndices(), GL_UNSIGNED_INT, 0);
	
	//light cone 
	glUseProgram(renderingProgram2);
	
	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
	
	mMat = glm::inverse(glm::lookAt(lightLoc,
						glm::vec3(0.0f, 0.0f, 0.0f),
						glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec4 vv = 2.0f * glm::vec4(lightLoc,1.0f) - mMat * glm::vec4(0.0f,1.0f,0.0f,1.0f);
	
	mMat = glm::inverse(glm::lookAt(lightLoc,
						glm::vec3(vv.x,vv.y,vv.z),
						glm::vec3(0.0f, 1.0f, 0.0f)));
						
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(lightScale));
	mvMat = vMat * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glBindBuffer(GL_ARRAY_BUFFER, vbolc[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	glDisable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbolc[2]);
	glDrawElements(GL_TRIANGLES, lightCone.getNumIndices(), GL_UNSIGNED_INT, 0);
}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Prob4", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
