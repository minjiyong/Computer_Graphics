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
#include <math.h>

#include "Importer.h"
#include "ShaderProgram.h"

using namespace std;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800


//--- �Լ� ���� �߰��ϱ�
std::pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WINDOW_WIDTH / 2)) / (WINDOW_WIDTH / 2); //gl��ǥ��� ����
	float my = -((float)y - (WINDOW_HEIGHT / 2)) / (WINDOW_HEIGHT / 2); //gl��ǥ��� ����
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
	return std::sqrt(x * x + y * y);  // �Ǵ� std::sqrt(std::pow(x, 2) + std::pow(y, 2));
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

	glm::vec3 location{};	// ��ġ(translate ����)
	glm::vec3 color{};		// ����

	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };  // ũ�� (����Ʈ�� 1)
	float rotationAngle = 0.0f;  // ȸ�� ���� (����)

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
				scale = glm::vec3(0.5f, 0.5f, 0.5f);  // �⺻ ũ�� ����
				rotationAngle = 0.0f;  // �⺻ ȸ�� ���� ����
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
		rotationAngle = glm::radians(angle);  // �Է��� ����, ���ο����� ���� ���
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

	void Draw_cube() {
		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	void Draw_pyramid() {
		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 18);
		glBindVertexArray(0);
	}
};

vector<Object> cube;
vector<Object> pyramid;

bool check_cube{ true };

bool timer{ false };
bool x_rotate{ true };

bool draw_only_line{ false };
bool draw_enable{ false };

int radian{ 0 };
bool normal_side{ true };

float now_x{};
float now_y{};

int main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	srand(static_cast<int>(time(NULL)));

	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Example14");
	cout << "������ ������" << endl;

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
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

	SP.make_vertexShaders(); //--- ���ؽ� ���̴� �����
	SP.make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
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

//--- �׸��� �ݹ� �Լ�
GLvoid drawScene()
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// x�� (������)
	glUseProgram(0);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(-10.0f, 0.0f, 0.0f); // x�� ���� ��
	glVertex3f(10.0f, 0.0f, 0.0f);  // x�� ������ ��
	glEnd();

	// y�� (���)
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, -10.0f, 0.0f); // y�� �Ʒ��� ��
	glVertex3f(0.0f, 10.0f, 0.0f);  // y�� ���� ��
	glEnd();

	//--- ������ ���������ο� ���̴� �ҷ�����
	glUseProgram(SP.shaderID);

	// ť�� �׸��� (��ȯ ����)
	if (check_cube) {
		for (auto& v : cube) {
			if (x_rotate) v.rotate_by_x();
			else if (!x_rotate) v.rotate_by_y();

			if (draw_enable) {
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				glFrontFace(GL_CCW);
			}
			else if(!draw_enable) glDisable(GL_CULL_FACE);

			if(draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else if(!draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			v.Draw_cube();
		}
	}
	else if (!check_cube) {
		for (auto& v : pyramid) {
			if (x_rotate) v.rotate_by_x();
			else if (!x_rotate) v.rotate_by_y();

			if (draw_enable) {
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				glFrontFace(GL_CCW);
			}
			else if (!draw_enable) glDisable(GL_CULL_FACE);

			if (draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else if (!draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			v.Draw_pyramid();
		}
	}

	glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
}
//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void TimerFunction(int value) {
	if (value == 1) {
		if (timer) {
			if (normal_side) ++radian;
			else if (!normal_side) --radian;

			cube[0].setRotation(radian);
			pyramid[0].setRotation(radian);

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
		pyramid.clear();
		initialize();

		check_cube = true;

		draw_enable = false;
		draw_only_line = false;

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

	else if (key == 'c') {
		check_cube = true;
	}

	else if (key == 'p') {
		check_cube = false;
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
		x_rotate = true;
		normal_side = true;
		timer = true;
	}
	else if (key == 'X') {
		x_rotate = true;
		normal_side = false;
		timer = true;
	}

	else if (key == 'y') {
		x_rotate = false;
		normal_side = true;
		timer = true;
	}

	else if (key == 'Y') {
		x_rotate = false;
		normal_side = false;
		timer = true;
	}


	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		if (check_cube) {
			for (auto& v : cube) v.move_UP();
		}
		else if (!check_cube) {
			for (auto& v : pyramid) v.move_UP();
		}
	}
	else if (key == GLUT_KEY_LEFT) {
		if (check_cube) {
			for (auto& v : cube) v.move_LEFT();
		}
		else if (!check_cube) {
			for (auto& v : pyramid) v.move_LEFT();
		}
	}
	else if (key == GLUT_KEY_DOWN) {
		if (check_cube) {
			for (auto& v : cube) v.move_DOWN();
		}
		else if (!check_cube) {
			for (auto& v : pyramid) v.move_DOWN();
		}
	}
	else if (key == GLUT_KEY_RIGHT) {
		if (check_cube) {
			for (auto& v : cube) v.move_RIGHT();
		}
		else if (!check_cube) {
			for (auto& v : pyramid) v.move_RIGHT();
		}
	}

	glutPostRedisplay();
}

void initialize() {
	Object temp_obj;
	temp_obj.initialize_Object("cube.obj", { 0.0f, 0.0f, 0.0f });
	temp_obj.setScale({ 0.25f, 0.25f, 0.25f });
	cube.push_back(temp_obj);

	temp_obj.initialize_Object("pyramid.obj", { 0.0f, 0.0f, 0.0f });
	pyramid.push_back(temp_obj);
}