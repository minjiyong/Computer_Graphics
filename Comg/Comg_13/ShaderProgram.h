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

class ShaderProgram {
public:
	GLuint shaderID; //--- ���̴� ���α׷� �̸�
	GLuint vertexShader, fragmentShader; //--- ���̴� ��ü

	char* filetobuf(const char* file);

	void make_shaderProgram();
	void make_vertexShaders();
	void make_fragmentShaders();
};