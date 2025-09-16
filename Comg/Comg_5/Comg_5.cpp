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

GLvoid drawScene(GLvoid);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
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

	float length{};

	void initialize() {
		x1 = (float)(rand() % 700);
		y1 = (float)(rand() % 700 + 100);
		x2 = x1 + 50;
		y2 = y1 - 50;

		red = (float)(rand() % 256) / 255.0f;
		green = (float)(rand() % 256) / 255.0f;
		blue = (float)(rand() % 256) / 255.0f;
	}

	void initialize_eraser() {
		length = 50;
		
		red = 0.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	void move_point(float x, float y) {
		x1 = x - length;
		y1 = y + length;
		x2 = x + length;
		y2 = y - length;
	}

	void make_bigger() {
		length += 10;
	}
};

vector<Rect> rect;
Rect eraser{};

float now_x{};
float now_y{};

bool mouse_down{ false };


void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ���� 
{
	srand(static_cast<int>(time(NULL)));
	initialize();

	//--- ������ �����ϱ�
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(100, 100); // �������� ��ġ ����
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // �������� ũ�� ����
	glutCreateWindow("Example5"); // ������ ����(������ �̸�)

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
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
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
	if (mouse_down) {
		glColor3f(eraser.red, eraser.green, eraser.blue);
		glRectf(eraser.x1, eraser.y1, eraser.x2, eraser.y2);
	}

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mouse_down = true;
		now_x = x;
		now_y = y;

		// --------- �������� ���� ũ��� �ǵ��ƿ���
		eraser.initialize_eraser();
		// --------- ����� ���� ũ�Ⱑ ���콺�� ���� ������.

		eraser.move_point(x, y);
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		mouse_down = false;
	}

	glutPostRedisplay();
}

void Motion(int x, int y) {
	float deltaX{ x - now_x };
	float deltaY{ y - now_y };

	eraser.x1 += deltaX;
	eraser.y1 += deltaY;
	eraser.x2 += deltaX;
	eraser.y2 += deltaY;

	for (int i = 0; i < rect.size(); ++i) {
		if (rect[i].x1 < eraser.x2 && rect[i].y2 < eraser.y1 && rect[i].x2 > eraser.x1 && rect[i].y1 > eraser.y2) {
			eraser.make_bigger();
			eraser.move_point(x, y);
			eraser.red = rect[i].red;
			eraser.green = rect[i].green;
			eraser.blue = rect[i].blue;

			rect.erase(rect.begin() + i);
		}
	}

	now_x = x;
	now_y = y;

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

	glutPostRedisplay();
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
{
	glViewport(0, 0, w, h);
}

void initialize() {
	for (int i = rect.size(); i < 20; ++i) {
		Rect temp_rect{};
		temp_rect.initialize();
		rect.push_back(temp_rect);
	}

	eraser.initialize_eraser();
}
