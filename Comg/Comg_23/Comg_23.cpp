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
#define CHILD 1


//--- �Լ� ���� �߰��ϱ�
std::pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WINDOW_WIDTH / 2)) / (WINDOW_WIDTH / 2); //gl��ǥ��� ����
	float my = -((float)y - (WINDOW_HEIGHT / 2)) / (WINDOW_HEIGHT / 2); //gl��ǥ��� ����
	return { mx, my };
}

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

void TimerFunction(int value);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
void KeyboardUp(unsigned char key, int x, int y);
void SpecialKeyboard(int key, int x, int y);

float distanceFromOrigin(float x, float y) {
	return std::sqrt(x * x + y * y);  // �Ǵ� std::sqrt(std::pow(x, 2) + std::pow(y, 2));
}
float randomValue() {
	return -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / 2.0f);
}

void initialize();
bool SATCheck(const std::vector<glm::vec3>& vertices1, const std::vector<glm::vec3>& vertices2);
bool collision_check();


Importer importer;
ShaderProgram SP;


class Camera {
public:
	glm::vec3 cameraPos = glm::vec3(0.0f, -1.0f, 20.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float rotationAngle = 0.0f;

	float fov = 20.0f;
	float aspectRatio = 800.0f / 800.0f;
	float nearClip = 0.1f;
	float farClip = 100.0f;

	void initialize() {
		cameraPos = glm::vec3(0.0f, -1.0f, 20.0f);
		cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		rotationAngle = 0.0f;

		fov = 20.0f;
		aspectRatio = 800.0f / 800.0f;
		nearClip = 0.1f;
		farClip = 100.0f;
	}

	void setRotation(float angle) {
		rotationAngle = glm::radians(angle);  // �Է��� ����, ���ο����� ���� ���
	}

	void make_camera_perspective() {
		// ī�޶��� ��ġ�� ȸ����Ű�� ����, ȸ�� ����� ����
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 rotatedCameraPos = rotationMatrix * glm::vec4(cameraPos, 1.0f); // ȸ���� ī�޶� ��ġ

		// ȸ���� ī�޶� ��ġ�� ��ǥ ������ ����Ͽ� LookAt ��� ���
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

		// ī�޶��� ��ġ�� ȸ����Ű�� ����, ȸ�� ����� ����
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 rotatedCameraPos = rotationMatrix * glm::vec4(cameraPos, 1.0f); // ȸ���� ī�޶� ��ġ

		// ȸ���� ī�޶� ��ġ�� ��ǥ ������ ����Ͽ� LookAt ��� ���
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

		// ī�޶��� ��ġ�� ȸ����Ű�� ����, ȸ�� ����� ����
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 rotatedCameraPos = rotationMatrix * glm::vec4(cameraPos, 1.0f); // ȸ���� ī�޶� ��ġ

		// ȸ���� ī�޶� ��ġ�� ��ǥ ������ ����Ͽ� LookAt ��� ���
		glm::mat4 view = glm::lookAt(glm::vec3(rotatedCameraPos), cameraTarget, cameraUp);
		glUniformMatrix4fv(glGetUniformLocation(SP.shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 projection = glm::ortho(left, right, bottom, top, nearClip, farClip);
		glUniformMatrix4fv(glGetUniformLocation(SP.shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	}
};

class Object {
public:
	int what{};
	int direction{ 1 };
	bool available{ false };
	bool bridge{ false };
	bool jump{ false };

	GLuint objVAO{};
	string objName{};

	glm::vec3 location{};	// ��ġ(translate ����)
	glm::vec3 color{};		// ����

	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };  // ũ�� (����Ʈ�� 1)
	float rotationAngle = 0.0f;  // ȸ�� ���� (����)
	float first_rotationAngle = 0.0f;  // ȸ�� ���� �ʱⰪ (����)

	glm::vec3 parent_location{};
	float parent_rotationAngle = 0.0f;
	glm::vec3 parent_scale{ 1.0f, 1.0f, 1.0f };

	glm::vec3 new_location{};

	std::vector<glm::vec3> object_vertices;

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
				scale = glm::vec3(0.5f, 0.5f, 0.5f);  // �⺻ ũ�� ����
				rotationAngle = 0.0f;  // �⺻ ȸ�� ���� ����

				parent_location = glm::vec3(0.0f, 0.0f, 0.0f);	//�θ������ ����

				object_vertices = v->vertex;
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

	void make_Cube_Polygon() {
		object_vertices.clear();

		// �ϴ� �� (�� �ﰢ��)
		object_vertices.push_back(glm::vec3(0.5f, -0.5f, -0.5f));  // v1
		object_vertices.push_back(glm::vec3(0.5f, -0.5f, 0.5f));   // v2
		object_vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));  // v3

		object_vertices.push_back(glm::vec3(0.5f, -0.5f, -0.5f));  // v1
		object_vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));  // v3
		object_vertices.push_back(glm::vec3(-0.5f, -0.5f, -0.5f)); // v4

		// ��� �� (�� �ﰢ��)
		object_vertices.push_back(glm::vec3(0.5f, 0.5f, -0.5f));   // v5
		object_vertices.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));  // v8
		object_vertices.push_back(glm::vec3(0.5f, 0.5f, 0.5f));    // v6

		object_vertices.push_back(glm::vec3(0.5f, 0.5f, 0.5f));    // v6
		object_vertices.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));  // v8
		object_vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));   // v7

		// �ո� (�� �ﰢ��)
		object_vertices.push_back(glm::vec3(0.5f, -0.5f, 0.5f));   // v2
		object_vertices.push_back(glm::vec3(0.5f, 0.5f, 0.5f));    // v6
		object_vertices.push_back(glm::vec3(0.5f, 0.5f, -0.5f));   // v5

		object_vertices.push_back(glm::vec3(0.5f, -0.5f, 0.5f));   // v2
		object_vertices.push_back(glm::vec3(0.5f, -0.5f, -0.5f));  // v1
		object_vertices.push_back(glm::vec3(0.5f, 0.5f, -0.5f));   // v5

		// �޸� (�� �ﰢ��)
		object_vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));  // v3
		object_vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));   // v7
		object_vertices.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));  // v8

		object_vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));  // v3
		object_vertices.push_back(glm::vec3(-0.5f, -0.5f, -0.5f)); // v4
		object_vertices.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));  // v8

		// ���� �� (�� �ﰢ��)
		object_vertices.push_back(glm::vec3(0.5f, -0.5f, -0.5f));  // v1
		object_vertices.push_back(glm::vec3(0.5f, 0.5f, -0.5f));   // v5
		object_vertices.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));  // v8

		object_vertices.push_back(glm::vec3(0.5f, -0.5f, -0.5f));  // v1
		object_vertices.push_back(glm::vec3(-0.5f, -0.5f, -0.5f)); // v4
		object_vertices.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));  // v8

		// ������ �� (�� �ﰢ��)
		object_vertices.push_back(glm::vec3(0.5f, -0.5f, 0.5f));   // v2
		object_vertices.push_back(glm::vec3(0.5f, 0.5f, 0.5f));    // v6
		object_vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));   // v7

		object_vertices.push_back(glm::vec3(0.5f, -0.5f, 0.5f));   // v2
		object_vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));  // v3
		object_vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));   // v7
	}
	void make_Square_Polygon() {
		object_vertices.clear();

		object_vertices.push_back(glm::vec3(0.5f, 1.0f, 0.0f));
		object_vertices.push_back(glm::vec3(-0.5f, 1.0f, 0.0f));
		object_vertices.push_back(glm::vec3(-0.5f, 0.0f, 0.0f));
		object_vertices.push_back(glm::vec3(0.5f, 0.0f, 0.0f));
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
		first_rotationAngle = glm::radians(angle);  // �Է��� ����, ���ο����� ���� ���
	}
	void setRotation(float angle) {
		rotationAngle = glm::radians(angle);  // �Է��� ����, ���ο����� ���� ���
	}
	float getRotation() {
		return glm::degrees(rotationAngle);  // ������ ���� ���� ������ ��ȯ�Ͽ� ��ȯ
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


	// ������ ��ȯ�� �����ؼ� ������ ���� ��ǥ�� ���ϴ� �Լ�
	std::vector<glm::vec3> calculateTransformedVertices() {
		std::vector<glm::vec3> real_object_vertices;
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		// ��ġ �� ũ�� ��ȯ ����� ����
		if (what == MAIN) {
			transform_Matrix = glm::translate(transform_Matrix, location);
			transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
			transform_Matrix = glm::scale(transform_Matrix, scale);
		}
		else if (what == CHILD) {
			transform_Matrix = glm::translate(transform_Matrix, parent_location);
			transform_Matrix = glm::rotate(transform_Matrix, parent_rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
			transform_Matrix = glm::scale(transform_Matrix, parent_scale);

			transform_Matrix = glm::translate(transform_Matrix, location);
			transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));
			transform_Matrix = glm::scale(transform_Matrix, scale);
		}

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

	void Draw_parent_cube() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);
		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// �̵�
		transform_Matrix = glm::translate(transform_Matrix, location);
		// ����
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		// ���Ȯ��
		transform_Matrix = glm::scale(transform_Matrix, scale);

		ObjectTransform = glGetUniformLocation(SP.shaderID, "parent_transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		glm::vec4 transformedLocation = transform_Matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		new_location = glm::vec3(transformedLocation);

		// color�� ���̴��� ����
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
	void Draw_child_cube() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);
		// �̵�
		transform_Matrix = glm::translate(transform_Matrix, location);
		// ����
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));
		// ���Ȯ��
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));


		transform_Matrix = glm::mat4(1.0f);
		// �̵�
		transform_Matrix = glm::translate(transform_Matrix, parent_location);
		// ����
		transform_Matrix = glm::rotate(transform_Matrix, parent_rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		// ���Ȯ��
		transform_Matrix = glm::scale(transform_Matrix, parent_scale);

		ObjectTransform = glGetUniformLocation(SP.shaderID, "parent_transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		glm::vec4 transformedLocation = transform_Matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		new_location = glm::vec3(transformedLocation);

		// color�� ���̴��� ����
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	void Draw_stage_cube() {
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
		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// �̵�
		transform_Matrix = glm::translate(transform_Matrix, location);
		// ����
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));
		// ���Ȯ��
		transform_Matrix = glm::scale(transform_Matrix, scale);

		ObjectTransform = glGetUniformLocation(SP.shaderID, "parent_transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// ������ �׸��� ���� �ʿ��� ���̴��� ��� ����
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objectVAO);
		// ���� �׸���
		for (int i = 0; i < 12; ++i) {
			if (i == 4) glBindVertexArray(0);
			glDrawArrays(GL_TRIANGLES, i * 3, (i + 1) * 3);
		}
		glBindVertexArray(0);

		// �޸� ����
		glDeleteBuffers(1, &objectVBO);
		glDeleteVertexArrays(1, &objectVAO);
	}
	void Draw_stage_square() {
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
		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// �̵�
		transform_Matrix = glm::translate(transform_Matrix, location);
		// ����
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));
		// ���Ȯ��
		transform_Matrix = glm::scale(transform_Matrix, scale);

		ObjectTransform = glGetUniformLocation(SP.shaderID, "parent_transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// ������ �׸��� ���� �ʿ��� ���̴��� ��� ����
		unsigned int ObjectColor = glGetUniformLocation(SP.shaderID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objectVAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);

		// �޸� ����
		glDeleteBuffers(1, &objectVBO);
		glDeleteVertexArrays(1, &objectVAO);
	}

	void Draw_line() {
		// ���� �׸� �� ��ȯ�� �������� ����
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		unsigned int ObjectTransform = glGetUniformLocation(SP.shaderID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));
		ObjectTransform = glGetUniformLocation(SP.shaderID, "parent_transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color�� ���̴��� ����
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

vector<Object> robot;
vector<Object> object;

Object front_stage;
Object back_stage;
vector<Object> bottom_stage;
Object top_stage;

bool timer{ true };

bool draw_only_line{ false };
bool draw_enable{ true };

bool MOVE_CHECK{ false };
bool MOVE_W{ false };
bool MOVE_A{ false };
bool MOVE_S{ false };
bool MOVE_D{ false };
bool MOVE_J{ false };

bool CHECK_A{ false };
bool CHECK_O{ false };

int radian{ 0 };
int stageradian{ 0 };
int direction{ 1 };
float move_speed{ 0.03f };
float move_radian{ 60.0f };

float now_x{};
float now_y{};

int main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	srand(static_cast<int>(time(NULL)));

	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Example23");
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

	importer.ReadObj();
	for (auto& v : importer.VertexBuffers) {
		cout << "Filename: " << v->filename << ", VAO: " << v->VAO << ", Vertices: " << v->vertex.size() << ", Colors: " << v->color.size() << endl;
	}

	SP.make_vertexShaders(); //--- ���ؽ� ���̴� �����
	SP.make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	SP.make_shaderProgram();

	initialize();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);

	glutTimerFunc(17, TimerFunction, 1);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutSpecialFunc(SpecialKeyboard);

	glutMainLoop();
}

//--- �׸��� �ݹ� �Լ�
GLvoid drawScene()
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ���������׸���
	glViewport(0, 0, 800, 800);
	camera.make_camera_perspective();

	//--- ������ ���������ο� ���̴� �ҷ�����
	glUseProgram(SP.shaderID);

	xyz_line[0].Draw_line();
	xyz_line[1].Draw_line();
	xyz_line[2].Draw_line();

	// ��������, ���� ����
	if (draw_enable) {
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}
	else if (!draw_enable) glDisable(GL_CULL_FACE);


	if (draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (!draw_only_line) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//if(!stage.object_vertices.empty()) stage.Draw_stage_cube();
	front_stage.Draw_stage_square();
	back_stage.Draw_stage_square();
	for(auto& s : bottom_stage) s.Draw_stage_square();
	top_stage.Draw_stage_square();

	// ť�� �׸��� (��ȯ ����)
	for (auto& r : robot) {
		if (r.what == MAIN) r.Draw_parent_cube();
		else if (r.what == CHILD) {
			r.setParent(robot[0]);
			r.Draw_child_cube();
		}
	}

	for (auto& o : object) {
		o.Draw_parent_cube();
	}


	glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
}
//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h)
{

}

void TimerFunction(int value) {
	if (value == 1) {
		if (timer) {
			if (MOVE_CHECK) {
				for (int i = 3; i <= 6; ++i) {
					float radian{ robot[i].getRotation() };

					if (radian >= 180.0f + move_radian || radian <= 180.0f - move_radian) robot[i].direction *= -1;

					if (robot[i].direction == 1) ++radian;
					else if (robot[i].direction == -1) --radian;

					robot[i].setRotation(radian);
				}

				if (MOVE_W) {
					glm::mat4 transform_Matrix = glm::mat4(1.0f);

					transform_Matrix = glm::rotate(transform_Matrix, robot[0].rotationAngle, glm::vec3(0, 1, 0));

					glm::vec3 frontVector = glm::vec3(0.0f, 0.0f, 1.0f);
					glm::vec3 rotatedFrontVector = glm::vec3(transform_Matrix * glm::vec4(frontVector, 1.0f));

					robot[0].location += move_speed * rotatedFrontVector;
					if (collision_check()) robot[0].location -= move_speed * rotatedFrontVector;
				}
				if (MOVE_A) {
					float radian{ robot[0].getRotation() };
					++radian;
					robot[0].setRotation(radian);
				}
				if (MOVE_S) {
					glm::mat4 transform_Matrix = glm::mat4(1.0f);

					transform_Matrix = glm::rotate(transform_Matrix, robot[0].rotationAngle, glm::vec3(0, 1, 0));

					glm::vec3 frontVector = glm::vec3(0.0f, 0.0f, -1.0f);
					glm::vec3 rotatedFrontVector = glm::vec3(transform_Matrix * glm::vec4(frontVector, 1.0f));

					robot[0].location += move_speed * rotatedFrontVector;
					if (collision_check()) robot[0].location -= move_speed * rotatedFrontVector;
				}
				if (MOVE_D) {
					float radian{ robot[0].getRotation() };
					--radian;
					robot[0].setRotation(radian);
				}
			}

			if (MOVE_J) {
				if (robot[0].direction == 1) {
					for (auto& r : robot) r.jump = true;

					if (collision_check()) {
						for (auto& r : robot) r.jump = false;
						robot[0].direction = 1;
						MOVE_J = false;
					}
					else robot[0].location.y += 0.06f;

					if (robot[0].location.y > -0.8f) {
						robot[0].direction = -1;
					}
				}
				else if (robot[0].direction == -1) {
					if (collision_check()) {
						for (auto& r : robot) r.jump = false;
						robot[0].direction = 1;
						MOVE_J = false;
					}
					else robot[0].location.y -= 0.06f;

					if (robot[0].location.y < -2.1f) {
						for (auto& r : robot) r.jump = false;
						robot[0].direction = 1;
						MOVE_J = false;
					}
				}
			}

			if (CHECK_O) {
				if (front_stage.direction == 1) {
					++stageradian;
					if (stageradian > 90.0f) {
						front_stage.direction = -1;
						CHECK_O = false;
					}
				}
				else if (front_stage.direction == -1) {
					--stageradian;
					if (stageradian < 0.0f) {
						front_stage.direction = 1;
						CHECK_O = false;
					}
				}

				front_stage.setRotation(stageradian);
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

	else if (key == '2') {
		draw_only_line = true;
	}
	else if (key == '@') {
		draw_only_line = false;
	}


	else if (key == 'r') {
		direction *= -1;
	}


	else if (key == 'w') {
		MOVE_CHECK = true;
		MOVE_W = true;
	}
	else if (key == 'a') {
		MOVE_CHECK = true;
		MOVE_A = true;
	}
	else if (key == 's') {
		MOVE_CHECK = true;
		MOVE_S = true;
	}
	else if (key == 'd') {
		MOVE_CHECK = true;
		MOVE_D = true;
	}
	else if (key == 'j') {
		MOVE_CHECK = true;
		MOVE_J = true;
	}


	else if (key == 'o') {
		CHECK_O = true;
	}
	else if (key == '+') {
		if (move_speed >= 0.04f) return;
		move_radian += 30.0f;
		move_speed += 0.01f;
		cout << "speed level: " << (move_speed - 0.01f) * 100 << endl;
	}
	else if (key == '-') {
		if (move_speed <= 0.02f) return;
		move_radian -= 30.0f;
		move_speed -= 0.01f;
		cout << "speed level: " << (move_speed - 0.01f) * 100 << endl;
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
	else if (key == 'y' || key == 'Y') {
		CHECK_A = true;
	}


	glutPostRedisplay();
}

void KeyboardUp(unsigned char key, int x, int y) {
	if (key == 'w') {
		MOVE_W = false;
	}
	else if (key == 'a') {
		MOVE_A = false;
	}
	else if (key == 's') {
		MOVE_S = false;
	}
	else if (key == 'd') {
		MOVE_D = false;
	}

	if (!MOVE_W && !MOVE_A && !MOVE_S && !MOVE_D) MOVE_CHECK = false;

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
	robot.clear();

	xyz_line[0].initialize_Object("x_Line", { 0.0f, 0.0f, 0.0f });
	xyz_line[0].setColor({ 1.0f, 0.0f, 0.0f });
	xyz_line[1].initialize_Object("y_Line", { 0.0f, 0.0f, 0.0f });
	xyz_line[1].setColor({ 0.0f, 1.0f, 0.0f });
	xyz_line[2].initialize_Object("z_Line", { 0.0f, 0.0f, 0.0f });
	xyz_line[2].setColor({ 0.0f, 0.0f, 1.0f });

	front_stage.make_Square_Polygon();
	front_stage.location = { 0.0f, -2.5f, 2.5f };
	front_stage.setScale({ 5.0f, 5.0f, 5.0f });
	front_stage.make_Color_random();

	back_stage.make_Square_Polygon();
	back_stage.location = { 0.0f, -2.5f, -2.5f };
	back_stage.setScale({ 5.0f, 5.0f, 5.0f });
	back_stage.make_Color_random();

	top_stage.make_Square_Polygon();
	top_stage.location = { 0.0f, 2.5f, -2.5f };
	top_stage.setScale({ 5.0f, 5.0f, 5.0f });
	top_stage.setRotation(90);
	top_stage.make_Color_random();


	Object temp_obj;

	// �ٴ�Ÿ��
	temp_obj.make_Square_Polygon();
	temp_obj.location = { 0.0f, -2.501f, -2.5f };
	temp_obj.setScale({ 5.0f, 5.0f, 5.0f });
	temp_obj.setRotation(90);
	temp_obj.make_Color_random();
	bottom_stage.push_back(temp_obj);
	 
	temp_obj.make_Square_Polygon();
	temp_obj.location = { -2.0f, -2.5f, -2.5f };
	temp_obj.setScale({ 1.0f, 1.0f, 1.0f });
	temp_obj.setRotation(90);
	temp_obj.color = { 1.0f, 0.0f, 0.0f };
	bottom_stage.push_back(temp_obj);

	temp_obj.make_Square_Polygon();
	temp_obj.location = { 0.0f, -2.5f, -2.5f };
	temp_obj.setScale({ 1.0f, 1.0f, 1.0f });
	temp_obj.setRotation(90);
	temp_obj.color = { 1.0f, 0.0f, 0.0f };
	bottom_stage.push_back(temp_obj);

	temp_obj.make_Square_Polygon();
	temp_obj.location = { 2.0f, -2.5f, -2.5f };
	temp_obj.setScale({ 1.0f, 1.0f, 1.0f });
	temp_obj.setRotation(90);
	temp_obj.color = { 1.0f, 0.0f, 0.0f };
	bottom_stage.push_back(temp_obj);

	temp_obj.make_Square_Polygon();
	temp_obj.location = { -1.0f, -2.5f, -1.5f };
	temp_obj.setScale({ 1.0f, 1.0f, 1.0f });
	temp_obj.setRotation(90);
	temp_obj.color = { 1.0f, 0.0f, 0.0f };
	bottom_stage.push_back(temp_obj);

	temp_obj.make_Square_Polygon();
	temp_obj.location = { 1.0f, -2.5f, -1.5f };
	temp_obj.setScale({ 1.0f, 1.0f, 1.0f });
	temp_obj.setRotation(90);
	temp_obj.color = { 1.0f, 0.0f, 0.0f };
	bottom_stage.push_back(temp_obj);

	temp_obj.make_Square_Polygon();
	temp_obj.location = { -2.0f, -2.5f, -0.5f };
	temp_obj.setScale({ 1.0f, 1.0f, 1.0f });
	temp_obj.setRotation(90);
	temp_obj.color = { 1.0f, 0.0f, 0.0f };
	bottom_stage.push_back(temp_obj);

	temp_obj.make_Square_Polygon();
	temp_obj.location = { 0.0f, -2.5f, -0.5f };
	temp_obj.setScale({ 1.0f, 1.0f, 1.0f });
	temp_obj.setRotation(90);
	temp_obj.color = { 1.0f, 0.0f, 0.0f };
	bottom_stage.push_back(temp_obj);

	temp_obj.make_Square_Polygon();
	temp_obj.location = { 2.0f, -2.5f, -0.5f };
	temp_obj.setScale({ 1.0f, 1.0f, 1.0f });
	temp_obj.setRotation(90);
	temp_obj.color = { 1.0f, 0.0f, 0.0f };
	bottom_stage.push_back(temp_obj);

	temp_obj.make_Square_Polygon();
	temp_obj.location = { -1.0f, -2.5f, 0.5f };
	temp_obj.setScale({ 1.0f, 1.0f, 1.0f });
	temp_obj.setRotation(90);
	temp_obj.color = { 1.0f, 0.0f, 0.0f };
	bottom_stage.push_back(temp_obj);

	temp_obj.make_Square_Polygon();
	temp_obj.location = { 1.0f, -2.5f, 0.5f };
	temp_obj.setScale({ 1.0f, 1.0f, 1.0f });
	temp_obj.setRotation(90);
	temp_obj.color = { 1.0f, 0.0f, 0.0f };
	bottom_stage.push_back(temp_obj);

	temp_obj.make_Square_Polygon();
	temp_obj.location = { -2.0f, -2.5f, 1.5f };
	temp_obj.setScale({ 1.0f, 1.0f, 1.0f });
	temp_obj.setRotation(90);
	temp_obj.color = { 1.0f, 0.0f, 0.0f };
	bottom_stage.push_back(temp_obj);

	temp_obj.make_Square_Polygon();
	temp_obj.location = { 0.0f, -2.5f, 1.5f };
	temp_obj.setScale({ 1.0f, 1.0f, 1.0f });
	temp_obj.setRotation(90);
	temp_obj.color = { 1.0f, 0.0f, 0.0f };
	bottom_stage.push_back(temp_obj);

	temp_obj.make_Square_Polygon();
	temp_obj.location = { 2.0f, -2.5f, 1.5f };
	temp_obj.setScale({ 1.0f, 1.0f, 1.0f });
	temp_obj.setRotation(90);
	temp_obj.color = { 1.0f, 0.0f, 0.0f };
	bottom_stage.push_back(temp_obj);


	// ��ֹ� 2��
	temp_obj.initialize_Object("cube.obj", { 1.0f, -2.0f, 1.0f });
	temp_obj.setScale({ 0.5f, 0.5f, 0.5f });
	temp_obj.setRotation(90);
	temp_obj.what = MAIN;
	temp_obj.available = true;
	object.push_back(temp_obj);

	temp_obj.initialize_Object("cube.obj", { -2.0f, -2.0f, 1.0f });
	temp_obj.setScale({ 0.5f, 0.5f, 0.5f });
	temp_obj.setRotation(90);
	temp_obj.what = MAIN;
	temp_obj.available = true;
	object.push_back(temp_obj);

	// ��� ��ֹ�
	temp_obj.initialize_Object("cube.obj", { -1.0f, -2.0f, -1.0f });
	temp_obj.setScale({ 0.5f, 0.5f, 0.5f });
	temp_obj.setRotation(90);
	temp_obj.what = MAIN;
	temp_obj.bridge = true;
	object.push_back(temp_obj);

	temp_obj.initialize_Object("cube.obj", { -1.0f, -1.25f, -1.0f });
	temp_obj.setScale({ 0.5f, 0.5f, 0.5f });
	temp_obj.setRotation(90);
	temp_obj.what = MAIN;
	temp_obj.bridge = true;
	object.push_back(temp_obj);

	temp_obj.initialize_Object("cube.obj", { -1.0f, -0.5f, -1.0f });
	temp_obj.setScale({ 0.5f, 0.5f, 0.5f });
	temp_obj.setRotation(90);
	temp_obj.what = MAIN;
	temp_obj.bridge = true;
	object.push_back(temp_obj);

	temp_obj.initialize_Object("cube.obj", { 1.0f, -2.0f, -1.0f });
	temp_obj.setScale({ 0.5f, 0.5f, 0.5f });
	temp_obj.setRotation(90);
	temp_obj.what = MAIN;
	temp_obj.bridge = true;
	object.push_back(temp_obj);

	temp_obj.initialize_Object("cube.obj", { 1.0f, -1.25f, -1.0f });
	temp_obj.setScale({ 0.5f, 0.5f, 0.5f });
	temp_obj.setRotation(90);
	temp_obj.what = MAIN;
	temp_obj.bridge = true;
	object.push_back(temp_obj);

	temp_obj.initialize_Object("cube.obj", { 1.0f, -0.5f, -1.0f });
	temp_obj.setScale({ 0.5f, 0.5f, 0.5f });
	temp_obj.setRotation(90);
	temp_obj.what = MAIN;
	temp_obj.bridge = true;
	object.push_back(temp_obj);

	temp_obj.initialize_Object("cube.obj", { 0.0f, -0.5f, -1.0f });
	temp_obj.setScale({ 0.5f, 0.5f, 0.5f });
	temp_obj.setRotation(90);
	temp_obj.what = MAIN;
	temp_obj.bridge = true;
	object.push_back(temp_obj);


	// ���� - main
	temp_obj.initialize_Object("cube.obj", { 0.0f, -2.0f, 0.0f });
	temp_obj.setScale({ 0.2f, 0.2f, 0.2f });
	temp_obj.what = MAIN;
	robot.push_back(temp_obj);

	temp_obj.initialize_Object("cube.obj", { 0.0f, 1.0f, 0.0f });
	temp_obj.what = CHILD;
	robot.push_back(temp_obj);

	temp_obj.initialize_Object("cube.obj", { 0.0f, 0.9f, 0.4f });
	temp_obj.setScale({ 0.2f, 0.2f, 0.2f });
	temp_obj.what = CHILD;
	robot.push_back(temp_obj);

	// �ٸ� - 3 , 4
	temp_obj.initialize_Object("long_cube.obj", { 0.5f, -1.0f, 0.0f });
	temp_obj.setRotation(180);
	temp_obj.direction = 1;
	temp_obj.what = CHILD;
	robot.push_back(temp_obj);

	temp_obj.initialize_Object("long_cube.obj", { -0.5f, -1.0f, 0.0f });
	temp_obj.setRotation(180);
	temp_obj.direction = -1;
	temp_obj.what = CHILD;
	robot.push_back(temp_obj);

	// �� - 5 , 6
	temp_obj.initialize_Object("long_cube.obj", { 1.2f, -0.2f, 0.0f });
	temp_obj.setRotation(180);
	temp_obj.direction = -1;
	temp_obj.what = CHILD;
	robot.push_back(temp_obj);

	temp_obj.initialize_Object("long_cube.obj", { -1.2f, -0.2f, 0.0f });
	temp_obj.setRotation(180);
	temp_obj.direction = 1;
	temp_obj.what = CHILD;
	robot.push_back(temp_obj);

	cout << "initialize �Ϸ�" << endl;
}



bool SATCheck(const std::vector<glm::vec3>& vertices1, const std::vector<glm::vec3>& vertices2) {
	std::vector<glm::vec3> axes;

	// 1. ù ��° ��ü�� �� ��� �߰�
	for (size_t i = 0; i < vertices1.size(); i += 3) {
		glm::vec3 edge1 = vertices1[i + 1] - vertices1[i];
		glm::vec3 edge2 = vertices1[i + 2] - vertices1[i];
		glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
		axes.push_back(normal);
	}

	// 2. �� ��° ��ü�� �� ��� �߰�
	for (size_t i = 0; i < vertices2.size(); i += 3) {
		glm::vec3 edge1 = vertices2[i + 1] - vertices2[i];
		glm::vec3 edge2 = vertices2[i + 2] - vertices2[i];
		glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
		axes.push_back(normal);
	}

	// 3. ��ü�� �𼭸��� �����Ͽ� ������ �� �߰�
	for (size_t i = 0; i < vertices1.size(); i++) {
		for (size_t j = 0; j < vertices2.size(); j++) {
			glm::vec3 edge1 = vertices1[(i + 1) % vertices1.size()] - vertices1[i];
			glm::vec3 edge2 = vertices2[(j + 1) % vertices2.size()] - vertices2[j];
			glm::vec3 axis = glm::normalize(glm::cross(edge1, edge2));
			if (glm::length(axis) > 1e-6f) { // ��ȿ�� �ุ �߰�
				axes.push_back(axis);
			}
		}
	}

	// 4. �� �࿡ ���� ���� �˻�
	for (const auto& axis : axes) {
		float min1, max1, min2, max2;

		// ù ��° ��ü ���� ���� ���
		min1 = max1 = glm::dot(vertices1[0], axis);
		for (const auto& vertex : vertices1) {
			float projection = glm::dot(vertex, axis);
			min1 = glm::min(min1, projection);
			max1 = glm::max(max1, projection);
		}

		// �� ��° ��ü ���� ���� ���
		min2 = max2 = glm::dot(vertices2[0], axis);
		for (const auto& vertex : vertices2) {
			float projection = glm::dot(vertex, axis);
			min2 = glm::min(min2, projection);
			max2 = glm::max(max2, projection);
		}

		// ������ ������ ��ġ�� ������ �浹���� ����
		if (max1 < min2 || max2 < min1) {
			return false;
		}
	}

	// ��� �࿡�� �浹�� Ȯ�εǸ� �浹 �߻�
	return true;
}

bool collision_check() {
	for (auto& r : robot) {
		for (auto& o : object) {
			// ��ȯ�� ���� ���
			std::vector<glm::vec3> robot_real_object_vertices = r.calculateTransformedVertices();
			std::vector<glm::vec3> object_real_object_vertices = o.calculateTransformedVertices();

			// SAT �浹 ����
			if (SATCheck(robot_real_object_vertices, object_real_object_vertices)) {
				if (o.bridge) return true;
				if (o.available) {
					if (r.jump) {
						o.location.y -= 0.1f;
						if (o.location.y < -2.739f) {
							o.location.y = -2.739f;
							o.available = false;
							return true;
						}
					}
					return true; // �浹 �߻�
				}
			}
		}
	}
	return false; // �浹 ����
}
