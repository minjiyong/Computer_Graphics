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

#define UP 1
#define DOWN 2

#define LEFT 3
#define RIGHT 4

GLvoid drawScene(GLvoid);
void TimerFunction(int value);
void Mouse(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Reshape(int w, int h);

void rect_bound_check();

class Rect {
public:
	float x1{};
	float y1{};
	float x2{};
	float y2{};

	float red{};
	float green{};
	float blue{};

	int dir_ud{};
	int dir_lr{};

	bool go_bigger{ true };

	void initialize(float x, float y) {
		x1 = x - 50;
		y1 = y + 50;
		x2 = x + 50;
		y2 = y - 50;

		red = (float)(rand() % 256) / 255.0f;
		green = (float)(rand() % 256) / 255.0f;
		blue = (float)(rand() % 256) / 255.0f;

		dir_ud = rand() % 2 + 1;	// UP or DOWN
		dir_lr = rand() % 2 + 3;	// LEFT or RIGHT

		go_bigger = true;
	}
};

vector<Rect> rect;
vector<Rect> origin_rect;

bool timer1{ false };
bool timer2{ false };
bool timer3{ false };
bool timer4{ false };

bool check1{ false };
bool check2{ false };
bool check3{ false };
bool check4{ false };


void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{
	srand(static_cast<int>(time(NULL)));

	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("Example4"); // 윈도우 생성(윈도우 이름)

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
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 ‘blue’로 지정
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.

	for (int i = 0; i < rect.size(); ++i) {
		glColor3f(rect[i].red, rect[i].green, rect[i].blue);
		glRectf(rect[i].x1, rect[i].y1, rect[i].x2, rect[i].y2);
	}

	glutSwapBuffers(); // 화면에 출력하기
}

void TimerFunction(int value) {
	if (value == 1) {
		for (int i = 0; i < rect.size(); ++i) {
			if (rect[i].dir_ud == UP && rect[i].dir_lr == LEFT) {
				--rect[i].x1;
				--rect[i].y1;
				--rect[i].x2;
				--rect[i].y2;
			}
			else if (rect[i].dir_ud == UP && rect[i].dir_lr == RIGHT) {
				++rect[i].x1;
				--rect[i].y1;
				++rect[i].x2;
				--rect[i].y2;
			}
			else if (rect[i].dir_ud == DOWN && rect[i].dir_lr == LEFT) {
				--rect[i].x1;
				++rect[i].y1;
				--rect[i].x2;
				++rect[i].y2;
			}
			else if (rect[i].dir_ud == DOWN && rect[i].dir_lr == RIGHT) {
				++rect[i].x1;
				++rect[i].y1;
				++rect[i].x2;
				++rect[i].y2;
			}
		}
		rect_bound_check();
		if(timer1) glutTimerFunc(17, TimerFunction, 1);
	}

	else if (value == 2) {
		for (int i = 0; i < rect.size(); ++i) {
			if (rect[i].dir_lr == LEFT && rect[i].x1 > 0) {
				--rect[i].x1;
				--rect[i].x2;
			}
			else if (rect[i].dir_lr == RIGHT && rect[i].x2 < WINDOW_WIDTH) {
				++rect[i].x1;
				++rect[i].x2;
			}

			if (rect[i].x1 <= 0 || rect[i].x2 >= WINDOW_WIDTH) {
				if (rect[i].dir_ud == UP) {
					rect[i].y1 -= 50;
					rect[i].y2 -= 50;
				}
				else if (rect[i].dir_ud == DOWN) {
					rect[i].y1 += 50;
					rect[i].y2 += 50;
				}
			}
		}
		rect_bound_check();
		if (timer2) glutTimerFunc(17, TimerFunction, 2);
	}

	else if (value == 3) {
		for (int i = 0; i < rect.size(); ++i) {
			if (rect[i].go_bigger) {
				if (rect[i].y1 - rect[i].y2 < 300) {
					++rect[i].y1;
					--rect[i].y2;
				}
				else rect[i].go_bigger = false;
			}
			else if (!rect[i].go_bigger) {
				if (rect[i].y1 - rect[i].y2 > 0) {
					--rect[i].y1;
					++rect[i].y2;
				}
				else rect[i].go_bigger = true;
			}
		}
		if (timer3) glutTimerFunc(17, TimerFunction, 3);
	}

	else if (value == 4) {
		for (int i = 0; i < rect.size(); ++i) {
			rect[i].red = (float)(rand() % 256) / 255.0f;
			rect[i].green = (float)(rand() % 256) / 255.0f;
			rect[i].blue = (float)(rand() % 256) / 255.0f;
		}
		if (timer4) glutTimerFunc(100, TimerFunction, 4);
	}

	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (rect.size() == 5) {
			glutPostRedisplay();
			return;
		}

		Rect temp_rect{};
		temp_rect.initialize(x, y);
		rect.push_back(temp_rect);
		origin_rect.push_back(temp_rect);
	}

	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	if (key == 'q') {
		glutDestroyWindow(true);
		exit(true);
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
			glutTimerFunc(100, TimerFunction, 4);
			check4 = true;
		}
		else if (check4) {
			timer4 = false;
			check4 = false;
		}
	}

	else if (key == 's') {
		timer1 = false;
		timer2 = false;
		timer3 = false;
		timer4 = false;
	}

	else if (key == 'm') {
		for (int i = 0; i < rect.size(); ++i) {
			rect[i] = origin_rect[i];
		}
	}

	else if (key == 'r') {
		rect.clear();
		origin_rect.clear();
	}

	glutPostRedisplay();
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	glViewport(0, 0, w, h);
}

void rect_bound_check()
{
	for (int i = 0; i < rect.size(); ++i) {
		if (rect[i].y2 <= 0) rect[i].dir_ud = DOWN;
		else if (rect[i].y1 >= WINDOW_HEIGHT) rect[i].dir_ud = UP;
		else if (rect[i].x1 <= 0) rect[i].dir_lr = RIGHT;
		else if (rect[i].x2 >= WINDOW_WIDTH) rect[i].dir_lr = LEFT;
	}
}
