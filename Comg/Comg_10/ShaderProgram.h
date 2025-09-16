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

class ShaderProgram {
public:
	GLuint shaderID; //--- 세이더 프로그램 이름
	GLuint vertexShader, fragmentShader; //--- 세이더 객체

	char* filetobuf(const char* file);

	void make_shaderProgram();
	void make_vertexShaders();
	void make_fragmentShaders();
};