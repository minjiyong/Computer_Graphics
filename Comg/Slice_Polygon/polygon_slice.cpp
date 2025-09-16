#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
//--- ���� �Լ�
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glew.h> //--- �ʿ��� ������� include
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


//--- �Լ� ���� �߰��ϱ�
std::pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WINDOW_WIDTH / 2)) / (WINDOW_WIDTH / 2); //gl��ǥ��� ����
	float my = -((float)y - (WINDOW_HEIGHT / 2)) / (WINDOW_HEIGHT / 2); //gl��ǥ��� ����
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


// ������ �ߺ��Ǵ��� Ȯ���ϴ� �Լ�
bool isDuplicateIntersection(const glm::vec3& newIntersection, const std::vector<glm::vec3>& intersections, float epsilon = 0.0001f) {
	for (const auto& intersection : intersections) {
		if (glm::distance(intersection, newIntersection) < epsilon) {
			return true;
		}
	}
	return false;
}
// �� ���� (p1, p2)�� (q1, q2)�� ���� ���ο� ������ ����ϴ� �Լ� (z���� 0.0f�� ������ vec3 ���)
std::optional<glm::vec3> getIntersectionPoint(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& q1, const glm::vec3& q2) {
	float a1 = p2.y - p1.y;
	float b1 = p1.x - p2.x;
	float c1 = a1 * p1.x + b1 * p1.y;

	float a2 = q2.y - q1.y;
	float b2 = q1.x - q2.x;
	float c2 = a2 * q1.x + b2 * q1.y;

	float determinant = a1 * b2 - a2 * b1;
	float epsilon_determinant = 1e-6;  // ���� ����

	// �� ������ �����ϰų� ���� ������ ���
	if (std::abs(determinant) < epsilon_determinant) {
		return std::nullopt;
	}
	else {
		float x = (b2 * c1 - b1 * c2) / determinant;
		float y = (a1 * c2 - a2 * c1) / determinant;
		glm::vec3 intersection(x, y, 0.0f);

		// ������ �� ������ ���� ���� �ִ��� Ȯ�� (��� ���� ����)
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
// �ٰ����� ������ �浹�� üũ�ϰ�, ������ ��ȯ�ϴ� �Լ�
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


// ȸ�� ������ ����ϴ� �Լ�
float calculateRotationAngle(const glm::vec3& lineStart, const glm::vec3& lineEnd) {
	glm::vec2 direction = lineEnd - lineStart;
	return -std::atan2(direction.y, direction.x);
}
// ������ ȸ����Ű�� �Լ�
glm::vec3 rotatePoint(const glm::vec3& point, const glm::vec3& origin, float angle) {
	glm::vec2 translatedPoint = glm::vec2(point.x - origin.x, point.y - origin.y);
	glm::vec2 rotatedPoint = glm::vec2(
		translatedPoint.x * cos(angle) - translatedPoint.y * sin(angle),
		translatedPoint.x * sin(angle) + translatedPoint.y * cos(angle)
	);
	return glm::vec3(rotatedPoint.x + origin.x, rotatedPoint.y + origin.y, 0.0f);
}
// �ٰ����� �ڸ��� �Լ�
std::pair<std::vector<glm::vec3>, std::vector<glm::vec3>> splitPolygonByLine(
	const std::vector<glm::vec3>& polygonVertices, const glm::vec3& lineStart, const glm::vec3& lineEnd) {

	float angle = calculateRotationAngle(lineStart, lineEnd);

	// ȸ���� ������ ���� ����
	std::vector<glm::vec3> rotatedPolygonVertices;
	glm::vec3 rotatedLineStart = rotatePoint(lineStart, lineStart, angle);
	glm::vec3 rotatedLineEnd = rotatePoint(lineEnd, lineStart, angle);

	for (const auto& vertex : polygonVertices) {
		rotatedPolygonVertices.push_back(rotatePoint(vertex, lineStart, angle));
	}

	// y���� �������� �и�
	std::vector<glm::vec3> upperPolygon, lowerPolygon;
	for (const auto& vertex : rotatedPolygonVertices) {
		if (vertex.y > rotatedLineStart.y) {
			upperPolygon.push_back(vertex);
		}
		else {
			lowerPolygon.push_back(vertex);
		}
	}

	// �� �ٰ����� ���� ȸ������ ����
	for (auto& vertex : upperPolygon) {
		vertex = rotatePoint(vertex, lineStart, -angle);
	}
	for (auto& vertex : lowerPolygon) {
		vertex = rotatePoint(vertex, lineStart, -angle);
	}

	return { upperPolygon, lowerPolygon };
}


// �߽����� ����ϴ� �Լ�
glm::vec3 calculateCentroid(const std::vector<glm::vec3>& vertices) {
	glm::vec3 centroid(0.0f, 0.0f, 0.0f);
	for (const auto& vertex : vertices) {
		centroid += vertex;
	}
	centroid /= static_cast<float>(vertices.size());
	return centroid;
}
// ������ �ݽð�������� �����ϴ� �Լ�
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

	glm::vec3 location{};	// ��ġ(translate ����)
	glm::vec3 color{};		// ����

	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };  // ũ�� (����Ʈ�� 1)
	float rotationAngle = 0.0f;  // ȸ�� ���� (����)

	std::vector<glm::vec3> object_vertices;
	std::vector<glm::vec3> orbit_vertices;

	void initialize_Object(glm::vec3 pos) {
		object_vertices.clear();
		location = pos;
		scale = { 0.2f, 0.2f, 0.2f };
		make_Color_random();
	}

	void make_Line(float x1, float y1, float x2, float y2) {
		object_vertices.clear();  // ���� ���� �ʱ�ȭ
		object_vertices.push_back(glm::vec3(x1, y1, 0.0f));  // ù ��° ����
		object_vertices.push_back(glm::vec3(x2, y2, 0.0f));   // �� ��° ����
	}
	// ������ �� ���� ���� �ٰ����� ����� �Լ�
	void make_Polygon(int sides) {
		object_vertices.clear();
		object_vertices.reserve(8);

		float radius = 0.6f;  // ������ ������ (ũ��)
		float angleIncrement = 2.0f * 3.14159265358979323846f / sides;  // �� ���� ������ ����

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
		rotationAngle = glm::radians(angle);  // �Է��� ����, ���ο����� ���� ���
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

	// ������ ��ȯ�� �����ؼ� ������ ���� ��ǥ�� ���ϴ� �Լ�
	std::vector<glm::vec3> calculateTransformedVertices() {
		std::vector<glm::vec3> real_object_vertices;

		// ��ġ �� ũ�� ��ȯ ����� ����
		glm::mat4 transform_Matrix = glm::mat4(1.0f);
		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);

		// ��ȯ ����� �� ������ ����
		for (const auto& v : object_vertices) {
			glm::vec4 transformed_vertex = transform_Matrix * glm::vec4(v, 1.0f); // vec3�� vec4�� Ȯ��
			real_object_vertices.push_back(glm::vec3(transformed_vertex)); // vec3�� �ٽ� ��ȯ�Ͽ� ����
		}

		return real_object_vertices;
	}
	// ���� ��ǥ�� ������� �������� �������ؼ� ����ó�� ����� �Լ�
	std::vector<glm::vec3> calculateOriginalVertices(const std::vector<glm::vec3>& transformedVertices) {
		std::vector<glm::vec3> original_object_vertices;

		// ��ġ �� ũ�� ��ȯ ����� ������� ����
		glm::mat4 transform_Matrix = glm::mat4(1.0f);
		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		transform_Matrix = glm::scale(transform_Matrix, scale);
		glm::mat4 inverse_transform_Matrix = glm::inverse(transform_Matrix);

		// ������� �� ��ȯ�� ������ �����Ͽ� ���� ��ǥ�� ����
		for (const auto& v : transformedVertices) {
			glm::vec4 original_vertex = inverse_transform_Matrix * glm::vec4(v, 1.0f); // vec3�� vec4�� Ȯ��
			original_object_vertices.push_back(glm::vec3(original_vertex)); // vec3�� �ٽ� ��ȯ�Ͽ� ����
		}
		return original_object_vertices;
	}

	void Draw_object(int num_vertices) {
		// VAO, VBO ����
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

		// ������ �׸��� ���� �ʿ��� ���̴��� ��� ����
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		// ���� �׸���
		glBindVertexArray(objectVAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, num_vertices);  // ������ ǥ���ϰ� �ʹٸ� GL_POINTS�� ����
		glBindVertexArray(0);

		// �޸� ����
		glDeleteBuffers(1, &objectVBO);
		glDeleteVertexArrays(1, &objectVAO);
	}
	void Draw_line() {
		// VAO, VBO ����
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

		// ������ �׸��� ���� �ʿ��� ���̴��� ��� ����
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		// ���� �׸���
		glBindVertexArray(objectVAO);
		glDrawArrays(GL_LINES, 0, 2);  // ������ ǥ���ϰ� �ʹٸ� GL_POINTS�� ����
		glBindVertexArray(0);

		// �޸� ����
		glDeleteVertexArrays(1, &objectVAO);
		glDeleteBuffers(1, &objectVBO);
	}

	void Draw_orbit() {
		// VAO, VBO ����
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

		// ��δ� ��� ������ �ʿ� ����. �̹� ��ȯ�� location ���� ����ְ� ����.
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// ������ �׸��� ���� �ʿ��� ���̴��� ��� ����
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		// ���� �׸���
		glBindVertexArray(orbitVAO);
		glDrawArrays(GL_LINES, 0, orbit_vertices.size());  // ������ ǥ���ϰ� �ʹٸ� GL_POINTS�� ����
		glBindVertexArray(0);

		// �޸� ����
		glDeleteVertexArrays(1, &orbitVAO);
		glDeleteBuffers(1, &orbitVBO);
	}
	void Draw_effect(int num_vertices) {
		// VAO, VBO ����
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

		// ������ �׸��� ���� �ʿ��� ���̴��� ��� ����
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		// ���� �׸���
		glBindVertexArray(objectVAO);
		glDrawArrays(GL_LINE_LOOP, 0, num_vertices);  // ������ ǥ���ϰ� �ʹٸ� GL_POINTS�� ����
		glBindVertexArray(0);

		// �޸� ����
		glDeleteBuffers(1, &objectVBO);
		glDeleteVertexArrays(1, &objectVAO);
	}
};


vector<Object> object{};				// �����Ǵ� ������Ʈ��
Object line{};							// �ڸ��� ��
Object basket{};						// �ٱ��� ��
vector<Object> effect{};				// ����Ʈ 

float gravity = -0.00003f;		// �߷� ���ӵ�
float poly_speed = 0.01f;		// �������� �ӵ� - x��

int made_polygon{ 0 };				// ������ �������� ��
bool timer1{ true };				// Ÿ�̸� �÷���
bool timer2{ true };				// Ÿ�̸� �÷���
bool draw_only_line{ false };		// draw ���� �÷���
bool draw_orbit{ false };			// ��� ������� �÷���

float line_x1{};					// ���� ����, �� ��
float line_y1{};					// ���� ����, �� ��
float line_x2{};					// ���� ����, �� ��
float line_y2{};					// ���� ����, �� ��



// ������ �����ϴ� ����
vector<glm::vec3> intersections{};
// ���Ϸ� ������ ������ ��ü ������ ������ ����
std::pair<std::vector<glm::vec3>, std::vector<glm::vec3>> new_polygons{};


// ������ �ٱ����� �浹�� üũ�ϴ� �Լ�
void basket_collision_with_object(Object& o);


int main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	srand(static_cast<int>(time(NULL)));

	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Polygon Slice!");
	cout << "������ ������" << endl;

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized" << std::endl;


	SP.make_vertexShaders(); //--- ���ؽ� ���̴� �����
	SP.make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
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

//--- �׸��� �ݹ� �Լ�
GLvoid drawScene()
{
	//--- ����� ���� ����
	glClearColor(0.8235f, 0.8235f, 0.8235f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- ������ ���������ο� ���̴� �ҷ�����
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


	glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
}
//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void TimerFunction(int value) {
	// �̵� Ÿ�̸�
	if (value == 1) {
		if (timer1) {
			for (auto& o : object) {
				// �ٱ��Ͽ� ������ �浹üũ
				basket_collision_with_object(o);

				// ������Ʈ �̵�
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

					// ���� �ӵ��� �߷� ���ӵ� ����
					o.vertical_speed += gravity;

					o.rotationAngle += 0.005f;
					
					// ��� �׸��� (�߸��� �ʾ��� ����)
					if (draw_orbit) o.orbit_vertices.push_back(o.location);
				}

				o.location.x += o.dir * o.speed;
			}

			// �ٱ��� �̵�
			basket.location.x += basket.dir * basket.speed;
			basket_boundary_check();

			// ������ ȭ�� ���� �Ѿ�� ����
			for (auto it = object.begin(); it != object.end();) {
				if (it->location.x < -1.5f || it->location.x > 1.5f || it->location.y < -1.5f) {
					it = object.erase(it); 
				}
				else {
					++it; 
				}
			}

			// ����Ʈ ����
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
	// ���� ���� Ÿ�̸�
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

			// ���� ó�� ����
			vector<glm::vec3> real_object_vertices = o.calculateTransformedVertices();
			intersections = getPolygonLineIntersections(real_object_vertices, line.object_vertices.front(), line.object_vertices.back());
			auto new_polygons = splitPolygonByLine(real_object_vertices, line.object_vertices.front(), line.object_vertices.back());

			// ������ �ϳ��� ���� ��� ���� �������� �Ѿ
			if (intersections.size() == 1) {
				continue;
			}

			if (!intersections.empty()) {
				// �� ���� ó��
				Object temp_object_upper;
				temp_object_upper.object_vertices.clear();
				for (auto& v : new_polygons.first) temp_object_upper.object_vertices.push_back(v);
				for (auto& i : intersections) temp_object_upper.object_vertices.push_back(i);
				sortVerticesCounterClockwise(temp_object_upper.object_vertices);

				o.object_vertices = temp_object_upper.object_vertices;
				o.object_vertices = o.calculateOriginalVertices(o.object_vertices);
				o.dir = RIGHT;
				o.sliced_check = true;

				// �Ʒ� ���� ó��
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

				// ���� �߰��� ������ new_objects ���Ϳ� ����
				new_objects.push_back(temp_object_lower);
				// ����Ʈ�� ����� ������ ����
				for (int i = 0; i < 6; ++i) make_effect(o.location.x, o.location.y);
			}
		}

		// ���� ������ lower �������� ���� object ���Ϳ� �ִ´�!
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
	gravity = -0.00003f;		// �߷� ���ӵ�
	poly_speed = 0.01f;		// �������� �ӵ� - x��
	
	made_polygon = 0;
	timer1 = true;
	timer2 = true;
	draw_only_line = false;		// draw ���� �÷���
	draw_orbit = false ;			// ��� ������� �÷���

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
	// y ���� ���� ���� �� �Ǻ�
	for (auto& v : real_object_vertices) {
		if (y_min.y > v.y) y_min = v;
	}

	// �浹üũ
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