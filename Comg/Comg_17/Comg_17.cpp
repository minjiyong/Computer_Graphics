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

#define ALL 0
#define CUBE 1
#define PYRAMID 2

#define UPSIDE 1
#define BOTHSIDE 2
#define FRONTSIDE 3
#define BACKSIDE 4


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
void SpecialKeyboard(int key, int x, int y);

float distanceFromOrigin(float x, float y) {
	return std::sqrt(x * x + y * y);  // 또는 std::sqrt(std::pow(x, 2) + std::pow(y, 2));
}
float randomValue() {
	return -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / 2.0f);
}

void initialize();
void initialize_2();
void obj_bound_check();


Importer importer;
ShaderProgram SP;


class Camera {
public:
	glm::vec3 cameraPos = glm::vec3(3.0f, 3.0f, 3.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float fov = 45.0f;
	float aspectRatio = 800.0f / 800.0f;
	float nearClip = 0.1f;
	float farClip = 100.0f;

	void make_camera_perspective() {
		glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
		glm::mat4 projection = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
		glUniformMatrix4fv(glGetUniformLocation(SP.shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SP.shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	}

	void make_camera_ortho() {
		glm::mat4 view = glm::lookAt(glm::vec3(0.1f, 0.1f, 0.1f), cameraTarget, cameraUp);
		glm::mat4 projection = glm::ortho(-1.5f, 1.5f, -1.5f, 1.5f, -1.0f, 1.0f);
		glUniformMatrix4fv(glGetUniformLocation(SP.shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SP.shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	}
};

class Object {
public:
	GLuint objVAO{};
	string objName{};

	// 전체 오브젝트에 대한 정보가 담긴 벡터
	glm::vec3 location{};	// 위치(translate 적용)
	glm::vec3 color{};		// 색상

	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };  // 크기 (디폴트로 1)
	float rotationAngle = 0.0f;  // 회전 각도 (라디안)

	//양 옆면
	glm::vec3 location_left_and_right{};
	//뒷면
	glm::vec3 scale_back{};

	//피라미드 xz축 회전
	float rotationAngle_pyramid = 0.0f;
	bool turn_point{ false };

	void initialize_Object(string name, glm::vec3 pos) {
		for (auto& v : importer.VertexBuffers) {
			if (name == v->filename) {
				objVAO = v->VAO;
				cout << "Object VAO: " << objVAO << endl;
				objName = name;
				location = pos;
				color.r = (float)(rand() % 256) / 255.0f;
				color.g = (float)(rand() % 256) / 255.0f;
				color.b = (float)(rand() % 256) / 255.0f;
				scale = glm::vec3(1.0f, 1.0f, 1.0f);  // 기본 크기 설정
				scale_back = scale; 
				rotationAngle = 0.0f;  // 기본 회전 각도 설정
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
		scale_back = scale;
	}

	void PlusScale() {
		scale.x += 0.002f;
		scale.y += 0.002f;
		scale.z += 0.002f;
	}

	void MinusScale() {
		scale.x -= 0.002f;
		scale.y -= 0.002f;
		scale.z -= 0.002f;
	}

	void PlusScale_back() {
		scale_back.x += 0.002f;
		scale_back.y += 0.002f;
		scale_back.z += 0.002f;
	}

	void MinusScale_back() {
		scale_back.x -= 0.002f;
		scale_back.y -= 0.002f;
		scale_back.z -= 0.002f;
	}

	void setRotation(float angle) {
		rotationAngle = glm::radians(angle);  // 입력은 각도, 내부에서는 라디안 사용
	}

	void setRotation_pyramid(float angle) {
		rotationAngle_pyramid = glm::radians(angle);  // 입력은 각도, 내부에서는 라디안 사용
	}

	void setColor(const glm::vec3& newcolor) {
		color = newcolor;
	}

	void make_Color_random() {
		color.r = (float)(rand() % 256) / 255.0f;
		color.g = (float)(rand() % 256) / 255.0f;
		color.b = (float)(rand() % 256) / 255.0f;
	}

	void make_normal() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));
	}

	void rotate_by_x() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle_pyramid, glm::vec3(1.0f, 0.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));
	}

	void rotate_by_y() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));
	}

	void rotate_by_z() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle_pyramid, glm::vec3(0.0f, 0.0f, 1.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));
	}

	void revolve_by_y() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));
	}

	void rotate_and_revolve_by_y() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));
	}


	void translate_left_and_right() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location_left_and_right);
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));
	}
	void scale_backside() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::scale(transform_Matrix, scale_back);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));
	}


	void Draw_cube() {
		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
	void Draw_cube_left_and_right() {
		glBindVertexArray(objVAO);
		// 오른쪽
		glDrawArrays(GL_TRIANGLES, 6, 3);
		glDrawArrays(GL_TRIANGLES, 24, 3);
		// 왼쪽
		glDrawArrays(GL_TRIANGLES, 12, 3);
		glDrawArrays(GL_TRIANGLES, 30, 3);
		glBindVertexArray(0);
	}
	void Draw_cube_top() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
	void Draw_cube_front() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		glm::vec3 temp_location = { 0.0f, 0.0f, 0.0f };

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
	void Draw_cube_bottom() {
		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 18, 3);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
	}
	void Draw_cube_back() {
		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 15, 3);
		glDrawArrays(GL_TRIANGLES, 33, 3);
		glBindVertexArray(0);
	}

	void Draw_pyramid() {
		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 18);
		glBindVertexArray(0);
	}
	void Draw_pyramid_side() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle_pyramid, glm::vec3(1.0f, 0.0f, 0.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
	}
	void Draw_pyramid_bottom() {
		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 12, 18);
		glBindVertexArray(0);
	}

	void Draw_line() {
		// 축을 그릴 때 변환을 적용하지 않음
		glm::mat4 transform_Matrix = glm::mat4(1.0f);
		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glLineWidth(2.0f);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}
};

Camera camera;

Object xyz_line[3];
Object cube_top;
Object cube_front;
Object pyramid_side[4];

vector<Object> cube;
vector<Object> pyramid;

int what_shape{ CUBE };

int what_timer{ 0 };

int cube_what_side{ 0 };

bool turn_point{ false };
bool check_pyramid_each_side[4]{ false };

bool draw_only_line{ false };
bool draw_enable{ false };


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
	glutCreateWindow("Example17");
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
	for (auto& v : importer.VertexBuffers) {
		cout << "Filename: " << v->filename << ", VAO: " << v->VAO << ", Vertices: " << v->vertex.size() << ", Colors: " << v->color.size() << endl;
	}

	SP.make_vertexShaders(); //--- 버텍스 세이더 만들기
	SP.make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	SP.make_shaderProgram();

	initialize();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);

	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);

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


	xyz_line[0].Draw_line();
	xyz_line[1].Draw_line();
	xyz_line[2].Draw_line();

	if (draw_enable) {
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}
	else if (!draw_enable) glDisable(GL_CULL_FACE);

	if (draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (!draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (what_shape == CUBE) {
		// 큐브 그리기 (변환 적용)
		for (auto& v : cube) {
			v.make_normal();
			v.Draw_cube_bottom();
	
			v.scale_backside();
			v.Draw_cube_back();
	
			v.translate_left_and_right();
			v.Draw_cube_left_and_right();
	
			cube_front.Draw_cube_front();
			cube_top.Draw_cube_top();
		}
	}

	else if (what_shape == PYRAMID) {
		for (auto& v : pyramid) {
			v.make_normal();
			v.Draw_pyramid_bottom();

			for (int i = 0; i < 4; ++i) {
				pyramid_side[i].Draw_pyramid_side();
			}
		}
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
		if (what_timer == 1) {
			if (!turn_point) {
				cube[0].location_left_and_right.y += 0.01f;
				if (cube[0].location_left_and_right.y > 1.0f) turn_point = true;
			}
			else if (turn_point) {
				cube[0].location_left_and_right.y -= 0.01f;
				if (cube[0].location_left_and_right.y < 0.0f) turn_point = false;
			}
		}
		if (what_timer == 1) glutTimerFunc(17, TimerFunction, 1);
	}

	if (value == 2) {
		if (what_timer == 2) {
			cube_top.rotationAngle += 0.01f;
		}
		if (what_timer == 2) glutTimerFunc(17, TimerFunction, 2);
	}

	if (value == 3) {
		if (what_timer == 3) {
			if (!turn_point) {
				cube_front.rotationAngle += 0.01f;
				if (cube_front.rotationAngle > glm::radians(360.0f)) turn_point = true;
			}
			else if (turn_point) {
				cube_front.rotationAngle -= 0.01f;
				if (cube_front.rotationAngle < glm::radians(270.0f)) turn_point = false;
			}
		}
		if (what_timer == 3) glutTimerFunc(17, TimerFunction, 3);
	}

	if (value == 4) {
		if (what_timer == 4) {
			if (!turn_point) {
				cube[0].MinusScale_back();
				if (cube[0].scale_back.x < 0.0f) turn_point = true;
			}
			else if (turn_point) {
				cube[0].PlusScale_back();
				if (cube[0].scale_back.x > 0.5f) turn_point = false;
			}
		}
		if (what_timer == 4) glutTimerFunc(17, TimerFunction, 4);
	}

	if (value == 5) {
		if (what_timer == 5) {
			if (!pyramid_side[0].turn_point) {
				pyramid_side[0].rotationAngle_pyramid -= 0.01f;
				if (pyramid_side[0].rotationAngle_pyramid < glm::radians(-213.0f)) pyramid_side[0].turn_point = true;
			}
			else if (pyramid_side[0].turn_point) {
				pyramid_side[0].rotationAngle_pyramid += 0.01f;
				if (pyramid_side[0].rotationAngle_pyramid > glm::radians(33.0f)) pyramid_side[0].turn_point = false;
			}

			if (!pyramid_side[1].turn_point) {
				pyramid_side[1].rotationAngle_pyramid += 0.01f;
				if (pyramid_side[1].rotationAngle_pyramid > glm::radians(573.0f)) pyramid_side[1].turn_point = true;
			}
			else if (pyramid_side[1].turn_point) {
				pyramid_side[1].rotationAngle_pyramid -= 0.01f;
				if (pyramid_side[1].rotationAngle_pyramid < glm::radians(330.0f)) pyramid_side[1].turn_point = false;
			}

			if (!pyramid_side[2].turn_point) {
				pyramid_side[2].rotationAngle_pyramid += 0.01f;
				if (pyramid_side[2].rotationAngle_pyramid > glm::radians(573.0f)) pyramid_side[2].turn_point = true;
			}
			else if (pyramid_side[2].turn_point) {
				pyramid_side[2].rotationAngle_pyramid -= 0.01f;
				if (pyramid_side[2].rotationAngle_pyramid < glm::radians(330.0f)) pyramid_side[2].turn_point = false;
			}

			if (!pyramid_side[3].turn_point) {
				pyramid_side[3].rotationAngle_pyramid -= 0.01f;
				if (pyramid_side[3].rotationAngle_pyramid < glm::radians(-213.0f)) pyramid_side[3].turn_point = true;
			}
			else if (pyramid_side[3].turn_point) {
				pyramid_side[3].rotationAngle_pyramid += 0.01f;
				if (pyramid_side[3].rotationAngle_pyramid > glm::radians(33.0f)) pyramid_side[3].turn_point = false;
			}
		}
		if (what_timer == 5) glutTimerFunc(17, TimerFunction, 5);
	}

	if (value == 6) {
		if (what_timer == 6) {
			if (check_pyramid_each_side[0] == false) {
				if (!pyramid_side[0].turn_point) {
					pyramid_side[0].rotationAngle_pyramid -= 0.01f;
					if (pyramid_side[0].rotationAngle_pyramid < glm::radians(-90.0f)) pyramid_side[0].turn_point = true;
				}
				else if (pyramid_side[0].turn_point) {
					pyramid_side[0].rotationAngle_pyramid += 0.01f;
					if (pyramid_side[0].rotationAngle_pyramid > glm::radians(33.0f)) {
						pyramid_side[0].turn_point = false;
						check_pyramid_each_side[0] = true;
						check_pyramid_each_side[1] = false;
					}
				}
			}

			else if (check_pyramid_each_side[1] == false) {
				if (!pyramid_side[1].turn_point) {
					pyramid_side[1].rotationAngle_pyramid += 0.01f;
					if (pyramid_side[1].rotationAngle_pyramid > glm::radians(450.0f)) pyramid_side[1].turn_point = true;
				}
				else if (pyramid_side[1].turn_point) {
					pyramid_side[1].rotationAngle_pyramid -= 0.01f;
					if (pyramid_side[1].rotationAngle_pyramid < glm::radians(330.0f)) {
						pyramid_side[1].turn_point = false;
						check_pyramid_each_side[1] = true;
						check_pyramid_each_side[2] = false;
					}
				}
			}

			else if (check_pyramid_each_side[2] == false) {
				if (!pyramid_side[2].turn_point) {
					pyramid_side[2].rotationAngle_pyramid += 0.01f;
					if (pyramid_side[2].rotationAngle_pyramid > glm::radians(450.0f)) pyramid_side[2].turn_point = true;
				}
				else if (pyramid_side[2].turn_point) {
					pyramid_side[2].rotationAngle_pyramid -= 0.01f;
					if (pyramid_side[2].rotationAngle_pyramid < glm::radians(330.0f)) {
						pyramid_side[2].turn_point = false;
						check_pyramid_each_side[2] = true;
						check_pyramid_each_side[3] = false;
					}
				}
			}

			else if (check_pyramid_each_side[3] == false) {
				if (!pyramid_side[3].turn_point) {
					pyramid_side[3].rotationAngle_pyramid -= 0.01f;
					if (pyramid_side[3].rotationAngle_pyramid < glm::radians(-90.0f)) pyramid_side[3].turn_point = true;
				}
				else if (pyramid_side[3].turn_point) {
					pyramid_side[3].rotationAngle_pyramid += 0.01f;
					if (pyramid_side[3].rotationAngle_pyramid > glm::radians(33.0f)) {
						pyramid_side[3].turn_point = false;
						check_pyramid_each_side[3] = true;
						check_pyramid_each_side[0] = false;
					}
				}
			}
		}
		if (what_timer == 6) glutTimerFunc(17, TimerFunction, 6);
	}

	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		auto mouse = ConvertWinToGL(x, y);
		now_x = mouse.first;
		now_y = mouse.second;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		auto mouse = ConvertWinToGL(x, y);

		glutPostRedisplay();
	}
}

void Motion(int x, int y) {
	auto mouse = ConvertWinToGL(x, y);
	float deltaX{ mouse.first - now_x };
	float deltaY{ mouse.second - now_y };

	now_x = mouse.first;
	now_y = mouse.second;

	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	if (key == 'q') {
		glutDestroyWindow(true);
	}

	else if (key == 'a') {
		for (auto& k : importer.VertexBuffers) {
			for (auto& v : k->vertex)
				cout << "vertex x: " << v.x << ", y: " << v.y << ", z: " << v.z << endl;
			for (auto& v : k->face)
				cout << "face x: " << v.x << ", y: " << v.y << ", z: " << v.z << endl;
			for (auto& v : k->color)
				cout << "color r: " << v.x << ", g: " << v.y << ", b: " << v.z << endl;
		}
	}

	else if (key == 's') {
		what_shape = CUBE;
		cube_what_side = BOTHSIDE;
		turn_point = false;
		what_timer = 1;
		glutTimerFunc(17, TimerFunction, 1);
	}
	else if (key == 't') {
		what_shape = CUBE;
		cube_what_side = UPSIDE;
		turn_point = false;
		what_timer = 2;
		glutTimerFunc(17, TimerFunction, 2);
	}
	else if (key == 'f') {
		what_shape = CUBE;
		cube_what_side = FRONTSIDE;
		turn_point = false;
		what_timer = 3;
		glutTimerFunc(17, TimerFunction, 3);
	}
	else if (key == 'b') {
		what_shape = CUBE;
		cube_what_side = BACKSIDE;
		turn_point = false;
		what_timer = 4;
		glutTimerFunc(17, TimerFunction, 4);
	}


	else if (key == 'o') {
		what_shape = PYRAMID;
		for (int i = 0; i < 4; ++i) pyramid_side[i].turn_point = false;
		what_timer = 5;
		glutTimerFunc(17, TimerFunction, 5);
	}
	else if (key == 'r') {
		what_shape = PYRAMID;
		for (int i = 0; i < 4; ++i)  check_pyramid_each_side[i] = true;
		check_pyramid_each_side[0] = false;
		what_timer = 6;
		glutTimerFunc(17, TimerFunction, 6);
	}

	else if (key == 'p') {
		camera.make_camera_ortho();
	}
	else if (key == 'P') {
		camera.make_camera_perspective();
	}


	else if (key == 'h') {
		draw_enable = true;
	}
	else if (key == 'H') {
		draw_enable = false;
	}

	else if (key == 'w') {
		draw_only_line = true;
	}
	else if (key == 'W') {
		draw_only_line = false;
	}

	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y) {
	if (key == GLUT_KEY_F1) {
		what_shape = ALL;
	}
	else if (key == GLUT_KEY_F2) {
		what_shape = CUBE;
		cout << "큐브" << what_shape << endl;
	}
	else if (key == GLUT_KEY_F3) {
		what_shape = PYRAMID;
		cout << "피라미드" << what_shape << endl;
	}
	else if (key == GLUT_KEY_F4) {
		initialize();

		what_shape = CUBE;

		draw_enable = false;
		draw_only_line = false;

		what_timer = 0;
	}

	glutPostRedisplay();
}

void initialize() {
	camera.make_camera_perspective();

	xyz_line[0].initialize_Object("x_Line", { 0.0f, 0.0f, 0.0f });
	xyz_line[1].initialize_Object("y_Line", { 0.0f, 0.0f, 0.0f });
	xyz_line[2].initialize_Object("z_Line", { 0.0f, 0.0f, 0.0f });

	cube.clear();
	pyramid.clear();

	Object temp_obj;
	temp_obj.initialize_Object("cube.obj", { 0.0f, 0.0f, 0.0f });
	temp_obj.setScale({ 0.5f, 0.5f, 0.5f });
	cube.push_back(temp_obj);

	cube_top.initialize_Object("cube_top", { 0.0f, 0.5f, 0.0f });
	cube_top.setScale({ 0.5f, 0.5f, 0.5f });

	cube_front.initialize_Object("cube_front", { 0.43f, 0.0f, 0.85f });
	cube_front.setScale({ 0.43f, 0.43f, 0.43f });
	cube_front.setRotation(270);

	temp_obj.initialize_Object("pyramid.obj", { 0.0f, 0.0f, 0.0f });
	pyramid.push_back(temp_obj);

	for (int i = 0; i < 4; ++i) {
		pyramid_side[i].initialize_Object("pyramid_side", { 0.0f, 0.0f, 0.0f });
	}
	pyramid_side[0].location = { -0.4000f, 0.0000f, 0.0000f };
	pyramid_side[0].setRotation(90);
	pyramid_side[0].setRotation_pyramid(30);
	pyramid_side[1].location = { 0.4000f, 0.0000f, 0.0000f };
	pyramid_side[1].setRotation(90);
	pyramid_side[1].setRotation_pyramid(330);
	pyramid_side[2].location = { 0.0000f, 0.0000f, 0.4000f };
	pyramid_side[2].setRotation_pyramid(330);
	pyramid_side[3].location = { 0.0000f, 0.0000f, -0.4000f };
	pyramid_side[3].setRotation_pyramid(30);
}