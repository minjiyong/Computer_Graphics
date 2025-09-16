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


Importer importer, importer_all;
ShaderProgram SP;


class Object {
public:
	GLuint objVAO{};

	int movedCircle{ 0 };

	bool directionChanges{ false };

	float radius{ 0.01f };

	glm::vec3 location{};	// 위치(translate 적용)
	glm::vec3 color{};		// 색상

	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };  // 크기 (디폴트로 1)
	float rotationAngle = 0.0f;  // 회전 각도 (라디안)

	void initialize_Object(string name, glm::vec3 pos) {
		for (auto& v : importer.VertexBuffers) {
			if (name == v->filename) {
				objVAO = v->VAO;
				location = pos;
				color.r = (float)(rand() % 256) / 255.0f;
				color.g = (float)(rand() % 256) / 255.0f;
				color.b = (float)(rand() % 256) / 255.0f;
				scale = glm::vec3(1.0f, 1.0f, 1.0f);  // 기본 크기 설정
				rotationAngle = 0.0f;  // 기본 회전 각도 설정
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
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}

	void Draw_Point() {
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
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);
	}
};

vector<Object> point;
vector<Object> new_point;

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
	glutCreateWindow("Example10");
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

	for (auto& v : point) v.Draw_Point();

	glutSwapBuffers(); //--- 화면에 출력하기
}
//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}


void TimerFunction(int value) {
	if (value == 1) {
		if (point.size() == 0) return;

		Object temp_point;
		temp_point.initialize_Object("Point", { 0.0f, 0.0f, 0.0f });
		temp_point.radius = point.back().radius;
		temp_point.directionChanges = point.back().directionChanges;
		temp_point.movedCircle = point.back().movedCircle;

		if (temp_point.radius >= 0.01f) {
			if (!temp_point.directionChanges) {
				temp_point.location.x = cos(point.back().rotationAngle) * point.back().radius + point.back().location.x;
				temp_point.location.y = sin(point.back().rotationAngle) * point.back().radius + point.back().location.y;

				temp_point.rotationAngle = point.back().rotationAngle + 0.1f;
				temp_point.radius += 0.0001f;
				if (temp_point.radius >= 0.05f) temp_point.directionChanges = true;
			}
			else if (temp_point.directionChanges) {
				temp_point.location.x = cos(point.back().rotationAngle) * point.back().radius + point.back().location.x;
				temp_point.location.y = sin(point.back().rotationAngle) * point.back().radius + point.back().location.y;

				temp_point.rotationAngle = point.back().rotationAngle - 0.1f;
				temp_point.radius -= 0.0001f;
			}
		}
		else if (temp_point.radius < 0.01f) return;

		if (temp_point.rotationAngle >= 2 * 3.14159f) {
			++temp_point.movedCircle;
			if (check == 1 && temp_point.movedCircle >= 1) temp_point.directionChanges = true;
			else if (check == 2 && temp_point.movedCircle >= 2) temp_point.directionChanges = true;
			else if (check == 3 && temp_point.movedCircle >= 3) temp_point.directionChanges = true;
			else if (check == 4 && temp_point.movedCircle >= 4) temp_point.directionChanges = true;
			else if (check == 5 && temp_point.movedCircle >= 5) temp_point.directionChanges = true;
			temp_point.rotationAngle -= 2 * 3.14159f;
		}

		point.push_back(temp_point);
		
		glutTimerFunc(17, TimerFunction, 1);
	}

	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		auto mouse = ConvertWinToGL(x, y);

		// 새 점 추가
		Object temp_point;
		temp_point.initialize_Object("Point", { mouse.first, mouse.second, 0.0f });
		point.push_back(temp_point);
		cout << point.back().location.x << ", " << point.back().location.y << endl;
	}

	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	if (key == 'q') {
		glutDestroyWindow(true);
	}

	else if (key == 's') {
		glutTimerFunc(17, TimerFunction, 1);
	}

	else if (key == 'l') {
		check_line = true;

	}
	else if (key == 'p') {
		check_line = false;
	}

	else if (key == '1') {
		check = 1;
		cout << "check : " << check << endl;
	}

	else if (key == '2') {
		check = 2;
		cout << "check : " << check << endl;
	}

	else if (key == '3') {
		check = 3;
		cout << "check : " << check << endl;
	}

	else if (key == '4') {
		check = 4;
		cout << "check : " << check << endl;
	}

	else if (key == '5') {
		check = 5;
		cout << "check : " << check << endl;
	}

	glutPostRedisplay();
}