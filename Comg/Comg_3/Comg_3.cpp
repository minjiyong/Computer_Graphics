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

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{
	srand(static_cast<int>(time(NULL)));

	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("Example3"); // 윈도우 생성(윈도우 이름)

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

	glutSwapBuffers(); // 화면에 출력하기
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

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	glViewport(0, 0, w, h);
}
