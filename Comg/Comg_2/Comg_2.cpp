#include <iostream>
#include <random>
#include <stdlib.h>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

GLvoid drawScene(GLvoid);
void Mouse(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Reshape(int w, int h);

void initiailize();

class Rect {
public:
	float x1{};
	float y1{};
	float x2{};
	float y2{};

	float red{};
	float green{};
	float blue{};
};

Rect rect[4]{};

float screen_red{ 255 };
float screen_green{ 255 };
float screen_blue{ 255 };

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{
	srand(static_cast<int>(time(NULL)));
	initiailize();

	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("Example2"); // 윈도우 생성(윈도우 이름)

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
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(screen_red, screen_green, screen_blue, 1.0f); // 바탕색을 ‘blue’로 지정
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.

	for (int i = 0; i < 4; ++i) {
		glColor3f(rect[i].red, rect[i].green, rect[i].blue);
		glRectf(rect[i].x1, rect[i].y1, rect[i].x2, rect[i].y2);
	}

	glutSwapBuffers(); // 화면에 출력하기
}

void Mouse(int button, int state, int x, int y)
{
	bool rect_click{ false };

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		std::cout << x << ", " << y << std::endl;
		for (int i = 0; i < 4; ++i) {
			if (rect[i].x1 <= x && x <= rect[i].x2 && rect[i].y2 <= y && y <= rect[i].y1) {
				rect[i].red = (float)(rand() % 256) / 255.0f;
				rect[i].green = (float)(rand() % 256) / 255.0f;
				rect[i].blue = (float)(rand() % 256) / 255.0f;

				rect_click = true;
			}
		}

		if (!rect_click) {
			screen_red = (float)(rand() % 256) / 255.0f;
			screen_green = (float)(rand() % 256) / 255.0f;
			screen_blue = (float)(rand() % 256) / 255.0f;
		}
	}

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		std::cout << x << ", " << y << std::endl;
		for (int i = 0; i < 4; ++i) {
			if (rect[i].x1 <= x && x <= rect[i].x2 && rect[i].y2 <= y && y <= rect[i].y1) {
				if (rect[i].x2 - rect[i].x1 < 100) break;
				rect[i].x1 += 10.0;
				rect[i].y1 -= 10.0;
				rect[i].x2 -= 10.0;
				rect[i].y2 += 10.0;

				rect_click = true;
			}
		}

		if (!rect_click) {
			// 좌상단
			if (0 < x && x < 400 && 0 < y && y < 400) {
				rect[0].x1 -= 10.0;
				rect[0].y1 += 10.0;
				rect[0].x2 += 10.0;
				rect[0].y2 -= 10.0;
			}
			// 우상단
			else if (400 < x && x < 800 && 0 < y && y < 400) {
				rect[1].x1 -= 10.0;
				rect[1].y1 += 10.0;
				rect[1].x2 += 10.0;
				rect[1].y2 -= 10.0;
			}
			// 좌하단
			else if (0 < x && x < 400 && 400 < y && y < 800) {
				rect[2].x1 -= 10.0;
				rect[2].y1 += 10.0;
				rect[2].x2 += 10.0;
				rect[2].y2 -= 10.0;
			}
			// 우하단
			else if (400 < x && x < 800 && 400 < y && y < 800) {
				rect[3].x1 -= 10.0;
				rect[3].y1 += 10.0;
				rect[3].x2 += 10.0;
				rect[3].y2 -= 10.0;
			}
		}
	}

	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	if (key == 'q') {
		glutDestroyWindow(true);
	}

	glutPostRedisplay();
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	glViewport(0, 0, w, h);
}

void initiailize()
{
	rect[0].x1 = 0.0f;
	rect[0].y1 = 400.0f;
	rect[0].x2 = 400.0f;
	rect[0].y2 = 0.0f;
	rect[0].red = 255.0f;
	rect[0].green = 255.0f;
	rect[0].blue = 0.0f;

	rect[1].x1 = 400.0f;
	rect[1].y1 = 400.0f;
	rect[1].x2 = 800.0f;
	rect[1].y2 = 0.0f;
	rect[1].red = 0.0f;
	rect[1].green = 255.0f;
	rect[1].blue = 0.0f;

	rect[2].x1 = 0.0f;
	rect[2].y1 = 800.0f;
	rect[2].x2 = 400.0f;
	rect[2].y2 = 400.0f;
	rect[2].red = 255.0f;
	rect[2].green = 0.0f;
	rect[2].blue = 0.0f;

	rect[3].x1 = 400.0f;
	rect[3].y1 = 800.0f;
	rect[3].x2 = 800.0f;
	rect[3].y2 = 400.0f;
	rect[3].red = 0.0f;
	rect[3].green = 255.0f;
	rect[3].blue = 255.0f;
}
