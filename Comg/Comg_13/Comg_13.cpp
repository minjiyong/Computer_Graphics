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
void Motion(int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);

float distanceFromOrigin(float x, float y) {
	return std::sqrt(x * x + y * y);  // 또는 std::sqrt(std::pow(x, 2) + std::pow(y, 2));
}
float randomValue() {
	return -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / 2.0f);
}

void initialize();
void obj_bound_check();


Importer importer;
ShaderProgram SP;


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

	void Draw_cube() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);
		
		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 1.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		for (int i = 0; i < 6; ++i) {
			glDrawArrays(GL_TRIANGLES, i * 6, 6);
		}
		glUniform3fv(ObjectColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		glDrawArrays(GL_LINE_LOOP, 0, 36);
		glBindVertexArray(0);
	}

	void Draw_cube_right() {
		setColor({ 1.0f, 1.0f, 0.0f });
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 1.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 6, 3);
		glDrawArrays(GL_TRIANGLES, 24, 3);
		
		glBindVertexArray(0);
	}

	void Draw_cube_left() {
		setColor({ 0.0f, 1.0f, 1.0f });
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 1.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 12, 3);
		glDrawArrays(GL_TRIANGLES, 30, 3);

		glBindVertexArray(0);
	}

	void Draw_cube_front() {
		setColor({ 0.0f, 1.0f, 0.0f });
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 1.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 15, 3);
		glDrawArrays(GL_TRIANGLES, 33, 3);

		glBindVertexArray(0);
	}

	void Draw_cube_bottom() {
		setColor({ 1.0f, 0.0f, 0.0f });
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 1.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 18, 3);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);
	}

	void Draw_cube_back() {
		setColor({ 0.0f, 0.0f, 1.0f });
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 1.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 9, 3);
		glDrawArrays(GL_TRIANGLES, 27, 3);

		glBindVertexArray(0);
	}

	void Draw_cube_top() {
		setColor({ 1.0f, 0.0f, 1.0f });
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 1.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 3, 3);
		glDrawArrays(GL_TRIANGLES, 21, 3);

		glBindVertexArray(0);
	}


	void Draw_pyramid() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 1.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO); 
		for (int i = 0; i < 6; ++i) {
			glDrawArrays(GL_TRIANGLES, i * 3, 18);
		}
		glUniform3fv(ObjectColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
		glDrawArrays(GL_LINE_LOOP, 0, 12);
		glBindVertexArray(0);
	}

	void Draw_pyramid_front() {
		setColor({ 1.0f, 0.0f, 1.0f });
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 1.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		glBindVertexArray(0);
	}

	void Draw_pyramid_right() {
		setColor({ 1.0f, 0.0f, 0.0f });
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 1.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 3, 3);

		glBindVertexArray(0);
	}

	void Draw_pyramid_back() {
		setColor({ 0.0f, 0.0f, 1.0f });
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 1.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 6, 3);

		glBindVertexArray(0);
	}

	void Draw_pyramid_left() {
		setColor({ 0.0f, 1.0f, 1.0f });
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 1.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 9, 3);

		glBindVertexArray(0);
	}

	void Draw_pyramid_bottom() {
		setColor({ 0.0f, 1.0f, 0.0f });
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 1.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 12, 18);

		glBindVertexArray(0);
	}
};

vector<Object> cube;
vector<Object> pyramid;

bool check_cube{ true };
int what_face{ 0 };

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
	glutCreateWindow("Example13");
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
		cout << "Filename: " << v->filename << ", VAO: " << v->VAO << ", Vertices: " << v->vertex.size() << endl;
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

	if (check_cube) {
		if (what_face == 1) for (auto& v : cube) v.Draw_cube_back();
		else if (what_face == 2) for (auto& v : cube) v.Draw_cube_left();
		else if (what_face == 3) for (auto& v : cube) v.Draw_cube_bottom();
		else if (what_face == 4) for (auto& v : cube) v.Draw_cube_top();
		else if (what_face == 5) for (auto& v : cube) v.Draw_cube_right();
		else if (what_face == 6) for (auto& v : cube) v.Draw_cube_front();
		else for (auto& v : cube) v.Draw_cube();
	}
	else if (!check_cube) {
		if (what_face == 1) for (auto& v : pyramid) v.Draw_pyramid_back();
		else if (what_face == 2) for (auto& v : pyramid) v.Draw_pyramid_left();
		else if (what_face == 3) for (auto& v : pyramid) v.Draw_pyramid_bottom();
		else if (what_face == 4) for (auto& v : pyramid) v.Draw_pyramid_right();
		else if (what_face == 5) for (auto& v : pyramid) v.Draw_pyramid_front();
		else for (auto& v : pyramid) v.Draw_pyramid();
	}

	glutSwapBuffers(); //--- 화면에 출력하기
}
//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

float i1 = 0;
void TimerFunction(int value) {
	if (value == 1) {
		cube[0].setRotation(++i1);
		pyramid[0].setRotation(i1);
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

	else if (key == 'r') {
		cube.clear();
		pyramid.clear();
		initialize();

		check_cube = false;
		what_face = 0;
	}

	else if (key == 'a') {
		for (auto& k : importer.VertexBuffers) {
			for (auto& v : k->vertex)
				cout << "vertex x: " << v.x << ", y: " << v.y << ", z: " << v.z << endl;
			for (auto& v : k->face)
				cout << "face x: " << v.x << ", y: " << v.y << ", z: " << v.z << endl;
		}
	}

	else if (key == '`') {
		check_cube = true;
		what_face = 0;
	}
	else if (key == '1') {
		check_cube = true;
		what_face = 1;
	}
	else if (key == '2') {
		check_cube = true;
		what_face = 2;
	}
	else if (key == '3') {
		check_cube = true;
		what_face = 3;
	}
	else if (key == '4') {
		check_cube = true;
		what_face = 4;
	}
	else if (key == '5') {
		check_cube = true;
		what_face = 5;
	}
	else if (key == '6') {
		check_cube = true;
		what_face = 6;
	}
	else if (key == 'c') {
		check_cube = true;
		what_face = rand() % 6 + 1;
	}

	else if (key == '7') {
		check_cube = false;
		what_face = 1;
	}
	else if (key == '8') {
		check_cube = false;
		what_face = 2;
	}
	else if (key == '9') {
		check_cube = false;
		what_face = 3;
	}
	else if (key == '0') {
		check_cube = false;
		what_face = 4;
	}
	else if (key == '-') {
		check_cube = false;
		what_face = 5;
	}
	else if (key == 't') {
		check_cube = false;
		what_face = rand() % 4 + 1;
	}
	else if (key == '=') {
		check_cube = false;
		what_face = 0;
	}


	glutPostRedisplay();
}

void initialize() {
	Object temp_obj;
	temp_obj.initialize_Object("cube.obj", { 0.0f, 0.0f, 0.0f });
	temp_obj.setRotation(10.0f);
	cube.push_back(temp_obj);

	temp_obj.initialize_Object("pyramid.obj", { 0.0f, 0.0f, 0.0f });
	temp_obj.setRotation(10.0f);
	pyramid.push_back(temp_obj);
}