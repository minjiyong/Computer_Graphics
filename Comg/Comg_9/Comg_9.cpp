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

#define STOP 0

#define UP 1
#define DOWN 2

#define LEFT 3
#define RIGHT 4


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


Importer importer;
ShaderProgram SP;


class Object {
public:
	GLuint objVAO{};

	int dir_ud{};
	int dir_lr{};
	int step{ 1 };
	int movedSteps{ 0 };
	int directionChanges = 0;

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

				dir_ud = rand() % 2 + 1;	// UP or DOWN
				dir_lr = rand() % 2 + 3;	// LEFT or RIGHT
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

	void Draw_triangle() {
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
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
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

	void Draw_triangle_onlyLine() {
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
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
	}
};

vector<Object> triangle;
Object line[2];

bool timer1{ false };
bool timer2{ false };
bool timer3{ false };
bool timer4{ false };

bool check1{ false };
bool check2{ false };
bool check3{ false };
bool check4{ false };

bool check_line{ false };
bool check_fill{ true };

int main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	srand(static_cast<int>(time(NULL)));

	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Example9");
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

	line[0].initialize_Object("Line", { 0.0f, 0.0f, 0.0f });
	line[1].initialize_Object("Line", { 0.0f, 0.0f, 0.0f });
	line[1].setRotation(90);

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

	for (int i = 0; i < 2; ++i) line[i].Draw_line();
	if (check_fill) for (auto& v : triangle) v.Draw_triangle();
	else if (check_line) for (auto& v : triangle) v.Draw_triangle_onlyLine();

	glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
}
//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}


void TimerFunction(int value) {
	if (value == 1) {
		for (auto& t : triangle) {
			if (t.dir_ud == UP && t.dir_lr == LEFT) {
				t.location.x -= 0.01f;
				t.location.y += 0.01f;
			}
			else if (t.dir_ud == UP && t.dir_lr == RIGHT) {
				t.location.x += 0.01f;
				t.location.y += 0.01f;
			}
			else if (t.dir_ud == DOWN && t.dir_lr == LEFT) {
				t.location.x -= 0.01f;
				t.location.y -= 0.01f;
			}
			else if (t.dir_ud == DOWN && t.dir_lr == RIGHT) {
				t.location.x += 0.01f;
				t.location.y -= 0.01f;
			}
		}
		tri_bound_check();
		if (timer1) glutTimerFunc(17, TimerFunction, 1);
	}

	else if (value == 2) {
		for (auto& t : triangle) {
			if (t.dir_lr == LEFT && t.location.x > -1.0f) {
				t.location.x -= 0.01f;
			}
			else if (t.dir_lr == RIGHT && t.location.x < 1.0f) {
				t.location.x += 0.01f;
			}

			if (t.location.x <= -1.0f || t.location.x >= 1.0f) {
				if (t.dir_ud == UP) {
					t.location.y += 0.1f;
				}
				else if (t.dir_ud == DOWN) {
					t.location.y -= 0.1f;
				}
			}
		}
		tri_bound_check();
		if (timer2) glutTimerFunc(17, TimerFunction, 2);
	}

	else if (value == 3) {
		for (auto& t : triangle) {
			if (t.dir_lr == LEFT) {
				t.setRotation(90);
				t.location.x -= 0.01f;  // �� ���� �� �ܰ辿 �̵�
				++t.movedSteps;
				if (t.movedSteps >= t.step) {
					t.dir_lr = STOP;
					t.dir_ud = UP;
					t.movedSteps = 0;  // �̵��� �ܰ� �ʱ�ȭ
					++t.directionChanges;
				}
			}
			else if (t.dir_lr == RIGHT) {
				t.setRotation(270);
				t.location.x += 0.01f;
				++t.movedSteps;
				if (t.movedSteps >= t.step) {
					t.dir_lr = STOP;
					t.dir_ud = DOWN;
					t.movedSteps = 0;
					++t.directionChanges;
				}
			}
			else if (t.dir_ud == UP) {
				t.setRotation(0);
				t.location.y += 0.01f;
				++t.movedSteps;
				if (t.movedSteps >= t.step) {
					t.dir_lr = RIGHT;
					t.dir_ud = STOP;
					t.movedSteps = 0;
					++t.directionChanges;
				}
			}
			else if (t.dir_ud == DOWN) {
				t.setRotation(180);
				t.location.y -= 0.01f;
				++t.movedSteps;
				if (t.movedSteps >= t.step) {
					t.dir_lr = LEFT;
					t.dir_ud = STOP;
					t.movedSteps = 0;
					++t.directionChanges;
				}
			}

			if (t.directionChanges % 2 == 1) {
				++t.step;
				t.directionChanges = 0;
			}
		}
		tri_bound_check();
		if (timer3) glutTimerFunc(17, TimerFunction, 3);
	}

	else if (value == 4) {
		for (auto& t : triangle) {
			t.location.x += cos(t.rotationAngle) * t.radius;
			t.location.y += sin(t.rotationAngle) * t.radius;

			t.rotationAngle += 0.05f;
			t.radius += 0.0001f;

			if (t.rotationAngle >= 2 * 3.14159f) {
				t.rotationAngle -= 2 * 3.14159f;
			}
		}

		tri_bound_check();
		if (timer4) glutTimerFunc(17, TimerFunction, 4);
	}

	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		auto mouse = ConvertWinToGL(x, y);

		// �� �ﰢ�� �߰�
		Object temp_triangle;
		temp_triangle.initialize_Object("Triangle", { mouse.first, mouse.second, 0.0f });
		triangle.push_back(temp_triangle);
	}

	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	if (key == 'q') {
		glutDestroyWindow(true);
	}

	else if (key == 'a') {
		check_line = true;
		check_fill = false;
	}
	else if (key == 'b') {
		check_fill = true;
		check_line = false;
	}

	else if (key == '1') {
		if (!check1) {
			timer1 = true;
			glutTimerFunc(17, TimerFunction, 1);
			check1 = true;
		}
		else if (check1) {
			timer1 = false;
			check1 = false;
		}
	}

	else if (key == '2') {
		if (!check2) {
			timer2 = true;
			glutTimerFunc(17, TimerFunction, 2);
			check2 = true;
		}
		else if (check2) {
			timer2 = false;
			check2 = false;
		}
	}

	else if (key == '3') {
		if (!check3) {
			timer3 = true;
			glutTimerFunc(17, TimerFunction, 3);
			check3 = true;
		}
		else if (check3) {
			timer3 = false;
			check3 = false;
		}
	}

	else if (key == '4') {
		if (!check4) {
			timer4 = true;
			glutTimerFunc(17, TimerFunction, 4);
			check4 = true;
		}
		else if (check4) {
			timer4 = false;
			check4 = false;
		}
	}

	glutPostRedisplay();
}

void tri_bound_check()
{
	for (auto& t : triangle) {
		if (t.location.y >= 1.0f) {
			t.dir_ud = DOWN;
			t.setRotation(180);
		}
		else if (t.location.y <= -1.0f) {
			t.dir_ud = UP;
			t.setRotation(0);
		}
		else if (t.location.x <= -1.0f) {
			t.dir_lr = RIGHT;
			t.setRotation(270);
		}
		else if (t.location.x >= 1.0f) {
			t.dir_lr = LEFT;
			t.setRotation(90);
		}
	}
}