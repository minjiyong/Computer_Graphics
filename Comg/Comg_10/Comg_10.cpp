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
GLvoid Keyboard(unsigned char key, int x, int y);

void tri_bound_check();
float distanceFromOrigin(float x, float y) {
	return std::sqrt(x * x + y * y);  // �Ǵ� std::sqrt(std::pow(x, 2) + std::pow(y, 2));
}


Importer importer, importer_all;
ShaderProgram SP;


class Object {
public:
	GLuint objVAO{};

	int movedCircle{ 0 };

	bool directionChanges{ false };

	float radius{ 0.01f };

	glm::vec3 location{};	// ��ġ(translate ����)
	glm::vec3 color{};		// ����

	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };  // ũ�� (����Ʈ�� 1)
	float rotationAngle = 0.0f;  // ȸ�� ���� (����)

	void initialize_Object(string name, glm::vec3 pos) {
		for (auto& v : importer.VertexBuffers) {
			if (name == v->filename) {
				objVAO = v->VAO;
				location = pos;
				color.r = (float)(rand() % 256) / 255.0f;
				color.g = (float)(rand() % 256) / 255.0f;
				color.b = (float)(rand() % 256) / 255.0f;
				scale = glm::vec3(1.0f, 1.0f, 1.0f);  // �⺻ ũ�� ����
				rotationAngle = 0.0f;  // �⺻ ȸ�� ���� ����
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
		rotationAngle = glm::radians(angle);  // �Է��� ����, ���ο����� ���� ���
	}

	void make_Color_random() {
		color.r = (float)(rand() % 256) / 255.0f;
		color.g = (float)(rand() % 256) / 255.0f;
		color.b = (float)(rand() % 256) / 255.0f;
	}

	void Draw_line() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));  // Z�� ���� ȸ��
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color�� ���̴��� ����
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}

	void Draw_Point() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));  // Z�� ���� ȸ��
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color�� ���̴��� ����
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

int main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	srand(static_cast<int>(time(NULL)));

	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Example10");
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

	SP.make_vertexShaders(); //--- ���ؽ� ���̴� �����
	SP.make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	SP.make_shaderProgram();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);

	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);

	glutMainLoop();
}

//--- �׸��� �ݹ� �Լ�
GLvoid drawScene()
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//--- ������ ���������ο� ���̴� �ҷ�����

	glUseProgram(SP.shaderID);

	for (auto& v : point) v.Draw_Point();

	glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
}
//--- �ٽñ׸��� �ݹ� �Լ�
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

		// �� �� �߰�
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