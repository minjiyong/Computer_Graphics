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
#include <math.h>

#include "Importer.h"
#include "ShaderProgram.h"

using namespace std;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800


//--- 함수 선언 추가하기
std::pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WINDOW_WIDTH / 2)) / (WINDOW_WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (WINDOW_HEIGHT / 2)) / (WINDOW_HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

void TimerFunction(int value);
void Mouse(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);

void tri_bound_check();
float distanceFromOrigin(float x, float y) {
	return std::sqrt(x * x + y * y);  // 또는 std::sqrt(std::pow(x, 2) + std::pow(y, 2));
}

void initialize();


Importer importer, importer_all;
ShaderProgram SP;


class Object {
public:
	GLuint objVAO{};
	string objName{};

	float radius{ 0.01f };

	glm::vec3 location{};	// 위치(translate 적용)
	glm::vec3 color{};		// 색상

	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };  // 크기 (디폴트로 1)
	float rotationAngle = 0.0f;  // 회전 각도 (라디안)

	void initialize_Object(string name, glm::vec3 pos) {
		for (auto& v : importer.VertexBuffers) {
			if (name == v->filename) {
				objVAO = v->VAO;
				objName = name;
				location = pos;
				color.r = (float)(rand() % 256) / 255.0f;
				color.g = (float)(rand() % 256) / 255.0f;
				color.b = (float)(rand() % 256) / 255.0f;
				scale = glm::vec3(1.0f, 1.0f, 1.0f);  // 기본 크기 설정
				rotationAngle = 0.0f;  // 기본 회전 각도 설정
			}
		}
	}

	void setVAO() {
		for (auto& v : importer.VertexBuffers) {
			if (objName == v->filename) {
				objVAO = v->VAO;
			}
		}
	}

	void setScale(const glm::vec3& newScale) {
		scale = newScale;
	}

	void setScale_random() {
		float temp = (float)(rand() % 5) * 0.1f + 1.0f;
		scale = { temp, temp, temp };
	}

	void PlusScale() {
		scale.x += 0.1f;
		scale.y += 0.1f;
	}

	void setRotation(float angle) {
		rotationAngle = glm::radians(angle);  // 입력은 각도, 내부에서는 라디안 사용
	}

	void make_Color_random() {
		color.r = (float)(rand() % 256) / 255.0f;
		color.g = (float)(rand() % 256) / 255.0f;
		color.b = (float)(rand() % 256) / 255.0f;
	}

	void Draw_line() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 기준 회전
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glLineWidth(3.0f);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}

	void Draw_triangle() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 기준 회전
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glPointSize(3.0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
		glBindVertexArray(0);
	}

	void Draw_rectangle() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 기준 회전
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glPointSize(3.0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	void Draw_pentagon() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 기준 회전
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glPointSize(3.0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
		glBindVertexArray(0);
	}
};

vector<Object> line;
vector<Object> triangle;
vector<Object> rect;
vector<Object> penta;

Object middle_obj;
glm::vec3 middle_obj_color{};

Object xy_line[2];

int check{ 0 };

bool check_line{ false };

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	srand(static_cast<int>(time(NULL)));

	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Example11");
	cout << "윈도우 생성됨" << endl;

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized" << std::endl;

	importer.ReadObj();

	SP.make_vertexShaders(); //--- 버텍스 세이더 만들기
	SP.make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	SP.make_shaderProgram();

	initialize();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);

	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);

	glutMainLoop();
}

//--- 그리기 콜백 함수
GLvoid drawScene()
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//--- 렌더링 파이프라인에 세이더 불러오기

	glUseProgram(SP.shaderID);

	for (int i = 0; i < 2; ++i) xy_line[i].Draw_line();

	if (check == 0) {
		for (auto& v : line) v.Draw_line();
		for (auto& v : triangle) v.Draw_triangle();
		for (auto& v : rect) v.Draw_rectangle();
		for (auto& v : penta) v.Draw_pentagon();
	}
	else if (check == 1) {
		if (middle_obj.objName == "Line") middle_obj.Draw_line();
		else if (middle_obj.objName == "Triangle") middle_obj.Draw_triangle();
		else if (middle_obj.objName == "Rectangle") middle_obj.Draw_rectangle();
		else if (middle_obj.objName == "Pentagon") middle_obj.Draw_pentagon();
	}

	glutSwapBuffers(); //--- 화면에 출력하기
}
//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}


void TimerFunction(int value) {
	if (value == 1) {
		
		glutTimerFunc(17, TimerFunction, 1);
	}

	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		auto mouse = ConvertWinToGL(x, y);

	
		
	}

	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	if (key == 'q') {
		glutDestroyWindow(true);
	}

	else if (key == 'a') {
		check = 0;
		line.clear();
		triangle.clear();
		rect.clear();
		penta.clear();
		initialize();
	}

	else if (key == 's') {
		Object temp_object;

		temp_object.initialize_Object("Line", { 0.0f, 0.0f, 0.0f });
		temp_object.location = penta.front().location;
		temp_object.color = penta.front().color;
		line.push_back(temp_object);

		temp_object.initialize_Object("Triangle", { 0.0f, 0.0f, 0.0f });
		temp_object.location = line.front().location;
		temp_object.color = line.front().color;
		triangle.push_back(temp_object);

		temp_object.initialize_Object("Rectangle", { 0.0f, 0.0f, 0.0f });
		temp_object.location = triangle.front().location;
		temp_object.color = triangle.front().color;
		rect.push_back(temp_object);

		temp_object.initialize_Object("Pentagon", { 0.0f, 0.0f, 0.0f });
		temp_object.location = rect.front().location;
		temp_object.color = rect.front().color;
		penta.push_back(temp_object);

		line.erase(line.begin());
		triangle.erase(triangle.begin());
		rect.erase(rect.begin());
		penta.erase(penta.begin());
	}

	else if (key == 'l') {
		check = 1;
		middle_obj.initialize_Object("Triangle", { 0.0f, 0.0f, 0.0f });
		middle_obj.setScale({ 2.5f, 2.5f, 0.0f });
		middle_obj.color = middle_obj_color;

		while (importer.VertexBuffers[0]->vertex[0].y > 0.0f)
			importer.VertexBuffers[0]->vertex[0].y -= 0.01f;

		middle_obj.setVAO();
	}

	else if (key == 't') {
		check = 1;
		middle_obj.initialize_Object("Triangle", { 0.0f, 0.0f, 0.0f });
		middle_obj.setScale({ 2.5f, 2.5f, 0.0f });
		middle_obj.color = middle_obj_color;
	}

	else if (key == 'r') {
		check = 1;
		middle_obj.initialize_Object("Rectangle", { 0.0f, 0.0f, 0.0f });
		middle_obj.setScale({ 2.5f, 2.5f, 0.0f });
		middle_obj.color = middle_obj_color;
	}

	else if (key == 'p') {
		check = 1;
		middle_obj.initialize_Object("Pentagon", { 0.0f, 0.0f, 0.0f });
		middle_obj.setScale({ 2.5f, 2.5f, 0.0f });
		middle_obj.color = middle_obj_color;
	}

	glutPostRedisplay();
}

void initialize() {
	xy_line[0].initialize_Object("xy_Line", { 0.0f, 0.0f, 0.0f });
	xy_line[1].initialize_Object("xy_Line", { 0.0f, 0.0f, 0.0f });
	xy_line[1].setRotation(90);

	Object temp_object;
	temp_object.initialize_Object("Line", { -0.5f, 0.5f, 0.0f });
	line.push_back(temp_object);

	temp_object.initialize_Object("Triangle", { 0.5f, 0.5f, 0.0f });
	triangle.push_back(temp_object);

	temp_object.initialize_Object("Rectangle", { -0.5f, -0.5f, 0.0f });
	rect.push_back(temp_object);

	temp_object.initialize_Object("Pentagon", { 0.5f, -0.5f, 0.0f });
	penta.push_back(temp_object);

	middle_obj_color.r = (float)(rand() % 256) / 255.0f;
	middle_obj_color.g = (float)(rand() % 256) / 255.0f;
	middle_obj_color.b = (float)(rand() % 256) / 255.0f;
}