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

#define ALL 0
#define CUBE 1
#define CONE 2


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
void Motion(int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
void SpecialKeyboard(int key, int x, int y);

float distanceFromOrigin(float x, float y) {
	return std::sqrt(x * x + y * y);  // 또는 std::sqrt(std::pow(x, 2) + std::pow(y, 2));
}
float randomValue() {
	return -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / 2.0f);
}

void initialize();
void initialize_2();
void obj_bound_check();


Importer importer;
ShaderProgram SP;


class Camera {
public:
	glm::vec3 cameraPos = glm::vec3(3.0f, 3.0f, 3.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float fov = 45.0f;
	float aspectRatio = 800.0f / 800.0f;
	float nearClip = 0.1f;
	float farClip = 100.0f;

	void make_camera() {
		glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
		glm::mat4 projection = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
		glUniformMatrix4fv(glGetUniformLocation(SP.shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SP.shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	}
};

class Object {
public:
	static bool del;
	GLuint objVAO{};
	string objName{};

	glm::vec3 location{};	// 위치(translate 적용)
	glm::vec3 color{};		// 색상

	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };  // 크기 (디폴트로 1)
	float rotationAngle = 0.0f;  // 회전 각도 (라디안)

	void initialize_Object(string name, glm::vec3 pos) {
		for (auto& v : importer.VertexBuffers) {
			if (name == v->filename) {
				objVAO = v->VAO;
				cout << "Object VAO: " << objVAO << endl;
				objName = name;
				location = pos;
				color.r = (float)(rand() % 256) / 255.0f;
				color.g = (float)(rand() % 256) / 255.0f;
				color.b = (float)(rand() % 256) / 255.0f;
				scale = glm::vec3(0.5f, 0.5f, 0.5f);  // 기본 크기 설정
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

	void move_UP() {
		location.y += 0.11f;
	}
	void move_LEFT() {
		location.x -= 0.11f;
	}
	void move_DOWN() {
		location.y -= 0.11f;
	}
	void move_RIGHT() {
		location.x += 0.11f;
	}

	void setScale(const glm::vec3& newScale) {
		scale = newScale;
	}

	void PlusScale() {
		scale.x += 0.1f;
		scale.y += 0.1f;
	}

	void setRotation(float angle) {
		rotationAngle = glm::radians(angle);  // 입력은 각도, 내부에서는 라디안 사용
	}

	void setColor(const glm::vec3& newcolor) {
		color = newcolor;
	}

	void make_Color_random() {
		color.r = (float)(rand() % 256) / 255.0f;
		color.g = (float)(rand() % 256) / 255.0f;
		color.b = (float)(rand() % 256) / 255.0f;
	}

	void rotate_by_x() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));
	}

	void rotate_by_y() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));
	}

	void revolve_by_y() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));
	}

	void Draw_cube() {
		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	void Draw_cone() {
		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 144);
		glBindVertexArray(0);
	}

	void Draw_pyramid() {
		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 18);
		glBindVertexArray(0);
	}

	void Draw_rifle() {
		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 4926);
		glBindVertexArray(0);
	}

	void Draw_line() {
		// 축을 그릴 때 변환을 적용하지 않음
		glm::mat4 transform_Matrix = glm::mat4(1.0f);
		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glLineWidth(2.0f);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}
};

Camera camera;

Object xyz_line[3];

vector<Object> cube;
vector<Object> cone;

int what_shape{ ALL };

bool timer{ false };
bool x_rotate{ true };

bool revolve{ false };

bool draw_only_line{ false };
bool draw_enable{ false };

int radian{ 0 };
bool normal_side{ true };

float now_x{};
float now_y{};

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	srand(static_cast<int>(time(NULL)));

	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Example15");
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
	for (auto& v : importer.VertexBuffers) {
		cout << "Filename: " << v->filename << ", VAO: " << v->VAO << ", Vertices: " << v->vertex.size() << ", Colors: " << v->color.size() << endl;
	}

	SP.make_vertexShaders(); //--- 버텍스 세이더 만들기
	SP.make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	SP.make_shaderProgram();

	initialize();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);

	glutTimerFunc(17, TimerFunction, 1);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);

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


	xyz_line[0].Draw_line();
	xyz_line[1].Draw_line();
	xyz_line[2].Draw_line();

	// 큐브 그리기 (변환 적용)
	for (auto& v : cube) {
		if (!revolve) {
			if (x_rotate) v.rotate_by_x();
			else if (!x_rotate) v.rotate_by_y();
		}
		else if (revolve) v.revolve_by_y();

		if (draw_enable) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);
		}
		else if (!draw_enable) glDisable(GL_CULL_FACE);

		if (draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else if (!draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if(v.objName == "cube.obj") v.Draw_cube();
		else if(v.objName == "pyramid.obj") v.Draw_pyramid();
	}
	
	for (auto& v : cone) {
		if (!revolve) {
			if (x_rotate) v.rotate_by_x();
			else if (!x_rotate) v.rotate_by_y();
		}
		else if (revolve) v.revolve_by_y();

		if (draw_enable) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);
		}
		else if (!draw_enable) glDisable(GL_CULL_FACE);

		if (draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else if (!draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (v.objName == "cone.obj") v.Draw_cone();
		else if (v.objName == "rifle.obj") v.Draw_rifle();
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
		if (timer) {
			if (normal_side) ++radian;
			else if (!normal_side) --radian;

			if (what_shape == ALL || what_shape == CUBE) {
				cube[0].setRotation(radian);
			}
			if (what_shape == ALL || what_shape == CONE) {
				cone[0].setRotation(radian);
			}

		}
		glutTimerFunc(17, TimerFunction, 1);
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		auto mouse = ConvertWinToGL(x, y);
		now_x = mouse.first;
		now_y = mouse.second;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		auto mouse = ConvertWinToGL(x, y);

		glutPostRedisplay();
	}
}

void Motion(int x, int y) {
	auto mouse = ConvertWinToGL(x, y);
	float deltaX{ mouse.first - now_x };
	float deltaY{ mouse.second - now_y };

	now_x = mouse.first;
	now_y = mouse.second;

	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	if (key == 'q') {
		glutDestroyWindow(true);
	}

	else if (key == 's') {
		cube.clear();
		cone.clear();
		initialize();

		what_shape = ALL;

		draw_enable = false;
		draw_only_line = false;

		revolve = false;

		timer = false;
		x_rotate = true;

		radian = 0;
		normal_side = true;
	}

	else if (key == 'a') {
		for (auto& k : importer.VertexBuffers) {
			for (auto& v : k->vertex)
				cout << "vertex x: " << v.x << ", y: " << v.y << ", z: " << v.z << endl;
			for (auto& v : k->face)
				cout << "face x: " << v.x << ", y: " << v.y << ", z: " << v.z << endl;
			for (auto& v : k->color)
				cout << "color r: " << v.x << ", g: " << v.y << ", b: " << v.z << endl;
		}
	}


	else if (key == '1') {
		what_shape = CUBE;
	}
	else if (key == '2') {
		what_shape = CONE;
	}
	else if (key == '3') {
		what_shape = ALL;
	}

	else if (key == 'r') {
		revolve = true;
		normal_side = true;
		timer = true;
	}
	else if (key == 'R') {
		revolve = true;
		normal_side = false;
		timer = true;
	}
	
	else if (key == 'h') {
		draw_enable = true;
	}
	else if (key == 'H') {
		draw_enable = false;
	}

	else if (key == 'w') {
		draw_only_line = true;
	}
	else if (key == 'W') {
		draw_only_line = false;
	}

	else if (key == 'x') {
		revolve = false;
		x_rotate = true;
		normal_side = true;
		timer = true;
	}
	else if (key == 'X') {
		revolve = false;
		x_rotate = true;
		normal_side = false;
		timer = true;
	}

	else if (key == 'y') {
		revolve = false;
		x_rotate = false;
		normal_side = true;
		timer = true;
	}

	else if (key == 'Y') {
		revolve = false;
		x_rotate = false;
		normal_side = false;
		timer = true;
	}

	else if (key == 'c') {
		cube.clear();
		cone.clear();
		initialize_2();

		what_shape = ALL;

		draw_enable = false;
		draw_only_line = false;

		revolve = false;

		timer = false;
		x_rotate = true;

		radian = 0;
		normal_side = true;
	}

	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y) {


	glutPostRedisplay();
}

void initialize() {
	camera.make_camera();

	xyz_line[0].initialize_Object("x_Line", { 0.0f, 0.0f, 0.0f });
	xyz_line[1].initialize_Object("y_Line", { 0.0f, 0.0f, 0.0f });
	xyz_line[2].initialize_Object("z_Line", { 0.0f, 0.0f, 0.0f });

	Object temp_obj;
	temp_obj.initialize_Object("cube.obj", { 1.0f, 0.0f, 0.0f });
	temp_obj.setScale({ 0.25f, 0.25f, 0.25f });
	cube.push_back(temp_obj);

	temp_obj.initialize_Object("cone.obj", { -1.0f, 0.0f, 0.0f });
	cone.push_back(temp_obj);
}

void initialize_2() {
	Object temp_obj;
	temp_obj.initialize_Object("pyramid.obj", { 1.0f, 0.0f, 0.0f });
	cube.push_back(temp_obj);

	temp_obj.initialize_Object("rifle.obj", { -1.0f, 0.0f, 0.0f });
	cone.push_back(temp_obj);
}