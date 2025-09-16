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


class Rect {
public:
	float x1{};
	float y1{};
	float x2{};
	float y2{};

	float red{};
	float green{};
	float blue{};

	bool selected{ false };

	void initialize() {
		x1 = (float)(rand() % 700);
		y1 = (float)(rand() % 700 + 100);
		x2 = x1 + 100;
		y2 = y1 - 100;

		red = (float)(rand() % 256) / 255.0f;
		green = (float)(rand() % 256) / 255.0f;
		blue = (float)(rand() % 256) / 255.0f;
	}
};

vector<Rect> rect;
float now_x{};
float now_y{};

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ���� 
{
	srand(static_cast<int>(time(NULL)));

	//--- ������ �����ϱ�
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(100, 100); // �������� ��ġ ����
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // �������� ũ�� ����
	glutCreateWindow("Example3"); // ������ ����(������ �̸�)

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

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		now_x = x;
		now_y = y;

		for (int i = 0; i < rect.size(); ++i) {
			if (rect[i].x1 <= x && x <= rect[i].x2 && rect[i].y2 <= y && y <= rect[i].y1) {
				rect[i].selected = true;
			}
		}
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		Rect temp_rect{};

		for (int i = 0; i < rect.size(); ++i) {
			if (rect[i].selected) {
				temp_rect = rect[i];
			}
		}

		for (int i = 0; i < rect.size(); ++i) {
			if (!rect[i].selected) {
				if (rect[i].x1 < temp_rect.x2 && rect[i].y2 < temp_rect.y1 && rect[i].x2 > temp_rect.x1 && rect[i].y1 > temp_rect.y2) {
					Rect new_rect{};
					new_rect.x1 = min(rect[i].x1, temp_rect.x1);
					new_rect.y1 = max(rect[i].y1, temp_rect.y1);
					new_rect.x2 = max(rect[i].x2, temp_rect.x2);
					new_rect.y2 = min(rect[i].y2, temp_rect.y2);
					new_rect.red = (float)(rand() % 256) / 255.0f;
					new_rect.green = (float)(rand() % 256) / 255.0f;
					new_rect.blue = (float)(rand() % 256) / 255.0f;

					rect.erase(rect.begin() + i);
					for (int j = 0; j < rect.size(); ++j) {
						if(rect[j].selected) rect.erase(rect.begin() + j);
					}
					rect.push_back(new_rect);
				}
			}
		}

		for (int i = 0; i < rect.size(); ++i) {
			rect[i].selected = false;
		}
	}

	glutPostRedisplay();
}

void Motion(int x, int y) {
	float deltaX{ x - now_x };
	float deltaY{ y - now_y };

	for (int i = 0; i < rect.size(); ++i) {
		if (rect[i].selected) {
			rect[i].x1 += deltaX;
			rect[i].y1 += deltaY;
			rect[i].x2 += deltaX;
			rect[i].y2 += deltaY;
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

	else if (key == 'a') {
		if (rect.size() == 10) {
			glutPostRedisplay();
			return;
		}

		Rect temp_rect{};
		temp_rect.initialize();
		rect.push_back(temp_rect);
	}

	glutPostRedisplay();
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
{
	glViewport(0, 0, w, h);
}
