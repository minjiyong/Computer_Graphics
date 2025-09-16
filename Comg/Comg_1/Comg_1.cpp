#include <iostream>
#include <random>
#include <stdlib.h>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

GLvoid drawScene(GLvoid);
void TimerFunction(int value);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Reshape(int w, int h);

float red{ 255 };
float green{ 255 };
float blue{ 255 };
bool timer{ false };

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ���� 
{
	srand(static_cast<int>(time(NULL)));

	//--- ������ �����ϱ�
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(100, 100); // �������� ��ġ ����
	glutInitWindowSize(800, 600); // �������� ũ�� ����
	glutCreateWindow("Example1"); // ������ ����(������ �̸�)

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else std::cout << "GLEW Initialized\n";

	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� 
{
	glClearColor(red, green, blue, 1.0f); // �������� ��blue���� ����
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�.

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}

void TimerFunction(int value) {
	if (value == 1) {
		red = (float)(rand() % 256) / 255.0f;
		green = (float)(rand() % 256) / 255.0f;
		blue = (float)(rand() % 256) / 255.0f;
	}

	glutPostRedisplay();
	if (timer) glutTimerFunc(500, TimerFunction, 1);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	if (key == 'c') {
		red = 0;
		green = 255;
		blue = 255;
	}

	else if (key == 'm') {
		red = 255;
		green = 0;
		blue = 255;
	}

	else if (key == 'y') {
		red = 255;
		green = 255;
		blue = 0;
	}

	else if (key == 'a') {
		red = (float)(rand() % 256) / 255.0f;
		green = (float)(rand() % 256) / 255.0f;
		blue = (float)(rand() % 256) / 255.0f;
	}

	else if (key == 'w') {
		red = 255;
		green = 255;
		blue = 255;
	}

	else if (key == 'k') {
		red = 0;
		green = 0;
		blue = 0;
	}

	else if (key == 't') {
		timer = true;
		glutTimerFunc(100, TimerFunction, 1);
	}

	else if (key == 's') {
		timer = false;
	}

	else if (key == 'q') {
		glutDestroyWindow(true);
	}

	glutPostRedisplay();
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
{
	glViewport(0, 0, w, h);
}