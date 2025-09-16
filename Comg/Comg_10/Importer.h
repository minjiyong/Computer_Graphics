#pragma once
#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
//--- ���� �Լ�
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glew.h> //--- �ʿ��� ������� include
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