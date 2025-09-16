#include <iostream>
#include <random>
#include <stdlib.h>
#include <gl/glew.h> //--- 필요한 헤더파일 include
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

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{
	srand(static_cast<int>(time(NULL)));

	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(800, 600); // 윈도우의 크기 지정
	glutCreateWindow("Example1"); // 윈도우 생성(윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else std::cout << "GLEW Initialized\n";

	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(red, green, blue, 1.0f); // 바탕색을 ‘blue’로 지정
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.

	glutSwapBuffers(); // 화면에 출력하기
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

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	glViewport(0, 0, w, h);
}