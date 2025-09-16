#include <iostream>
#include <random>
#include <stdlib.h>
#include <vector>
#include <gl/glew.h> //--- 필요한 헤더파일 include
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


void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{
	srand(static_cast<int>(time(NULL)));
	initialize();

	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("Example5"); // 윈도우 생성(윈도우 이름)

	gluOrtho2D(0.0, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0);

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else std::cout << "GLEW Initialized\n";

	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 ‘blue’로 지정
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.

	for (int i = 0; i < rect.size(); ++i) {
		glColor3f(rect[i].red, rect[i].green, rect[i].blue);
		glRectf(rect[i].x1, rect[i].y1, rect[i].x2, rect[i].y2);
	}
	if (mouse_down) {
		glColor3f(eraser.red, eraser.green, eraser.blue);
		glRectf(eraser.x1, eraser.y1, eraser.x2, eraser.y2);
	}

	glutSwapBuffers(); // 화면에 출력하기
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mouse_down = true;
		now_x = x;
		now_y = y;

		// --------- 검은색과 원래 크기로 되돌아오기
		eraser.initialize_eraser();
		// --------- 지우면 색과 크기가 마우스를 떼도 유지됨.

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

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
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
