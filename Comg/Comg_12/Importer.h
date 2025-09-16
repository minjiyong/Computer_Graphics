#pragma once
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
//--- 메인 함수
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <random>
#include <utility>
#include <string>

using namespace std;

struct VertexData {
	string filename;

	GLuint VAO;
	GLuint VBOVertex;
	GLuint VBOColor;

	vector <glm::vec3> vertex;
	vector <glm::vec3> color;
};

class Importer {
public:
	vector<VertexData*> VertexBuffers;

	void GetShapeData() {
	}

	void ReadObj();

	void SetupMesh(VertexData* VD);
};