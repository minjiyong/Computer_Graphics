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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;


struct VertexData {
	string filename;

	GLuint VAO;
	GLuint VBOVertex;
	GLuint VBOColor;
	GLuint EBOface;

	vector <glm::vec3> vertex;
	vector <glm::ivec3> face;
	vector <glm::vec3> color;
};

class Importer {
public:
	vector<VertexData*> VertexBuffers;

	void read_newline(char* str);
	void read_obj_file(const std::string& filename, VertexData* model);
	void rearrangeVerticesByFace(VertexData* vertexData);

	void ReadObj();

	void SetupMesh(VertexData* VD);
};