#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
//--- 메인 함수
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glew.h> //--- 필요한 헤더파일 include
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


//--- 함수 선언 추가하기
std::pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WINDOW_WIDTH / 2)) / (WINDOW_WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (WINDOW_HEIGHT / 2)) / (WINDOW_HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

void TimerFunction(int value);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);

void tri_bound_check();
float distanceFromOrigin(float x, float y) {
	return std::sqrt(x * x + y * y);  // 또는 std::sqrt(std::pow(x, 2) + std::pow(y, 2));
}
float randomValue() {
	return -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / 2.0f);
}

void initialize();
void obj_bound_check();


Importer importer;
ShaderProgram SP;


class Rect {
public:
	float x1{};
	float y1{};
	float x2{};
	float y2{};

	void initialize(float x, float y) {
		x1 = x - 0.05f;
		y1 = y + 0.05f;
		x2 = x + 0.05f;
		y2 = y - 0.05f;
	}
};

class Object {
public:
	static bool del;
	GLuint objVAO{};
	string objName{};

	int point{};
	Rect boundary{};
	bool selected{ false };
	bool animation{ false };
	int what_ani{ 0 };

	int dir_ud{ STOP };
	int dir_lr{ STOP };

	glm::vec3 location{};	// 위치(translate 적용)
	glm::vec3 color{};		// 색상

	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };  // 크기 (디폴트로 1)
	float rotationAngle = 0.0f;  // 회전 각도 (라디안)

	void initialize_Object(string name, glm::vec3 pos) {
		for (auto& v : importer.VertexBuffers) {
			if (name == v->filename) {
				objVAO = v->VAO;
				objName = name;
				location = pos;
				color.r = (float)(rand() % 256) / 255.0f;
				color.g = (float)(rand() % 256) / 255.0f;
				color.b = (float)(rand() % 256) / 255.0f;
				scale = glm::vec3(1.0f, 1.0f, 1.0f);  // 기본 크기 설정
				rotationAngle = 0.0f;  // 기본 회전 각도 설정

				if (name == "Point") point = 1;
				else if (name == "Line") point = 2;
				else if (name == "Triangle") point = 3;
				else if (name == "Rectangle") point = 4;
				else if (name == "Pentagon") point = 5;
				else if (name == "Hexagon") point = 6;

				make_boundary();
			}
		}
	}

	void setVAO() {
		for (auto& v : importer.VertexBuffers) {
			if (objName == v->filename) {
				objVAO = v->VAO;
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
		rotationAngle = glm::radians(angle);  // 입력은 각도, 내부에서는 라디안 사용
	}

	void make_Color_random() {
		color.r = (float)(rand() % 256) / 255.0f;
		color.g = (float)(rand() % 256) / 255.0f;
		color.b = (float)(rand() % 256) / 255.0f;
	}

	void make_boundary() {
		boundary.initialize(location.x, location.y);
	}

	static bool del_what() {
		return del;
	}
	static void del_true() {
		del = true;
	}
	static void del_false() {
		del = false;
	}

	void Draw_point() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 기준 회전
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glPointSize(10.0);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);
	}

	void Draw_line() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 기준 회전
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glLineWidth(3.0f);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}

	void Draw_triangle() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 기준 회전
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glPointSize(3.0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
		glBindVertexArray(0);
	}

	void Draw_rectangle() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 기준 회전
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glPointSize(3.0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	void Draw_pentagon() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 기준 회전
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glPointSize(3.0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
		glBindVertexArray(0);
	}

	void Draw_hexagon() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 기준 회전
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glPointSize(3.0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
		glBindVertexArray(0);
	}
};

bool Object::del = false;

vector<Object> obj;

float now_x{};
float now_y{};

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	srand(static_cast<int>(time(NULL)));

	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Example12");
	cout << "윈도우 생성됨" << endl;

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized" << std::endl;

	importer.ReadObj();

	SP.make_vertexShaders(); //--- 버텍스 세이더 만들기
	SP.make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	SP.make_shaderProgram();

	initialize();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);

	glutTimerFunc(17, TimerFunction, 1);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);

	glutMainLoop();
}

//--- 그리기 콜백 함수
GLvoid drawScene()
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//--- 렌더링 파이프라인에 세이더 불러오기

	glUseProgram(SP.shaderID);

	
	for (auto& v : obj) {
		if(v.point == 1) v.Draw_point();
		else if(v.point == 2) v.Draw_line();
		else if(v.point == 3) v.Draw_triangle();
		else if(v.point == 4) v.Draw_rectangle();
		else if(v.point == 5) v.Draw_pentagon();
		else if(v.point == 6) v.Draw_hexagon();
	}
	

	glutSwapBuffers(); //--- 화면에 출력하기
}
//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}


void TimerFunction(int value) {
	if (value == 1) {
		for (auto& t : obj) {
			if (t.animation) {
				//튕기기
				if (t.what_ani == 1) {
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
				//지그재그
				else if (t.what_ani == 2) {
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
			}
		}
		obj_bound_check();
		glutTimerFunc(17, TimerFunction, 1);
	}

	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		auto mouse = ConvertWinToGL(x, y);
		now_x = mouse.first;
		now_y = mouse.second;

		for (auto& v : obj) {
			if (v.boundary.x1 <= mouse.first && mouse.first <= v.boundary.x2 && v.boundary.y2 <= mouse.second && mouse.second <= v.boundary.y1) {
				v.selected = true;
			}
		}
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		auto mouse = ConvertWinToGL(x, y);
		Object temp_obj{};

		for (auto& v : obj) {
			if (v.selected) {
				temp_obj = v;
			}
		}

		for (int i = obj.size() - 1; i >= 0; --i) {
			if (!obj[i].selected) {
				if (obj[i].boundary.x1 <= mouse.first && mouse.first <= obj[i].boundary.x2 && obj[i].boundary.y2 <= mouse.second && mouse.second <= obj[i].boundary.y1) {
					Object new_obj;
					new_obj.location = { mouse.first, mouse.second, 0.0f };
					new_obj.make_Color_random();
					new_obj.make_boundary();
					new_obj.point = (temp_obj.point + obj[i].point) % 6;
					if (new_obj.point == 0) new_obj.point = 6;

					if (new_obj.point == 1) new_obj.objName = "Point";
					else if (new_obj.point == 2) new_obj.objName = "Line";
					else if (new_obj.point == 3) new_obj.objName = "Triangle";
					else if (new_obj.point == 4) new_obj.objName = "Rectangle";
					else if (new_obj.point == 5) new_obj.objName = "Pentagon";
					else if (new_obj.point == 6) new_obj.objName = "Hexagon";
					new_obj.setVAO();

					new_obj.dir_ud = rand() % 2 + 1;	// UP or DOWN
					new_obj.dir_lr = rand() % 2 + 3;	// LEFT or RIGHT
					new_obj.animation = true;
					new_obj.what_ani = rand() % 2 + 1;

					obj.erase(obj.begin() + i);
					obj.push_back(new_obj);
					cout << obj.back().objName << endl;
					Object::del_true();
				}
			}
		}

		if (Object::del_what()) {
			for (int i = obj.size() - 1; i >= 0; --i) {
				if (obj[i].selected) obj.erase(obj.begin() + i);
			}
		}

		//for (int i = 0; i < obj.size(); ++i) {
		//	if (!obj[i].selected) {
		//		if (obj[i].boundary.x1 < temp_obj.boundary.x2 && obj[i].boundary.y1 < temp_obj.boundary.y2 && obj[i].boundary.x2 > temp_obj.boundary.x1 && obj[i].boundary.y2 > temp_obj.boundary.y1) {
		//			cout << "충돌" << endl;
		//			obj.erase(obj.begin() + i);
		//			/*for (int j = 0; j < rect.size(); ++j) {
		//				if (rect[j].selected) rect.erase(rect.begin() + j);
		//			}*/
		//		}
		//	}
		//}

		for (auto& v : obj) {
			v.selected = false;
			Object::del_false();
		}
	}

	glutPostRedisplay();
}

void Motion(int x, int y) {
	auto mouse = ConvertWinToGL(x, y);
	float deltaX{ mouse.first - now_x };
	float deltaY{ mouse.second - now_y };

	for (auto& v : obj) {
		if (v.selected) {
			v.location.x += deltaX;
			v.location.y += deltaY;

			v.make_boundary();
		}
	}

	now_x = mouse.first;
	now_y = mouse.second;

	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	if (key == 'q') {
		glutDestroyWindow(true);
	}

	else if (key == 'r') {
		obj.clear();
		initialize();
	}

	glutPostRedisplay();
}

void initialize() {
	Object::del_false();

	Object temp_object;
	float tempx;
	float tempy;

	for (int i = 0; i < 3; ++i) {
		tempx = randomValue();
		tempy = randomValue();
		temp_object.initialize_Object("Point", { tempx, tempy, 0.0f });
		obj.push_back(temp_object);

		tempx = randomValue();
		tempy = randomValue();
		temp_object.initialize_Object("Line", { tempx, tempy, 0.0f });
		obj.push_back(temp_object);

		tempx = randomValue();
		tempy = randomValue();
		temp_object.initialize_Object("Triangle", { tempx, tempy, 0.0f });
		obj.push_back(temp_object);

		tempx = randomValue();
		tempy = randomValue();
		temp_object.initialize_Object("Rectangle", { tempx, tempy, 0.0f });
		obj.push_back(temp_object);

		tempx = randomValue();
		tempy = randomValue();
		temp_object.initialize_Object("Pentagon", { tempx, tempy, 0.0f });
		obj.push_back(temp_object);

		tempx = randomValue();
		tempy = randomValue();
		temp_object.initialize_Object("Hexagon", { tempx, tempy, 0.0f });
		obj.push_back(temp_object);
	}
}

void obj_bound_check()
{
	for (auto& t : obj) {
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