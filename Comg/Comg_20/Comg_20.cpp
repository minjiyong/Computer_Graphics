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

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 800

#define MAIN 0 
#define CHILD 1


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
	glm::vec3 cameraPos = glm::vec3(15.0f, 10.0f, 15.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float rotationAngle = 0.0f;

	float fov = 45.0f;
	float aspectRatio = 800.0f / 800.0f;
	float nearClip = 0.1f;
	float farClip = 100.0f;

	void initialize() {
		cameraPos = glm::vec3(15.0f, 10.0f, 15.0f);
		cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		rotationAngle = 0.0f;

		fov = 45.0f;
		aspectRatio = 800.0f / 800.0f;
		nearClip = 0.1f;
		farClip = 100.0f;
	}

	void setRotation(float angle) {
		rotationAngle = glm::radians(angle);  // 입력은 각도, 내부에서는 라디안 사용
	}

	void make_camera_perspective() {
		// 카메라의 위치를 회전시키기 위해, 회전 행렬을 적용
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 rotatedCameraPos = rotationMatrix * glm::vec4(cameraPos, 1.0f); // 회전된 카메라 위치

		// 회전된 카메라 위치와 목표 지점을 사용하여 LookAt 행렬 계산
		glm::mat4 view = glm::lookAt(glm::vec3(rotatedCameraPos), cameraTarget, cameraUp);
		glUniformMatrix4fv(glGetUniformLocation(SP.shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 projection = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
		glUniformMatrix4fv(glGetUniformLocation(SP.shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	}

	void make_camera_orbit_xz() {
		glm::vec3 cameraPos = glm::vec3(0.0f, 10.0f, 0.0f);
		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, -1.0f);

		float left = -10.0f;
		float right = 10.0f;
		float bottom = -10.0f;
		float top = 10.0f;
		float nearClip = 0.1f;
		float farClip = 100.0f;

		// 카메라의 위치를 회전시키기 위해, 회전 행렬을 적용
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 rotatedCameraPos = rotationMatrix * glm::vec4(cameraPos, 1.0f); // 회전된 카메라 위치

		// 회전된 카메라 위치와 목표 지점을 사용하여 LookAt 행렬 계산
		glm::mat4 view = glm::lookAt(glm::vec3(rotatedCameraPos), cameraTarget, cameraUp);
		glUniformMatrix4fv(glGetUniformLocation(SP.shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 projection = glm::ortho(left, right, bottom, top, nearClip, farClip);
		glUniformMatrix4fv(glGetUniformLocation(SP.shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	}

	void make_camera_orbit_xy() {
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		float left = -10.0f;
		float right = 10.0f;
		float bottom = -10.0f;
		float top = 10.0f;
		float nearClip = 0.1f;
		float farClip = 100.0f;

		// 카메라의 위치를 회전시키기 위해, 회전 행렬을 적용
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 rotatedCameraPos = rotationMatrix * glm::vec4(cameraPos, 1.0f); // 회전된 카메라 위치

		// 회전된 카메라 위치와 목표 지점을 사용하여 LookAt 행렬 계산
		glm::mat4 view = glm::lookAt(glm::vec3(rotatedCameraPos), cameraTarget, cameraUp);
		glUniformMatrix4fv(glGetUniformLocation(SP.shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 projection = glm::ortho(left, right, bottom, top, nearClip, farClip);
		glUniformMatrix4fv(glGetUniformLocation(SP.shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	}
};

class Object {
public:
	int what{};

	GLuint objVAO{};
	string objName{};

	glm::vec3 location{};	// 위치(translate 적용)
	glm::vec3 color{};		// 색상

	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };  // 크기 (디폴트로 1)
	float rotationAngle = 0.0f;  // 회전 각도 (라디안)
	float first_rotationAngle = 0.0f;  // 회전 각도 초기값 (라디안)

	glm::vec3 parent_location{};
	float parent_rotationAngle = 0.0f;
	glm::vec3 parent_scale{ 1.0f, 1.0f, 1.0f };

	glm::vec3 new_location{};

	std::vector<glm::vec3> orbit_vertices;

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
				scale = glm::vec3(0.5f, 0.5f, 0.5f);  // 기본 크기 설정
				rotationAngle = 0.0f;  // 기본 회전 각도 설정

				parent_location = glm::vec3(0.0f, 0.0f, 0.0f);	//부모없으면 원점
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

	void move_UP() {
		location.y += 0.11f;
	}
	void move_LEFT() {
		location.x -= 0.11f;
	}
	void move_DOWN() {
		location.y -= 0.11f;
	}
	void move_RIGHT() {
		location.x += 0.11f;
	}

	void setScale(const glm::vec3& newScale) {
		scale = newScale;
	}

	void PlusScale() {
		scale.x += 0.1f;
		scale.y += 0.1f;
	}

	void setFirstRotation(float angle) {
		first_rotationAngle = glm::radians(angle);  // 입력은 각도, 내부에서는 라디안 사용
	}
	void setRotation(float angle) {
		rotationAngle = glm::radians(angle);  // 입력은 각도, 내부에서는 라디안 사용
	}

	void setColor(const glm::vec3& newcolor) {
		color = newcolor;
	}

	void make_Color_random() {
		color.r = (float)(rand() % 256) / 255.0f;
		color.g = (float)(rand() % 256) / 255.0f;
		color.b = (float)(rand() % 256) / 255.0f;
	}

	void setParent(Object& obj) {
		parent_location = obj.location;
		parent_rotationAngle = obj.rotationAngle;
		parent_scale = obj.scale;
	}

	void Draw_parent_cube() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);
		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// 이동
		transform_Matrix = glm::translate(transform_Matrix, location);
		// 자전
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		// 축소확대
		transform_Matrix = glm::scale(transform_Matrix, scale);

		ObjectTransform = glGetUniformLocation(SP.shaderID, "parent_transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		glm::vec4 transformedLocation = transform_Matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		new_location = glm::vec3(transformedLocation);

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
	void Draw_child_cube() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);
		// 공전
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		// 이동
		transform_Matrix = glm::translate(transform_Matrix, location);
		// 초기 자전
		transform_Matrix = glm::rotate(transform_Matrix, first_rotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));
		// 자전
		//transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		// 축소확대
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));


		transform_Matrix = glm::mat4(1.0f);
		// 이동
		transform_Matrix = glm::translate(transform_Matrix, parent_location);
		// 자전
		transform_Matrix = glm::rotate(transform_Matrix, parent_rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		// 축소확대
		transform_Matrix = glm::scale(transform_Matrix, parent_scale);

		ObjectTransform = glGetUniformLocation(SP.shaderID, "parent_transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		glm::vec4 transformedLocation = transform_Matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		new_location = glm::vec3(transformedLocation);

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}


	void Draw_line() {
		// 축을 그릴 때 변환을 적용하지 않음
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));
		ObjectTransform = glGetUniformLocation(SP.shaderID, "parent_transform");
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

vector<Object> object;

bool timer{ true };

bool draw_only_line{ false };
bool draw_enable{ true };

bool CHECK_B{ false };
bool CHECK_M{ false };
bool CHECK_F{ false };
bool CHECK_E{ false };
bool CHECK_T{ false };
bool CHECK_A{ false };

int radian{ 0 };
int direction{ 1 };

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
	glutCreateWindow("Example20");
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

	glutTimerFunc(17, TimerFunction, 1);
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

	// xz 직각투영 그리기
	glViewport(800, 400, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	camera.make_camera_orbit_xz();

	xyz_line[0].Draw_line();
	xyz_line[1].Draw_line();
	xyz_line[2].Draw_line();

	// 음면제거, 라인 따기
	if (draw_enable) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}
	else if (!draw_enable) glDisable(GL_CULL_FACE);

	if (draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (!draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	// 큐브 그리기 (변환 적용)

	for (auto& v : object) {
		if (v.what == MAIN) v.Draw_parent_cube();
		else if (v.what == CHILD) {
			v.setParent(object[0]);
			v.Draw_child_cube();
		}
	}


	// xy 직각투영 그리기
	glViewport(800, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	camera.make_camera_orbit_xy();

	xyz_line[0].Draw_line();
	xyz_line[1].Draw_line();
	xyz_line[2].Draw_line();

	// 음면제거, 라인 따기
	if (draw_enable) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}
	else if (!draw_enable) glDisable(GL_CULL_FACE);

	if (draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (!draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	// 큐브 그리기 (변환 적용)

	for (auto& v : object) {
		if (v.what == MAIN) v.Draw_parent_cube();
		else if (v.what == CHILD) {
			v.setParent(object[0]);
			v.Draw_child_cube();
		}
	}


	// 원근투영그리기
	glViewport(0, 0, 800, 800);
	camera.make_camera_perspective();

	xyz_line[0].Draw_line();
	xyz_line[1].Draw_line();
	xyz_line[2].Draw_line();

	// 음면제거, 라인 따기
	if (draw_enable) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}
	else if (!draw_enable) glDisable(GL_CULL_FACE);

	if (draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (!draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	// 큐브 그리기 (변환 적용)

	for (auto& v : object) {
		if (v.what == MAIN) v.Draw_parent_cube();
		else if (v.what == CHILD) {
			v.setParent(object[0]);
			v.Draw_child_cube();
		}
	}


	glutSwapBuffers(); //--- 화면에 출력하기
}
//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h)
{

}

void TimerFunction(int value) {
	if (value == 1) {
		if (timer) {
			if (CHECK_B) {
				if (object[0].location.x <= -4.0f || object[0].location.x >= 4.0f) {
					direction *= -1; // 방향 반전
				}
				// 위치 업데이트
				object[0].location.x += direction * 0.05f;
			}

			if (CHECK_M) {
				if (direction == 1) ++radian;
				else if (direction == -1) --radian;

				object[1].setRotation(radian);
				object[2].setRotation(radian);
				object[3].setRotation(radian);
			}

			if (CHECK_F) {
				if (direction == 1) ++radian;
				else if (direction == -1) --radian;

				object[4].setRotation(-radian);
				object[5].setRotation(radian);
			}

			if (CHECK_E) {
				if (object[4].rotationAngle != 0.0f) {
					object[4].setRotation(0);
					object[5].setRotation(0);
				}

				if (object[4].location.x < -0.8f || object[4].location.x > 0.8f) {
					direction *= -1; // 방향 반전
				}
				// 위치 업데이트
				object[4].location.x += direction * 0.01f;
				object[5].location.x -= direction * 0.01f;
			}

			if (CHECK_T) {
				if (direction == 1) ++radian;
				else if (direction == -1) --radian;

				object[2].setFirstRotation(radian);
				object[3].setFirstRotation(-radian);
			}

			if (CHECK_A) {
				++radian;
				camera.setRotation(radian);
				camera.make_camera_perspective();
			}
		}
		glutTimerFunc(17, TimerFunction, 1);
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

	else if (key == '1') {
		timer = true;
		glutTimerFunc(17, TimerFunction, 1);
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


	else if (key == 'r') {
		direction *= -1;
	}

	else if (key == 'b') {
		CHECK_B = true;
	}
	else if (key == 'B') {
		CHECK_B = false;
	}

	else if (key == 'm') {
		CHECK_M = true;
	}
	else if (key == 'M') {
		CHECK_M = false;
	}

	else if (key == 'f') {
		CHECK_F = true;
	}
	else if (key == 'F') {
		CHECK_F = false;
	}

	else if (key == 'e') {
		CHECK_E = true;
	}
	else if (key == 'E') {
		CHECK_E = false;
	}

	else if (key == 't') {
		CHECK_T = true;
	}
	else if (key == 'T') {
		CHECK_T = false;
	}


	else if (key == 'z') {
		camera.cameraPos.z += 1.0f;
		camera.make_camera_perspective();
	}
	else if (key == 'Z') {
		camera.cameraPos.z -= 1.0f;
		camera.make_camera_perspective();
	}

	else if (key == 'x') {
		camera.cameraPos.x += 1.0f;
		camera.make_camera_perspective();
	}
	else if (key == 'X') {
		camera.cameraPos.x -= 1.0f;
		camera.make_camera_perspective();
	}

	else if (key == 'a' || key == 'A') {
		CHECK_A = true;
	}

	else if (key == 's' || key == 'S') {
		CHECK_A = false;
		CHECK_B = false;
		CHECK_M = false;
		CHECK_F = false;
		CHECK_E = false;
		CHECK_T = false;
	}


	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y) {
	if (key == GLUT_KEY_F1) {
		initialize();

		draw_enable = true;
		draw_only_line = false;

		timer = false;

		radian = 0;
		direction = 1;
	}

	glutPostRedisplay();
}

void initialize() {
	camera.initialize();
	camera.make_camera_perspective();
	object.clear();

	xyz_line[0].initialize_Object("x_Line", { 0.0f, 0.0f, 0.0f });
	xyz_line[0].setColor({ 1.0f, 0.0f, 0.0f });
	xyz_line[1].initialize_Object("y_Line", { 0.0f, 0.0f, 0.0f });
	xyz_line[1].setColor({ 0.0f, 1.0f, 0.0f });
	xyz_line[2].initialize_Object("z_Line", { 0.0f, 0.0f, 0.0f });
	xyz_line[2].setColor({ 0.0f, 0.0f, 1.0f });

	Object temp_obj;
	temp_obj.initialize_Object("cube.obj", { 0.0f, 0.0f, 0.0f });
	temp_obj.setScale({ 1.2f, 1.2f, 1.2f });
	temp_obj.what = MAIN;
	object.push_back(temp_obj);

	temp_obj.initialize_Object("cube.obj", { 0.0f, 1.0f, 0.0f });
	temp_obj.what = CHILD;
	object.push_back(temp_obj);

	temp_obj.initialize_Object("long_cube.obj", { -0.3f, 0.0f, 0.0f });
	temp_obj.setScale({ 0.2f, 1.2f, 0.2f });
	temp_obj.what = CHILD;
	object.push_back(temp_obj);

	temp_obj.initialize_Object("long_cube.obj", { 0.3f, 0.0f, 0.0f });
	temp_obj.setScale({ 0.2f, 1.2f, 0.2f });
	temp_obj.what = CHILD;
	object.push_back(temp_obj);

	temp_obj.initialize_Object("long_cube.obj", { -0.8f, 0.0f, 0.2f });
	temp_obj.setFirstRotation(90);
	temp_obj.setScale({ 0.2f, 1.2f, 0.2f });
	temp_obj.what = CHILD;
	object.push_back(temp_obj);

	temp_obj.initialize_Object("long_cube.obj", { 0.8f, 0.0f, 0.2f });
	temp_obj.setFirstRotation(90);
	temp_obj.setScale({ 0.2f, 1.2f, 0.2f });
	temp_obj.what = CHILD;
	object.push_back(temp_obj);
}