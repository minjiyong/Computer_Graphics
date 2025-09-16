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
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <random>
#include <utility>
#include <math.h>
#include <optional>
#include <cmath>

#include "ShaderProgram.h"

using namespace std;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define LEFT -1
#define RIGHT 1


//--- 함수 선언 추가하기
std::pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WINDOW_WIDTH / 2)) / (WINDOW_WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (WINDOW_HEIGHT / 2)) / (WINDOW_HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

void TimerFunction(int value);
void TimerFunction_MakePolygon(int value);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
void SpecialKeyboard(int key, int x, int y);


// 교점이 중복되는지 확인하는 함수
bool isDuplicateIntersection(const glm::vec3& newIntersection, const std::vector<glm::vec3>& intersections, float epsilon = 0.0001f) {
	for (const auto& intersection : intersections) {
		if (glm::distance(intersection, newIntersection) < epsilon) {
			return true;
		}
	}
	return false;
}
// 두 선분 (p1, p2)와 (q1, q2)의 교차 여부와 교점을 계산하는 함수 (z값이 0.0f로 고정된 vec3 사용)
std::optional<glm::vec3> getIntersectionPoint(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& q1, const glm::vec3& q2) {
	float a1 = p2.y - p1.y;
	float b1 = p1.x - p2.x;
	float c1 = a1 * p1.x + b1 * p1.y;

	float a2 = q2.y - q1.y;
	float b2 = q1.x - q2.x;
	float c2 = a2 * q1.x + b2 * q1.y;

	float determinant = a1 * b2 - a2 * b1;
	float epsilon_determinant = 1e-6;  // 오차 보정

	// 두 선분이 평행하거나 거의 평행한 경우
	if (std::abs(determinant) < epsilon_determinant) {
		return std::nullopt;
	}
	else {
		float x = (b2 * c1 - b1 * c2) / determinant;
		float y = (a1 * c2 - a2 * c1) / determinant;
		glm::vec3 intersection(x, y, 0.0f);

		// 교점이 두 선분의 범위 내에 있는지 확인 (경계 오차 보정)
		float epsilon_range = 1e-5;
		auto withinRange = [epsilon_range](const glm::vec3& p, const glm::vec3& q, const glm::vec3& r) {
			return std::min(p.x, q.x) - epsilon_range <= r.x && r.x <= std::max(p.x, q.x) + epsilon_range &&
				std::min(p.y, q.y) - epsilon_range <= r.y && r.y <= std::max(p.y, q.y) + epsilon_range;
			};

		if (withinRange(p1, p2, intersection) && withinRange(q1, q2, intersection)) {
			return intersection;
		}
		else {
			return std::nullopt;
		}
	}
}
// 다각형과 직선의 충돌을 체크하고, 교점을 반환하는 함수
std::vector<glm::vec3> getPolygonLineIntersections(const std::vector<glm::vec3>& polygonVertices, const glm::vec3& lineStart, const glm::vec3& lineEnd) {
	std::vector<glm::vec3> intersections{};
	size_t numVertices = polygonVertices.size();
	for (size_t i = 0; i < numVertices; i++) {
		glm::vec3 polyStart = polygonVertices[i];
		glm::vec3 polyEnd = polygonVertices[(i + 1) % numVertices];

		std::optional<glm::vec3> intersection = getIntersectionPoint(lineStart, lineEnd, polyStart, polyEnd);
		if (intersection && !isDuplicateIntersection(*intersection, intersections)) {
			intersections.push_back(*intersection);
		}
	}

	return intersections;
}


// 회전 각도를 계산하는 함수
float calculateRotationAngle(const glm::vec3& lineStart, const glm::vec3& lineEnd) {
	glm::vec2 direction = lineEnd - lineStart;
	return -std::atan2(direction.y, direction.x);
}
// 정점을 회전시키는 함수
glm::vec3 rotatePoint(const glm::vec3& point, const glm::vec3& origin, float angle) {
	glm::vec2 translatedPoint = glm::vec2(point.x - origin.x, point.y - origin.y);
	glm::vec2 rotatedPoint = glm::vec2(
		translatedPoint.x * cos(angle) - translatedPoint.y * sin(angle),
		translatedPoint.x * sin(angle) + translatedPoint.y * cos(angle)
	);
	return glm::vec3(rotatedPoint.x + origin.x, rotatedPoint.y + origin.y, 0.0f);
}
// 다각형을 자르는 함수
std::pair<std::vector<glm::vec3>, std::vector<glm::vec3>> splitPolygonByLine(
	const std::vector<glm::vec3>& polygonVertices, const glm::vec3& lineStart, const glm::vec3& lineEnd) {

	float angle = calculateRotationAngle(lineStart, lineEnd);

	// 회전된 정점을 담을 벡터
	std::vector<glm::vec3> rotatedPolygonVertices;
	glm::vec3 rotatedLineStart = rotatePoint(lineStart, lineStart, angle);
	glm::vec3 rotatedLineEnd = rotatePoint(lineEnd, lineStart, angle);

	for (const auto& vertex : polygonVertices) {
		rotatedPolygonVertices.push_back(rotatePoint(vertex, lineStart, angle));
	}

	// y값을 기준으로 분리
	std::vector<glm::vec3> upperPolygon, lowerPolygon;
	for (const auto& vertex : rotatedPolygonVertices) {
		if (vertex.y > rotatedLineStart.y) {
			upperPolygon.push_back(vertex);
		}
		else {
			lowerPolygon.push_back(vertex);
		}
	}

	// 두 다각형을 원래 회전으로 복원
	for (auto& vertex : upperPolygon) {
		vertex = rotatePoint(vertex, lineStart, -angle);
	}
	for (auto& vertex : lowerPolygon) {
		vertex = rotatePoint(vertex, lineStart, -angle);
	}

	return { upperPolygon, lowerPolygon };
}


// 중심점을 계산하는 함수
glm::vec3 calculateCentroid(const std::vector<glm::vec3>& vertices) {
	glm::vec3 centroid(0.0f, 0.0f, 0.0f);
	for (const auto& vertex : vertices) {
		centroid += vertex;
	}
	centroid /= static_cast<float>(vertices.size());
	return centroid;
}
// 정점을 반시계방향으로 정렬하는 함수
void sortVerticesCounterClockwise(std::vector<glm::vec3>& vertices) {
	glm::vec3 centroid = calculateCentroid(vertices);

	std::sort(vertices.begin(), vertices.end(), [centroid](const glm::vec3& a, const glm::vec3& b) {
		float angleA = std::atan2(a.y - centroid.y, a.x - centroid.x);
		float angleB = std::atan2(b.y - centroid.y, b.x - centroid.x);
		return angleA < angleB;
		});
}


void initialize();
void make_polygon(const float poly_speed);
void make_polygon_with_location(const float poly_speed, float p1_x, float p1_y);
void basket_boundary_check();

void make_effect(float p1_x, float p1_y);


ShaderProgram SP;


class Object {
public:
	bool basket_check{ false };
	bool sliced_check{ false };

	int dir{};
	float speed{ 0.01f };
	float vertical_speed{ 0.0f };

	glm::vec3 location{};	// 위치(translate 적용)
	glm::vec3 color{};		// 색상

	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };  // 크기 (디폴트로 1)
	float rotationAngle = 0.0f;  // 회전 각도 (라디안)

	std::vector<glm::vec3> object_vertices;
	std::vector<glm::vec3> orbit_vertices;

	void initialize_Object(glm::vec3 pos) {
		object_vertices.clear();
		location = pos;
		scale = { 0.2f, 0.2f, 0.2f };
		make_Color_random();
	}

	void make_Line(float x1, float y1, float x2, float y2) {
		object_vertices.clear();  // 기존 정점 초기화
		object_vertices.push_back(glm::vec3(x1, y1, 0.0f));  // 첫 번째 정점
		object_vertices.push_back(glm::vec3(x2, y2, 0.0f));   // 두 번째 정점
	}
	// 정해진 각 수에 따라 다각형을 만드는 함수
	void make_Polygon(int sides) {
		object_vertices.clear();
		object_vertices.reserve(8);

		float radius = 0.6f;  // 각형의 반지름 (크기)
		float angleIncrement = 2.0f * 3.14159265358979323846f / sides;  // 각 정점 사이의 각도

		for (int i = 0; i < sides; ++i) {
			float angle = i * angleIncrement;
			float x = radius * cos(angle);
			float y = radius * sin(angle);
			object_vertices.push_back(glm::vec3(x, y, 0.0f));
		}
	}

	void setScale(const glm::vec3& newScale) {
		scale = newScale;
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

	void lowerScale() {
		scale.x -= 0.001f;
		scale.y -= 0.001f;
		scale.z -= 0.001f;
	}

	// 정점에 변환을 적용해서 정점별 실제 좌표를 구하는 함수
	std::vector<glm::vec3> calculateTransformedVertices() {
		std::vector<glm::vec3> real_object_vertices;

		// 위치 및 크기 변환 행렬을 생성
		glm::mat4 transform_Matrix = glm::mat4(1.0f);
		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		// 변환 행렬을 각 정점에 적용
		for (const auto& v : object_vertices) {
			glm::vec4 transformed_vertex = transform_Matrix * glm::vec4(v, 1.0f); // vec3을 vec4로 확장
			real_object_vertices.push_back(glm::vec3(transformed_vertex)); // vec3로 다시 변환하여 저장
		}

		return real_object_vertices;
	}
	// 실제 좌표로 만들어진 정점들을 역연산해서 원본처럼 만드는 함수
	std::vector<glm::vec3> calculateOriginalVertices(const std::vector<glm::vec3>& transformedVertices) {
		std::vector<glm::vec3> original_object_vertices;

		// 위치 및 크기 변환 행렬의 역행렬을 생성
		glm::mat4 transform_Matrix = glm::mat4(1.0f);
		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);
		glm::mat4 inverse_transform_Matrix = glm::inverse(transform_Matrix);

		// 역행렬을 각 변환된 정점에 적용하여 원본 좌표를 복원
		for (const auto& v : transformedVertices) {
			glm::vec4 original_vertex = inverse_transform_Matrix * glm::vec4(v, 1.0f); // vec3을 vec4로 확장
			original_object_vertices.push_back(glm::vec3(original_vertex)); // vec3로 다시 변환하여 저장
		}
		return original_object_vertices;
	}

	void Draw_object(int num_vertices) {
		// VAO, VBO 생성
		unsigned int objectVAO, objectVBO;
		glGenVertexArrays(1, &objectVAO);
		glGenBuffers(1, &objectVBO);

		glBindVertexArray(objectVAO);
		glBindBuffer(GL_ARRAY_BUFFER, objectVBO);
		glBufferData(GL_ARRAY_BUFFER, object_vertices.size() * sizeof(glm::vec3), &object_vertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// 궤적을 그리기 위해 필요한 셰이더와 행렬 설정
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		// 궤적 그리기
		glBindVertexArray(objectVAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, num_vertices);  // 점으로 표시하고 싶다면 GL_POINTS로 변경
		glBindVertexArray(0);

		// 메모리 해제
		glDeleteBuffers(1, &objectVBO);
		glDeleteVertexArrays(1, &objectVAO);
	}
	void Draw_line() {
		// VAO, VBO 생성
		unsigned int objectVAO, objectVBO;
		glGenVertexArrays(1, &objectVAO);
		glGenBuffers(1, &objectVBO);

		glBindVertexArray(objectVAO);
		glBindBuffer(GL_ARRAY_BUFFER, objectVBO);
		glBufferData(GL_ARRAY_BUFFER, object_vertices.size() * sizeof(glm::vec3), &object_vertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// 궤적을 그리기 위해 필요한 셰이더와 행렬 설정
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		// 궤적 그리기
		glBindVertexArray(objectVAO);
		glDrawArrays(GL_LINES, 0, 2);  // 점으로 표시하고 싶다면 GL_POINTS로 변경
		glBindVertexArray(0);

		// 메모리 해제
		glDeleteVertexArrays(1, &objectVAO);
		glDeleteBuffers(1, &objectVBO);
	}

	void Draw_orbit() {
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

		// 경로는 행렬 곱셈할 필요 없음. 이미 변환된 location 값을 집어넣고 있음.
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// 궤적을 그리기 위해 필요한 셰이더와 행렬 설정
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		// 궤적 그리기
		glBindVertexArray(orbitVAO);
		glDrawArrays(GL_LINES, 0, orbit_vertices.size());  // 점으로 표시하고 싶다면 GL_POINTS로 변경
		glBindVertexArray(0);

		// 메모리 해제
		glDeleteVertexArrays(1, &orbitVAO);
		glDeleteBuffers(1, &orbitVBO);
	}
	void Draw_effect(int num_vertices) {
		// VAO, VBO 생성
		unsigned int objectVAO, objectVBO;
		glGenVertexArrays(1, &objectVAO);
		glGenBuffers(1, &objectVBO);

		glBindVertexArray(objectVAO);
		glBindBuffer(GL_ARRAY_BUFFER, objectVBO);
		glBufferData(GL_ARRAY_BUFFER, object_vertices.size() * sizeof(glm::vec3), &object_vertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// 궤적을 그리기 위해 필요한 셰이더와 행렬 설정
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		// 궤적 그리기
		glBindVertexArray(objectVAO);
		glDrawArrays(GL_LINE_LOOP, 0, num_vertices);  // 점으로 표시하고 싶다면 GL_POINTS로 변경
		glBindVertexArray(0);

		// 메모리 해제
		glDeleteBuffers(1, &objectVBO);
		glDeleteVertexArrays(1, &objectVAO);
	}
};


vector<Object> object{};				// 생성되는 오브젝트들
Object line{};							// 자르는 선
Object basket{};						// 바구니 선
vector<Object> effect{};				// 이펙트 

float gravity = -0.00003f;		// 중력 가속도
float poly_speed = 0.01f;		// 도형들의 속도 - x축

int made_polygon{ 0 };				// 생성한 폴리곤의 수
bool timer1{ true };				// 타이머 플래그
bool timer2{ true };				// 타이머 플래그
bool draw_only_line{ false };		// draw 종류 플래그
bool draw_orbit{ false };			// 경로 출력할지 플래그

float line_x1{};					// 선의 시작, 끝 점
float line_y1{};					// 선의 시작, 끝 점
float line_x2{};					// 선의 시작, 끝 점
float line_y2{};					// 선의 시작, 끝 점



// 교점을 저장하는 벡터
vector<glm::vec3> intersections{};
// 상하로 나눠진 도형의 전체 교점을 저장할 벡터
std::pair<std::vector<glm::vec3>, std::vector<glm::vec3>> new_polygons{};


// 도형과 바구니의 충돌을 체크하는 함수
void basket_collision_with_object(Object& o);


int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	srand(static_cast<int>(time(NULL)));

	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Polygon Slice!");
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


	SP.make_vertexShaders(); //--- 버텍스 세이더 만들기
	SP.make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	SP.make_shaderProgram();

	initialize();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);

	glutTimerFunc(17, TimerFunction, 1);
	glutTimerFunc(150, TimerFunction_MakePolygon, 2);
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
	glClearColor(0.8235f, 0.8235f, 0.8235f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- 렌더링 파이프라인에 세이더 불러오기
	glUseProgram(SP.shaderID);

	glEnable(GL_DEPTH_TEST);

	if (draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (!draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	glLineWidth(1.0f);
	line.Draw_line();

	for (auto& v : object) {
		v.Draw_object(v.object_vertices.size());
		if (draw_orbit && v.orbit_vertices.size() != 0) v.Draw_orbit();
	}
	for (auto& v : effect) v.Draw_effect(v.object_vertices.size());
	

	glLineWidth(5.0f);
	basket.Draw_line();


	glutSwapBuffers(); //--- 화면에 출력하기
}
//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void TimerFunction(int value) {
	// 이동 타이머
	if (value == 1) {
		if (timer1) {
			for (auto& o : object) {
				// 바구니와 도형의 충돌체크
				basket_collision_with_object(o);

				// 오브젝트 이동
				if (o.sliced_check) {
					if (!o.orbit_vertices.empty()) o.orbit_vertices.clear();
					
					if (o.basket_check) {
						o.dir = basket.dir;
						o.speed = basket.speed;
					}
					else if (!o.basket_check) {
						o.speed = 0.001f;
						o.location.y -= 0.005f;
					}
				}
				else if (!o.sliced_check) {
					o.location.y += o.vertical_speed;

					// 수직 속도에 중력 가속도 적용
					o.vertical_speed += gravity;

					o.rotationAngle += 0.005f;
					
					// 경로 그리기 (잘리지 않았을 때만)
					if (draw_orbit) o.orbit_vertices.push_back(o.location);
				}

				o.location.x += o.dir * o.speed;
			}

			// 바구니 이동
			basket.location.x += basket.dir * basket.speed;
			basket_boundary_check();

			// 도형이 화면 밖을 넘어가면 삭제
			for (auto it = object.begin(); it != object.end();) {
				if (it->location.x < -1.5f || it->location.x > 1.5f || it->location.y < -1.5f) {
					it = object.erase(it); 
				}
				else {
					++it; 
				}
			}

			// 이펙트 관리
			for (int i = effect.size() - 1; i >= 0; --i) {
				float dx = 0.01f * cos(effect[i].rotationAngle);
				float dy = 0.01f * sin(effect[i].rotationAngle);
				effect[i].location.x += dx;
				effect[i].location.y += dy;

				effect[i].lowerScale();

				if (effect[i].scale.x < 0.01f) effect.erase(effect.begin() + i);
			}

			glutTimerFunc(17, TimerFunction, 1);
		}
	}
	glutPostRedisplay();
}
void TimerFunction_MakePolygon(int value) {
	// 도형 생성 타이머
	if (value == 2) {
		if (timer2) {
			make_polygon(poly_speed);

			glutTimerFunc(1500, TimerFunction_MakePolygon, 2);
		}
	}
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		auto mouse = ConvertWinToGL(x, y);
		line_x1 = mouse.first;
		line_y1 = mouse.second;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		auto mouse = ConvertWinToGL(x, y);
		line_x2 = mouse.first;
		line_y2 = mouse.second;

		line.make_Line(line_x1, line_y1, line_x2, line_y2);

		std::vector<Object> new_objects;
		
		for (size_t i = 0; i < object.size(); ++i) {
			auto& o = object[i];

			// 도형 처리 로직
			vector<glm::vec3> real_object_vertices = o.calculateTransformedVertices();
			intersections = getPolygonLineIntersections(real_object_vertices, line.object_vertices.front(), line.object_vertices.back());
			auto new_polygons = splitPolygonByLine(real_object_vertices, line.object_vertices.front(), line.object_vertices.back());

			// 교점이 하나만 있을 경우 다음 도형으로 넘어감
			if (intersections.size() == 1) {
				continue;
			}

			if (!intersections.empty()) {
				// 위 도형 처리
				Object temp_object_upper;
				temp_object_upper.object_vertices.clear();
				for (auto& v : new_polygons.first) temp_object_upper.object_vertices.push_back(v);
				for (auto& i : intersections) temp_object_upper.object_vertices.push_back(i);
				sortVerticesCounterClockwise(temp_object_upper.object_vertices);

				o.object_vertices = temp_object_upper.object_vertices;
				o.object_vertices = o.calculateOriginalVertices(o.object_vertices);
				o.dir = RIGHT;
				o.sliced_check = true;

				// 아래 도형 처리
				Object temp_object_lower;
				temp_object_lower.object_vertices.clear();
				for (auto& v : new_polygons.second) temp_object_lower.object_vertices.push_back(v);
				for (auto& i : intersections) temp_object_lower.object_vertices.push_back(i);
				sortVerticesCounterClockwise(temp_object_lower.object_vertices);

				temp_object_lower.location = o.location;
				temp_object_lower.rotationAngle = o.rotationAngle;
				temp_object_lower.scale = o.scale;
				temp_object_lower.object_vertices = o.calculateOriginalVertices(temp_object_lower.object_vertices);
				temp_object_lower.setColor(o.color);
				temp_object_lower.dir = LEFT;
				temp_object_lower.sliced_check = true;

				// 새로 추가할 도형을 new_objects 벡터에 저장
				new_objects.push_back(temp_object_lower);
				// 이펙트로 사용할 도형을 저장
				for (int i = 0; i < 6; ++i) make_effect(o.location.x, o.location.y);
			}
		}

		// 따로 저장한 lower 도형들을 원본 object 벡터에 넣는다!
		for (auto& n : new_objects) object.push_back(n);
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		auto mouse = ConvertWinToGL(x, y);

		make_polygon_with_location(poly_speed, mouse.first, mouse.second);
	}

	glutPostRedisplay();
}

void Motion(int x, int y) {
	auto mouse = ConvertWinToGL(x, y);

	line_x2 = mouse.first;
	line_y2 = mouse.second;

	line.make_Line(line_x1, line_y1, line_x2, line_y2);

	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	if (key == 'q') {
		glutDestroyWindow(true);
	}
	if (key == 's') {
		initialize();
	}
	
	else if (key == '1') {
		timer1 = false;
	}
	else if (key == '2') {
		timer2 = false;
	}


	else if (key == 'w') {
		draw_only_line = true;
	}
	else if (key == 'W') {
		draw_only_line = false;
	}


	else if (key == 'a') {
		draw_orbit = true;
	}
	else if (key == 'A') {
		draw_orbit = false;
		for (auto& o : object) o.orbit_vertices.clear();
	}


	else if (key == '+') {
		if (gravity + -0.00001f <= -0.0001f) return;
		for (auto& o : object) {
			o.speed += 0.001f;
		}
		poly_speed += 0.001f;
		gravity += -0.00001f;
		cout << "Speed Level : " << int(poly_speed / 0.001f)  - 7<< ", Polygon speed : " << poly_speed << ", Gravity : " << gravity << endl;
	}
	else if (key == '-') {
		if (gravity - -0.00001f >= 0.0f) return;
		for (auto& o : object) {
			o.speed -= 0.001f;
		}
		poly_speed -= 0.001f;
		gravity -= -0.00001f;
		cout << "Speed Level : " << int(poly_speed / 0.001f) - 7<< ", polygon speed : " << poly_speed << ", gravity : " << gravity << endl;
	}


	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y) {
	

	glutPostRedisplay();
}

void initialize() {
	gravity = -0.00003f;		// 중력 가속도
	poly_speed = 0.01f;		// 도형들의 속도 - x축
	
	made_polygon = 0;
	timer1 = true;
	timer2 = true;
	draw_only_line = false;		// draw 종류 플래그
	draw_orbit = false ;			// 경로 출력할지 플래그

	object.clear();

	line.location = { 0.0f, 0.0f, 0.0f };
	line.setColor({ 1.0f, 0.0f, 0.0f });
	line.make_Line(line_x1, line_y1, line_x2, line_y2);

	basket.location = { 0.0f, -0.6f, 0.0f };
	basket.setColor({ 0.0f, 0.0f, 1.0f });
	basket.make_Line(-0.15f, 0.0f, 0.15f, 0.0f);
	basket.dir = LEFT;
}

void make_polygon(const float poly_speed) {
	Object temp_obj{};
	
	std::random_device rd;     
	std::mt19937 gen(rd());
	uniform_int_distribution<int> uid(3, 8);
	int poly_points{};
	poly_points = uid(gen);

	if (made_polygon % 2 == 0) {
		temp_obj.initialize_Object({ 1.1f, 0.8f, 0.0f });
		temp_obj.location.y -= rand() % 41 / 100.0f;
		temp_obj.make_Polygon(poly_points);
		temp_obj.dir = LEFT;
		temp_obj.speed = poly_speed;
		object.push_back(temp_obj);
	}
	else if (made_polygon % 2 == 1) {
		temp_obj.initialize_Object({ -1.1f, 0.8f, 0.0f });
		temp_obj.location.y -= rand() % 41 / 100.0f;
		temp_obj.make_Polygon(poly_points);
		temp_obj.dir = RIGHT;
		temp_obj.speed = poly_speed;
		object.push_back(temp_obj);
	}
	++made_polygon;
}

void make_polygon_with_location(const float poly_speed, float p1_x, float p1_y) {
	Object temp_obj{};

	std::random_device rd;
	std::mt19937 gen(rd());
	uniform_int_distribution<int> uid(3, 8);
	int poly_points{};
	poly_points = uid(gen);

	int temp_dir{};
	temp_dir = rand() % 2;

	temp_obj.initialize_Object({ p1_x, p1_y, 0.0f });
	temp_obj.make_Polygon(poly_points);

	if(temp_dir == 0) temp_obj.dir = RIGHT;
	else if(temp_dir == 1) temp_obj.dir = LEFT;
	
	temp_obj.speed = poly_speed;
	object.push_back(temp_obj);
}


void basket_boundary_check() {
	if (basket.object_vertices.front().x + basket.location.x < -1.0f) basket.dir = RIGHT;
	else if (basket.object_vertices.back().x + basket.location.x > 1.0f) basket.dir = LEFT;
}

void basket_collision_with_object(Object& o) {
	vector<glm::vec3> real_object_vertices = o.calculateTransformedVertices();
	vector<glm::vec3> real_basket_vertices;
	real_basket_vertices.push_back(basket.object_vertices.front() + basket.location);
	real_basket_vertices.push_back(basket.object_vertices.back() + basket.location);

	glm::vec3 y_min{ 10.0f, 10.0f, 10.0f };
	// y 값이 제일 작은 점 판별
	for (auto& v : real_object_vertices) {
		if (y_min.y > v.y) y_min = v;
	}

	// 충돌체크
	if (real_basket_vertices.front().x <= y_min.x && real_basket_vertices.back().x >= y_min.x && real_basket_vertices.front().y >= y_min.y && real_basket_vertices.front().y - 0.01f <= y_min.y) {
		o.basket_check = true;
	}
	else o.basket_check = false;
}

void make_effect(float p1_x, float p1_y) {
	Object temp_obj{};

	std::random_device rd;
	std::mt19937 gen(rd());
	uniform_int_distribution<int> uid(3, 8);
	uniform_int_distribution<int> uid_a(0, 360);
	int poly_points{};
	poly_points = uid(gen);
	int move_angle{};
	move_angle = uid_a(gen);

	temp_obj.initialize_Object({ p1_x, p1_y, 0.0f });
	temp_obj.setRotation(move_angle);
	temp_obj.setScale({ 0.08f, 0.08f, 0.08f });
	temp_obj.make_Polygon(poly_points);

	temp_obj.speed = poly_speed;
	effect.push_back(temp_obj);
}