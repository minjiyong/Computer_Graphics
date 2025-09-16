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

#define MAIN 0
#define FIRST45 1
#define FIRSTREV45 2
#define LEAF 3
#define FIRSTNORMAL 4


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
	glm::vec3 cameraPos = glm::vec3(20.0f, 15.0f, 20.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float rotationAngle = 0.0f;

	float fov = 45.0f;
	float aspectRatio = 800.0f / 800.0f;
	float nearClip = 0.1f;
	float farClip = 100.0f;

	void initialize() {
		cameraPos = glm::vec3(20.0f, 15.0f, 20.0f);
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

		glUniform3fv(glGetUniformLocation(SP.shaderID, "viewPos"), 1, glm::value_ptr(glm::vec3(rotatedCameraPos)));
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

class Light {
public:
	glm::vec3 position{ 25.0f, 0.0f, 0.0f };     // 광원의 위치
	glm::vec3 color{ 1.0f, 1.0f, 1.0f };        // 광원의 색상
	float intensity{ 1.0f };        // 광원의 강도

	float rotationAngle = 0.0f;

	void setRotation(float angle) {
		rotationAngle = glm::radians(angle);  // 입력은 각도, 내부에서는 라디안 사용
	}

	void sendToShader() {
		// 광원의 위치를 회전시키기 위해, 회전 행렬을 적용
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 rotatedLightPos = rotationMatrix * glm::vec4(position, 1.0f); // 회전된 카메라 위치

		// position uniform 전달
		glUniform3fv(glGetUniformLocation(SP.shaderID, "lightPos"), 1, glm::value_ptr(glm::vec3(rotatedLightPos)));
		// color uniform 전달
		glUniform3fv(glGetUniformLocation(SP.shaderID, "lightColor"), 1, glm::value_ptr(color));
		// intensity uniform 전달
		glUniform1f(glGetUniformLocation(SP.shaderID, "lightIntensity"), intensity);
	}
};


class Object {
public:
	int what{ 0 };

	static bool del;
	GLuint objVAO{};
	string objName{};

	glm::vec3 location{};	// 위치(translate 적용)
	glm::vec3 color{};		// 색상

	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };  // 크기 (디폴트로 1)
	float rotationAngle = 0.0f;  // 회전 각도 (라디안)

	glm::vec3 parent_location{};
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
	}
	void setMiniParent(Object& obj) {
		parent_location = obj.new_location;
	}


	void Draw_orbit(float radius, int num_segments) {
		glm::vec3 orbit_color{ 0.0f,0.0f,0.0f };

		for (int i = 0; i < num_segments; i++) {
			float theta = 2.0f * 3.1415926f * float(i) / float(num_segments); // 각도 계산
			float x = radius * cosf(theta);
			float z = radius * sinf(theta);
			orbit_vertices.push_back(glm::vec3(x, 0.0f, z));
		}

		// VAO, VBO 생성
		unsigned int orbitVAO, orbitVBO;
		glGenVertexArrays(1, &orbitVAO);
		glGenBuffers(1, &orbitVBO);

		glBindVertexArray(orbitVAO);
		glBindBuffer(GL_ARRAY_BUFFER, orbitVBO);
		glBufferData(GL_ARRAY_BUFFER, orbit_vertices.size() * sizeof(glm::vec3), &orbit_vertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, new_location);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// 궤적을 그리기 위해 필요한 셰이더와 행렬 설정
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(orbit_color));

		// 궤적 그리기
		glBindVertexArray(orbitVAO);
		glDrawArrays(GL_LINE_LOOP, 0, num_segments);  // 점으로 표시하고 싶다면 GL_POINTS로 변경
		glBindVertexArray(0);

		// 메모리 해제
		glDeleteVertexArrays(1, &orbitVAO);
		glDeleteBuffers(1, &orbitVBO);
	}
	void Draw_orbit_angle(float radius, int num_segments, float angle) {
		glm::vec3 orbit_color{ 0.0f,0.0f,0.0f };

		for (int i = 0; i < num_segments; i++) {
			float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);  // 각도
			float x = radius * cosf(theta);
			float z = radius * sinf(theta);
			orbit_vertices.push_back(glm::vec3(x, 0.0f, z));
		}

		// VAO, VBO 생성
		unsigned int orbitVAO, orbitVBO;
		glGenVertexArrays(1, &orbitVAO);
		glGenBuffers(1, &orbitVBO);

		glBindVertexArray(orbitVAO);
		glBindBuffer(GL_ARRAY_BUFFER, orbitVBO);
		glBufferData(GL_ARRAY_BUFFER, orbit_vertices.size() * sizeof(glm::vec3), &orbit_vertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, new_location);
		transform_Matrix = glm::rotate(transform_Matrix, glm::radians(angle), glm::vec3(1.0f, 0.0f, 1.0f));

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// 궤적을 그리기 위해 필요한 셰이더와 행렬 설정
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(orbit_color));

		// 궤적 그리기
		glBindVertexArray(orbitVAO);
		glDrawArrays(GL_LINE_LOOP, 0, num_segments);  // 점으로 표시하고 싶다면 GL_POINTS로 변경
		glBindVertexArray(0);

		// 메모리 해제
		glDeleteVertexArrays(1, &orbitVAO);
		glDeleteBuffers(1, &orbitVBO);
	}

	void Draw_sphere() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		// 부모로 이동
		transform_Matrix = glm::translate(transform_Matrix, parent_location);
		// 공전(이동 후 회전하니까)
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		// 이동
		transform_Matrix = glm::translate(transform_Matrix, location);
		// 자전
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		// 축소확대
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		glm::vec4 transformedLocation = transform_Matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		new_location = glm::vec3(transformedLocation);

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 360);
		glBindVertexArray(0);
	}

	void Draw_sphere_root() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);
		// 이동
		transform_Matrix = glm::translate(transform_Matrix, location);
		// 자전
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		// 축소확대
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		glm::vec4 transformedLocation = transform_Matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		new_location = glm::vec3(transformedLocation);

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 360);
		glBindVertexArray(0);
	}

	void Draw_sphere_45() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		// 부모로 이동
		transform_Matrix = glm::translate(transform_Matrix, parent_location);
		// 공전(이동 후 회전하니까)
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(-0.7071f, 1.0f, 0.7071f));
		// 이동
		transform_Matrix = glm::translate(transform_Matrix, location);
		// 자전
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		// 축소확대
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		glm::vec4 transformedLocation = transform_Matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		new_location = glm::vec3(transformedLocation);

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 360);
		glBindVertexArray(0);
	}

	void Draw_sphere_rev45() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		// 부모로 이동
		transform_Matrix = glm::translate(transform_Matrix, parent_location);
		// 공전(이동 후 회전하니까)
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.7071f, 1.0f, -0.7071f));
		// 이동
		transform_Matrix = glm::translate(transform_Matrix, location);
		// 자전
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		// 축소확대
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		glm::vec4 transformedLocation = transform_Matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		new_location = glm::vec3(transformedLocation);

		// color를 셰이더로 전달
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 360);
		glBindVertexArray(0);
	}

	void Draw_cube() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		// 이동
		transform_Matrix = glm::translate(transform_Matrix, location);
		// 자전
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		// 축소확대
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));
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
Light light;

Object light_object;

Object xyz_line[3];

vector<Object> object;

bool timer{ false };

bool draw_only_line{ false };
bool draw_enable{ true };

bool CHECK_LIGHT_ROTATE{ false };
bool CHECK_PLANET_MOVE{ false };
float light_radian{ 0.0f };

int radian{ 0 };
int satelite_radian{ 0 };
bool normal_side{ true };

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
	glutCreateWindow("Example25");
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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- 렌더링 파이프라인에 세이더 불러오기
	glUseProgram(SP.shaderID);


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


	light_object.Draw_cube();


	// 큐브 그리기 (변환 적용)
	object[1].setMiniParent(object[0]);
	object[2].setMiniParent(object[0]);
	object[3].setMiniParent(object[0]);
	object[4].setMiniParent(object[1]);
	object[5].setMiniParent(object[2]);
	object[6].setMiniParent(object[3]);

	for (auto& v : object) {
		if (v.objName == "sphere.obj") {
			if (v.what == FIRSTNORMAL) {
				v.Draw_sphere();
				v.Draw_orbit(3.0f, 360);
			}
			else if (v.what == FIRST45) {
				v.Draw_sphere_45();
				v.Draw_orbit(3.0f, 360);
			}
			else if (v.what == FIRSTREV45) {
				v.Draw_sphere_rev45();
				v.Draw_orbit(3.0f, 360);
			}
			else if (v.what == LEAF) {
				v.Draw_sphere();
			}
			else {
				v.Draw_sphere_root();
				v.Draw_orbit(7.071f, 360);
				v.Draw_orbit_angle(7.071f, 360, 45.0f);
				v.Draw_orbit_angle(7.071f, 360, -45.0f);
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
		if (timer) {
			if (CHECK_PLANET_MOVE) {
				if (normal_side) {
					++radian;
					satelite_radian += 2;
				}
				else if (!normal_side) {
					--radian;
					satelite_radian -= 2;
				}

				for (auto& v : object) {
					if (v.what == 3) v.setRotation(satelite_radian);
					else v.setRotation(radian);
				}
			}

			if (CHECK_LIGHT_ROTATE) {
				++light_radian;
				light.setRotation(light_radian);
				light.sendToShader();

				light_object.setRotation(light_radian);
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

	else if (key == '0') {
		light.color = { 1.0f, 1.0f, 1.0f };
		light.sendToShader();
	}
	else if (key == '1') {
		light.color = { 1.0f, 0.0f, 0.0f };
		light.sendToShader();
	}
	else if (key == '2') {
		light.color = { 0.0f, 1.0f, 0.0f };
		light.sendToShader();
	}
	else if (key == '3') {
		light.color = { 0.0f, 0.0f, 1.0f };
		light.sendToShader();
	}

	else if (key == '+') {
		for (auto& v : object) {
			v.location.z += 1.0f;
		}
	}
	else if (key == '-') {
		for (auto& v : object) {
			v.location.z -= 1.0f;
		}
	}

	else if (key == 'r') {
		CHECK_LIGHT_ROTATE = true;
		timer = true;
	}
	else if (key == 'x') {
		CHECK_PLANET_MOVE = true;
		timer = true;
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

	else if (key == 'P') {
		camera.make_camera_perspective();
	}

	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y) {
	if (key == GLUT_KEY_F1) {
		initialize();

		draw_enable = false;
		draw_only_line = false;

		timer = false;

		radian = 0;
		normal_side = true;
	}

	else if (key == GLUT_KEY_UP) {
		for (auto& v : object) {
			v.location.y += 1.0f;
		}
	}
	else if (key == GLUT_KEY_LEFT) {
		for (auto& v : object) {
			v.location.x -= 1.0f;
		}
	}
	else if (key == GLUT_KEY_DOWN) {
		for (auto& v : object) {
			v.location.y -= 1.0f;
		}
	}
	else if (key == GLUT_KEY_RIGHT) {
		for (auto& v : object) {
			v.location.x += 1.0f;
		}
	}

	glutPostRedisplay();
}

void initialize() {
	camera.initialize();
	camera.make_camera_perspective();
	light.sendToShader();

	object.clear();

	xyz_line[0].initialize_Object("x_Line", { 0.0f, 0.0f, 0.0f });
	xyz_line[0].setColor({ 1.0f, 0.0f, 0.0f });
	xyz_line[1].initialize_Object("y_Line", { 0.0f, 0.0f, 0.0f });
	xyz_line[1].setColor({ 0.0f, 1.0f, 0.0f });
	xyz_line[2].initialize_Object("z_Line", { 0.0f, 0.0f, 0.0f });
	xyz_line[2].setColor({ 0.0f, 0.0f, 1.0f });

	light_object.initialize_Object("cube.obj", { 12.0f, 0.0f, 0.0f });
	light_object.setScale({ 0.02f, 0.02f, 0.02f });

	Object temp_obj;
	temp_obj.initialize_Object("sphere.obj", { 0.0f, 0.0f, 0.0f });
	object.push_back(temp_obj);

	// Object[1]
	temp_obj.initialize_Object("sphere.obj", { 7.071f, 0.0f, 0.0f });
	temp_obj.setScale({ 0.25f, 0.25f, 0.25f });
	temp_obj.setParent(object[0]);
	temp_obj.what = FIRSTNORMAL;
	object.push_back(temp_obj);
	// Object[2]
	temp_obj.initialize_Object("sphere.obj", { 5.0f, 5.0f, 0.0f });
	temp_obj.setScale({ 0.25f, 0.25f, 0.25f });
	temp_obj.setParent(object[0]);
	temp_obj.what = FIRST45;
	object.push_back(temp_obj);
	// Object[3]
	temp_obj.initialize_Object("sphere.obj", { 5.0f, -5.0f, 0.0f });
	temp_obj.setScale({ 0.25f, 0.25f, 0.25f });
	temp_obj.setParent(object[0]);
	temp_obj.what = FIRSTREV45;
	object.push_back(temp_obj);


	temp_obj.initialize_Object("sphere.obj", { 3.0f, 0.0f, 0.0f });
	temp_obj.setScale({ 0.12f, 0.12f, 0.12f });
	temp_obj.setColor({ 0.0f, 0.0f, 1.0f });
	temp_obj.setMiniParent(object[1]);
	temp_obj.what = LEAF;
	object.push_back(temp_obj);

	temp_obj.initialize_Object("sphere.obj", { 3.0f, 0.0f, 0.0f });
	temp_obj.setScale({ 0.12f, 0.12f, 0.12f });
	temp_obj.setColor({ 0.0f, 0.0f, 1.0f });
	temp_obj.setMiniParent(object[2]);
	temp_obj.what = LEAF;
	object.push_back(temp_obj);

	temp_obj.initialize_Object("sphere.obj", { 3.0f, 0.0f, 0.0f });
	temp_obj.setScale({ 0.12f, 0.12f, 0.12f });
	temp_obj.setColor({ 0.0f, 0.0f, 1.0f });
	temp_obj.setMiniParent(object[3]);
	temp_obj.what = LEAF;
	object.push_back(temp_obj);
}