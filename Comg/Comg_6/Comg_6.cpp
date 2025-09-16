#include <iostream>
#include <random>
#include <stdlib.h>
#include <vector>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define UP 1
#define LEFT 2
#define DOWN 3
#define RIGHT 4
#define Q_SIDE 5
#define E_SIDE 6
#define Z_SIDE 7
#define C_SIDE 8

GLvoid drawScene(GLvoid);
void TimerFunction(int value);
void Mouse(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Reshape(int w, int h);

void initialize();

class Rect {
public:
	float x1{};
	float y1{};
	float x2{};
	float y2{};

	float red{};
	float green{};
	float blue{};

	int dir{};

	bool selected{ false };
	bool animate{ false };
	

	Rect() {}
	Rect(float x1, float y1, float x2, float y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}
	~Rect() {}


	void initialize() {
		x1 = (float)(rand() % 700);
		y1 = (float)(rand() % 700 + 100);
		x2 = x1 + 200;
		y2 = y1 - 200;

		red = (float)(rand() % 256) / 255.0f;
		green = (float)(rand() % 256) / 255.0f;
		blue = (float)(rand() % 256) / 255.0f;

		selected = false;
		animate = false;
	}

	void make_same_color(const Rect& rect) {
		red = rect.red;
		green = rect.green;
		blue = rect.blue;
	}

	void make_smaller() {
		++x1;
		--y1;
		--x2;
		++y2;
	}
};

vector<Rect> rect;

float now_x{};
float now_y{};

bool mouse_down{ false };

bool check1{ false };
bool check2{ false };
bool check3{ false };
bool check4{ false };


void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ���� 
{
	srand(static_cast<int>(time(NULL)));
	initialize();

	//--- ������ �����ϱ�
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(100, 100); // �������� ��ġ ����
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // �������� ũ�� ����
	glutCreateWindow("Example6"); // ������ ����(������ �̸�)

	gluOrtho2D(0.0, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0);

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else std::cout << "GLEW Initialized\n";

	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutTimerFunc(100, TimerFunction, 0);
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // �������� ��blue���� ����
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�.

	for (int i = 0; i < rect.size(); ++i) {
		glColor3f(rect[i].red, rect[i].green, rect[i].blue);
		glRectf(rect[i].x1, rect[i].y1, rect[i].x2, rect[i].y2);
	}

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}

void TimerFunction(int value) {
	if (value == 0) {
		for (int i = 0; i < rect.size(); ++i) {
			if (rect[i].animate) {
				if (rect[i].dir == UP) {
					rect[i].y1 -= 3;
					rect[i].y2 -= 3;
				}
				else if (rect[i].dir == LEFT) {
					rect[i].x1 -= 3;
					rect[i].x2 -= 3;
				}
				else if (rect[i].dir == DOWN) {
					rect[i].y1 += 3;
					rect[i].y2 += 3;
				}
				else if (rect[i].dir == RIGHT) {
					rect[i].x1 += 3;
					rect[i].x2 += 3;
				}
				else if (rect[i].dir == Q_SIDE) {
					rect[i].x1 -= 5;
					rect[i].x2 -= 5;
					rect[i].y1 -= 5;
					rect[i].y2 -= 5;
				}
				else if (rect[i].dir == E_SIDE) {
					rect[i].x1 += 5;
					rect[i].x2 += 5;
					rect[i].y1 -= 5;
					rect[i].y2 -= 5;
				}
				else if (rect[i].dir == Z_SIDE) {
					rect[i].x1 -= 5;
					rect[i].x2 -= 5;
					rect[i].y1 += 5;
					rect[i].y2 += 5;
				}
				else if (rect[i].dir == C_SIDE) {
					rect[i].x1 += 5;
					rect[i].x2 += 5;
					rect[i].y1 += 5;
					rect[i].y2 += 5;
				}

				rect[i].make_smaller();
				if (rect[i].x2 - rect[i].x1 < 10) {
					rect.erase(rect.begin() + i);
				}
			}
		}

		glutTimerFunc(100, TimerFunction, 0);
	}


	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mouse_down = true;
		
		for (int i = 0; i < rect.size(); ++i) {
			if (rect[i].x1 <= x && x <= rect[i].x2 && rect[i].y2 <= y && y <= rect[i].y1) {
				rect[i].selected = true;
			}
		}

		for (int i = 0; i < rect.size(); ++i) {
			if (rect[i].selected) {
				float x_half{ rect[i].x1 + (rect[i].x2 - rect[i].x1) / 2 };
				float y_half{ rect[i].y2 + (rect[i].y1 - rect[i].y2) / 2 };

				Rect temp1(rect[i].x1, y_half, x_half, rect[i].y2);
				Rect temp2(x_half, y_half, rect[i].x2, rect[i].y2);
				Rect temp3(rect[i].x1, rect[i].y1, x_half, y_half);
				Rect temp4(x_half, rect[i].y1, rect[i].x2, y_half);

				temp1.make_same_color(rect[i]);
				temp2.make_same_color(rect[i]);
				temp3.make_same_color(rect[i]);
				temp4.make_same_color(rect[i]);

				// �¿���� �ϳ��� �̵�
				if (check1) {
					temp1.dir = UP;
					temp2.dir = RIGHT;
					temp3.dir = LEFT;
					temp4.dir = DOWN;
				}

				// �밢�� �̵�
				else if (check2) {
					temp1.dir = Q_SIDE;
					temp2.dir = E_SIDE;
					temp3.dir = Z_SIDE;
					temp4.dir = C_SIDE;
				}

				// ���� �������� �̵�
				else if (check3) {
					switch (rand() % 4) {
					case UP:
						temp1.dir = UP;
						temp2.dir = UP;
						temp3.dir = UP;
						temp4.dir = UP;
						break;
					case LEFT:
						temp1.dir = LEFT;
						temp2.dir = LEFT;
						temp3.dir = LEFT;
						temp4.dir = LEFT;
						break;
					case RIGHT:
						temp1.dir = RIGHT;
						temp2.dir = RIGHT;
						temp3.dir = RIGHT;
						temp4.dir = RIGHT;
						break;
					case DOWN:
						temp1.dir = DOWN;
						temp2.dir = DOWN;
						temp3.dir = DOWN;
						temp4.dir = DOWN;
						break;
					}
				}

				// �¿���� & �밢�� �̵�
				else if (check4) {
					Rect temp5(rect[i].x1, y_half, x_half, rect[i].y2);
					Rect temp6(x_half, y_half, rect[i].x2, rect[i].y2);
					Rect temp7(rect[i].x1, rect[i].y1, x_half, y_half);
					Rect temp8(x_half, rect[i].y1, rect[i].x2, y_half);

					temp5.make_same_color(rect[i]);
					temp6.make_same_color(rect[i]);
					temp7.make_same_color(rect[i]);
					temp8.make_same_color(rect[i]);

					temp1.dir = UP;
					temp2.dir = RIGHT;
					temp3.dir = LEFT;
					temp4.dir = DOWN;
					temp5.dir = Q_SIDE;
					temp6.dir = E_SIDE;
					temp7.dir = Z_SIDE;
					temp8.dir = C_SIDE;

					temp5.animate = true;
					temp6.animate = true;
					temp7.animate = true;
					temp8.animate = true;

					rect.push_back(temp5);
					rect.push_back(temp6);
					rect.push_back(temp7);
					rect.push_back(temp8);
				}

				temp1.animate = true;
				temp2.animate = true;
				temp3.animate = true;
				temp4.animate = true;

				rect.push_back(temp1);
				rect.push_back(temp2);
				rect.push_back(temp3);
				rect.push_back(temp4);
				rect.erase(rect.begin() + i);
			}
		}
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		mouse_down = false;
	}

	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	if (key == 'q') {
		glutDestroyWindow(true);
	}

	else if (key == 'r') {
		rect.clear();
		initialize();
	}

	else if (key == '1') {
		if (!check1) {
			check1 = true;
			cout << "�¿���� �̵� ���õ�." << endl;
		}
		else if (check1) {
			check1 = false;
			cout << "�¿���� �̵� ������." << endl;
		}
	}

	else if (key == '2') {
		if (!check2) {
			check2 = true;
			cout << "�밢�� �̵� ���õ�." << endl;
		}
		else if (check2) {
			check2 = false;
			cout << "�밢�� �̵� ������." << endl;
		}
	}

	else if (key == '3') {
		if (!check3) {
			check3 = true;
			cout << "���� ���� �̵� ���õ�." << endl;
		}
		else if (check3) {
			check3 = false;
			cout << "���� ���� �̵� ������." << endl;
		}
	}

	else if (key == '4') {
		if (!check4) {
			check4 = true;
			cout << "�¿���� & �밢�� �̵� ���õ�." << endl;
		}
		else if (check4) {
			check4 = false;
			cout << "�¿���� & �밢�� �̵� ������." << endl;
		}
	}

	glutPostRedisplay();
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
{
	glViewport(0, 0, w, h);
}

void initialize() {
	for (int i = rect.size(); i < 5; ++i) {
		Rect temp_rect{};
		temp_rect.initialize();
		rect.push_back(temp_rect);
	}
}
