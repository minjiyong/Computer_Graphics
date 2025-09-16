#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
//--- ���� �Լ�
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm.hpp>

#include <vector>
#include <random>
#include <utility>

using namespace std;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

//--- �Լ� ���� �߰��ϱ�
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading
	if (!fptr) // Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator
	return buf; // Return the buffer
}
std::pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WINDOW_WIDTH / 2)) / (WINDOW_WIDTH / 2); //gl��ǥ��� ����
	float my = -((float)y - (WINDOW_HEIGHT / 2)) / (WINDOW_HEIGHT / 2); //gl��ǥ��� ����
	return { mx, my };
}

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

void TimerFunction(int value);
void Motion(int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();


//--- �ʿ��� ���� ����
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader, fragmentShader; //--- ���̴� ��ü


class Point {
public:
	glm::vec3 m_vertex[1];
	glm::vec3 m_color[1];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	GLuint m_VAO;
	Point(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		cout << "Original window coordinates: (" << x << ", " << y << ")" << endl;
		cout << "Converted OpenGL coordinates: (" << center.first << ", " << center.second << ")" << endl;
		float r = (float)(rand() % 256) / 255.0f;
		float g = (float)(rand() % 256) / 255.0f;
		float b = (float)(rand() % 256) / 255.0f;
		m_color[0].r = r;
		m_color[0].g = g;
		m_color[0].b = b;
		m_vertex[0].x = center.first;
		m_vertex[0].y = center.second;
		m_vertex[0].z = 0.0f;
		SetVAO();
	}
	void SetVAO() {
		glGenVertexArrays(1, &m_VAO);//vao ��ü ����, params(����, GLuint*)//n���� �޾Ƶ� �����ǰ��Ϸ���
		glBindVertexArray(m_VAO);//���ε��ҰŶ�� �˷��ֱ�
		//vbo��ü ����
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertex), m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		if (positionAttrib == -1) {
			std::cerr << "ERROR: 'vPos' �Ӽ��� ã�� �� �����ϴ�." << std::endl;
			return; // ���� �߻� �� �Լ��� �����մϴ�.
		}
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);

		glGenBuffers(1, &m_VBOColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_color), m_color, GL_STATIC_DRAW);
		int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
		if (colorAttrib == -1) {
			std::cerr << "ERROR: 'vColor' �Ӽ��� ã�� �� �����ϴ�." << std::endl;
			return; // ���� �߻� �� �Լ��� �����մϴ�.
		}
		glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttrib);
	}
	void UpdateVAO() {
		// ���� VAO ���ε�
		glBindVertexArray(m_VAO);

		// ���� �����͸� ������Ʈ
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertex), m_vertex);

		// ���� �����͸� ������Ʈ
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_color), m_color);
	}

	void Draw() {
		glUseProgram(shaderProgramID);
		UpdateVAO();
		glPointSize(15.0);
		glDrawArrays(GL_POINTS, 0, 1);

	}
	void Move_up() {
		m_move.y += 0.01f;
	}
	void Move_down() {
		m_move.y -= 0.01f;
	}
	void Move_left() {
		m_move.x -= 0.01f;
	}
	void Move_right() {
		m_move.x += 0.01f;
	}
};

class Line {
public:
	glm::vec3 m_vertex[2];
	glm::vec3 m_color[2];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	GLuint m_VAO;
	Line(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		cout << "Original window coordinates: (" << x << ", " << y << ")" << endl;
		cout << "Converted OpenGL coordinates: (" << center.first << ", " << center.second << ")" << endl;
		float r = (float)(rand() % 256) / 255.0f;
		float g = (float)(rand() % 256) / 255.0f;
		float b = (float)(rand() % 256) / 255.0f;

		for (int i = 0; i < 2; ++i) {
			m_color[i].r = r;
			m_color[i].g = g;
			m_color[i].b = b;
		}
		m_vertex[0].x = center.first - 0.11f;
		m_vertex[0].y = center.second - 0.11f;
		m_vertex[1].x = center.first + 0.11f;
		m_vertex[1].y = center.second + 0.11f;
		for (int i = 0; i < 2; ++i) m_vertex[i].z = 0.0f;
		SetVAO();
	}
	void SetVAO() {
		glGenVertexArrays(1, &m_VAO);//vao ��ü ����, params(����, GLuint*)//n���� �޾Ƶ� �����ǰ��Ϸ���
		glBindVertexArray(m_VAO);//���ε��ҰŶ�� �˷��ֱ�
		//vbo��ü ����
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertex), m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		if (positionAttrib == -1) {
			std::cerr << "ERROR: 'vPos' �Ӽ��� ã�� �� �����ϴ�." << std::endl;
			return; // ���� �߻� �� �Լ��� �����մϴ�.
		}
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);

		glGenBuffers(1, &m_VBOColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_color), m_color, GL_STATIC_DRAW);
		int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
		if (colorAttrib == -1) {
			std::cerr << "ERROR: 'vColor' �Ӽ��� ã�� �� �����ϴ�." << std::endl;
			return; // ���� �߻� �� �Լ��� �����մϴ�.
		}
		glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttrib);
	}
	void UpdateVAO() {
		// ���� VAO ���ε�
		glBindVertexArray(m_VAO);

		// ���� �����͸� ������Ʈ
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertex), m_vertex);

		// ���� �����͸� ������Ʈ
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_color), m_color);
	}

	void Draw() {
		glUseProgram(shaderProgramID);
		UpdateVAO();
		glPointSize(10.0);
		glDrawArrays(GL_LINES, 0, 2);
	}
	void Move_up() {
		m_move.y += 0.01f;
	}
	void Move_down() {
		m_move.y -= 0.01f;
	}
	void Move_left() {
		m_move.x -= 0.01f;
	}
	void Move_right() {
		m_move.x += 0.01f;
	}
};

class Triangle {
public:
	glm::vec3 m_vertex[3];
	glm::vec3 m_color[3];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	GLuint m_VAO;
	Triangle(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		cout << "Original window coordinates: (" << x << ", " << y << ")" << endl;
		cout << "Converted OpenGL coordinates: (" << center.first << ", " << center.second << ")" << endl;
		float r = (float)(rand() % 256) / 255.0f;
		float g = (float)(rand() % 256) / 255.0f;
		float b = (float)(rand() % 256) / 255.0f;
		for (int i = 0; i < 3; ++i) {
			m_color[i].r = r;
			m_color[i].g = g;
			m_color[i].b = b;
		}
		m_vertex[0].x = center.first;
		m_vertex[0].y = center.second + 0.1f;
		m_vertex[1].x = center.first - 0.1f;
		m_vertex[1].y = center.second - 0.05f;
		m_vertex[2].x = center.first + 0.1f;
		m_vertex[2].y = center.second - 0.05f;
		for (int i = 0; i < 3; ++i) m_vertex[i].z = 0.0f;
		SetVAO();
	}
	void SetVAO() {
		glGenVertexArrays(1, &m_VAO);//vao ��ü ����, params(����, GLuint*)//n���� �޾Ƶ� �����ǰ��Ϸ���
		glBindVertexArray(m_VAO);//���ε��ҰŶ�� �˷��ֱ�
		//vbo��ü ����
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertex), m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		if (positionAttrib == -1) {
			std::cerr << "ERROR: 'vPos' �Ӽ��� ã�� �� �����ϴ�." << std::endl;
			return; // ���� �߻� �� �Լ��� �����մϴ�.
		}
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);

		glGenBuffers(1, &m_VBOColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_color), m_color, GL_STATIC_DRAW);
		int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
		if (colorAttrib == -1) {
			std::cerr << "ERROR: 'vColor' �Ӽ��� ã�� �� �����ϴ�." << std::endl;
			return; // ���� �߻� �� �Լ��� �����մϴ�.
		}
		glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttrib);
	}
	void UpdateVAO() {
		// ���� VAO ���ε�
		glBindVertexArray(m_VAO);

		// ���� �����͸� ������Ʈ
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertex), m_vertex);

		// ���� �����͸� ������Ʈ
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_color), m_color);
	}

	void Draw() {
		glUseProgram(shaderProgramID);
		UpdateVAO();
		glPointSize(1.0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	void Move_up() {
		m_move.y += 0.01f;
	}
	void Move_down() {
		m_move.y -= 0.01f;
	}
	void Move_left() {
		m_move.x -= 0.01f;
	}
	void Move_right() {
		m_move.x += 0.01f;
	}
};

class Rect {
public:
	glm::vec3 m_vertex[4];
	glm::vec3 m_color[4];
	glm::vec3 m_move = { 0,0,0 };
	GLuint m_VBOVertex;
	GLuint m_VBOColor;
	GLuint m_VAO;
	Rect(int x, int y) {
		auto center = ConvertWinToGL(x, y);
		cout << "Original window coordinates: (" << x << ", " << y << ")" << endl;
		cout << "Converted OpenGL coordinates: (" << center.first << ", " << center.second << ")" << endl;
		float r = (float)(rand() % 256) / 255.0f;
		float g = (float)(rand() % 256) / 255.0f;
		float b = (float)(rand() % 256) / 255.0f;
		for (int i = 0; i < 4; ++i) {
			m_color[i].r = r;
			m_color[i].g = g;
			m_color[i].b = b;
		}
		m_vertex[0].x = center.first - 0.1f;
		m_vertex[0].y = center.second + 0.1f;
		m_vertex[1].x = center.first + 0.1f;
		m_vertex[1].y = center.second + 0.1f;
		m_vertex[2].x = center.first - 0.1f;
		m_vertex[2].y = center.second - 0.1f;
		m_vertex[3].x = center.first + 0.1f;
		m_vertex[3].y = center.second - 0.1f;
		for (int i = 0; i < 4; ++i) m_vertex[i].z = 0.0f;
		SetVAO();
	}
	void SetVAO() {
		glGenVertexArrays(1, &m_VAO);//vao ��ü ����, params(����, GLuint*)//n���� �޾Ƶ� �����ǰ��Ϸ���
		glBindVertexArray(m_VAO);//���ε��ҰŶ�� �˷��ֱ�
		//vbo��ü ����
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertex), m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		if (positionAttrib == -1) {
			std::cerr << "ERROR: 'vPos' �Ӽ��� ã�� �� �����ϴ�." << std::endl;
			return; // ���� �߻� �� �Լ��� �����մϴ�.
		}
		//� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);

		glGenBuffers(1, &m_VBOColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_color), m_color, GL_STATIC_DRAW);
		int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
		if (colorAttrib == -1) {
			std::cerr << "ERROR: 'vColor' �Ӽ��� ã�� �� �����ϴ�." << std::endl;
			return; // ���� �߻� �� �Լ��� �����մϴ�.
		}
		glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttrib);
	}
	void UpdateVAO() {
		// ���� VAO ���ε�
		glBindVertexArray(m_VAO);

		// ���� �����͸� ������Ʈ
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertex), m_vertex);

		// ���� �����͸� ������Ʈ
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_color), m_color);
	}

	void Draw() {
		glUseProgram(shaderProgramID);
		UpdateVAO();
		glPointSize(1.0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	void Move_up() {
		m_move.y += 0.01f;
	}
	void Move_down() {
		m_move.y -= 0.01f;
	}
	void Move_left() {
		m_move.x -= 0.01f;
	}
	void Move_right() {
		m_move.x += 0.01f;
	}
};


vector<Point> points;
vector<Line> lines;
vector<Triangle> triangles;
vector<Rect> rects;

bool check_p{ false };
bool check_l{ false };
bool check_t{ false };
bool check_r{ false };

int mouse_x{};
int mouse_y{};



int main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	srand(static_cast<int>(time(NULL)));

	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Example7");
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

	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	shaderProgramID = make_shaderProgram();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);

	glutPassiveMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();
}

//--- �׸��� �ݹ� �Լ�
GLvoid drawScene()
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//--- ������ ���������ο� ���̴� �ҷ�����
	glUseProgram(shaderProgramID);
	//--- �ﰢ�� �׸���
	for (auto& p : points) p.Draw();
	for (auto& l : lines) l.Draw();
	for (auto& t : triangles) t.Draw();
	for (auto& r : rects) r.Draw();
	glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
}
//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void Motion(int x, int y) {
	mouse_x = x;
	mouse_y = y;
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	if (key == 'q') {
		glutDestroyWindow(true);
	}

	else if (key == 'p') {
		points.emplace_back(mouse_x, mouse_y);
		cout << "���������� �� ������" << endl;
	}

	else if (key == 'l') {
		lines.emplace_back(mouse_x, mouse_y);
		cout << "���������� �� ������" << endl;
	}

	else if (key == 't') {
		triangles.emplace_back(mouse_x, mouse_y);
		cout << "���������� �ﰢ�� ������" << endl;
	}
	
	else if (key == 'r') {
		rects.emplace_back(mouse_x, mouse_y);
		cout << "���������� �׸� ������" << endl;
	}

	else if (key == 'w') {
		int shape{ rand() % 4 };
		if (shape == 0) {
			if (points.size() == 0) return;
			size_t i{ rand() % points.size() };
			points[i].m_vertex->y += 0.05f;
		}
		else if (shape == 1){
			if (lines.size() == 0) return;
			size_t i{ rand() % lines.size() };
			lines[i].m_vertex[0].y += 0.05f;
			lines[i].m_vertex[1].y += 0.05f;
		}
		else if (shape == 2) {
			if (triangles.size() == 0) return;
			size_t i{ rand() % triangles.size() };
			//for (int j = 0; j < 3; ++j) triangles[i].m_vertex[j].y += 0.05f;
			triangles[i].m_vertex[0].y += 0.05f;
			triangles[i].m_vertex[1].y += 0.05f;
			triangles[i].m_vertex[2].y += 0.05f;
		}
		else if (shape == 3) {
			if (rects.size() == 0) return;
			size_t i{ rand() % rects.size() };
			//for (int j = 0; j < 4; ++j) rects[i].m_vertex[j].y += 0.05f;
			rects[i].m_vertex[0].y += 0.05f;
			rects[i].m_vertex[1].y += 0.05f;
			rects[i].m_vertex[2].y += 0.05f;
			rects[i].m_vertex[3].y += 0.05f;
		}
		cout << "�� ���� �̵�" << endl;
	}

	else if (key == 'a') {
		int shape{ rand() % 4 };
		if (shape == 0) {
			if (points.size() == 0) return;
			size_t i{ rand() % points.size() };
			points[i].m_vertex->x -= 0.05f;
		}
		else if (shape == 1) {
			if (lines.size() == 0) return;
			size_t i{ rand() % lines.size() };
			lines[i].m_vertex[0].x -= 0.05f;
			lines[i].m_vertex[1].x -= 0.05f;
		}
		else if (shape == 2) {
			if (triangles.size() == 0) return;
			size_t i{ rand() % triangles.size() };
			triangles[i].m_vertex[0].x -= 0.05f;
			triangles[i].m_vertex[1].x -= 0.05f;
			triangles[i].m_vertex[2].x -= 0.05f;
		}
		else if (shape == 3) {
			if (rects.size() == 0) return;
			size_t i{ rand() % rects.size() };
			rects[i].m_vertex[0].x -= 0.05f;
			rects[i].m_vertex[1].x -= 0.05f;
			rects[i].m_vertex[2].x -= 0.05f;
			rects[i].m_vertex[3].x -= 0.05f;
		}
		cout << "���� ���� �̵�" << endl;
	}

	else if (key == 's') {
		int shape{ rand() % 4 };
		if (shape == 0) {
			if (points.size() == 0) return;
			size_t i{ rand() % points.size() };
			points[i].m_vertex->y -= 0.05f;
		}
		else if (shape == 1) {
			if (lines.size() == 0) return;
			size_t i{ rand() % lines.size() };
			lines[i].m_vertex[0].y -= 0.05f;
			lines[i].m_vertex[1].y -= 0.05f;
		}
		else if (shape == 2) {
			if (triangles.size() == 0) return;
			size_t i{ rand() % triangles.size() };
			triangles[i].m_vertex[0].y -= 0.05f;
			triangles[i].m_vertex[1].y -= 0.05f;
			triangles[i].m_vertex[2].y -= 0.05f;
		}
		else if (shape == 3) {
			if (rects.size() == 0) return;
			size_t i{ rand() % rects.size() };
			rects[i].m_vertex[0].y -= 0.05f;
			rects[i].m_vertex[1].y -= 0.05f;
			rects[i].m_vertex[2].y -= 0.05f;
			rects[i].m_vertex[3].y -= 0.05f;
		}
		cout << "�Ʒ��� ���� �̵�" << endl;
	}

	else if (key == 'd') {
		int shape{ rand() % 4 };
		if (shape == 0) {
			if (points.size() == 0) return;
			size_t i{ rand() % points.size() };
			points[i].m_vertex->x += 0.05f;
		}
		else if (shape == 1) {
			if (lines.size() == 0) return;
			size_t i{ rand() % lines.size() };
			lines[i].m_vertex[0].x += 0.05f;
			lines[i].m_vertex[1].x += 0.05f;
		}
		else if (shape == 2) {
			if (triangles.size() == 0) return;
			size_t i{ rand() % triangles.size() };
			triangles[i].m_vertex[0].x += 0.05f;
			triangles[i].m_vertex[1].x += 0.05f;
			triangles[i].m_vertex[2].x += 0.05f;
		}
		else if (shape == 3) {
			if (rects.size() == 0) return;
			size_t i{ rand() % rects.size() };
			rects[i].m_vertex[0].x += 0.05f;
			rects[i].m_vertex[1].x += 0.05f;
			rects[i].m_vertex[2].x += 0.05f;
			rects[i].m_vertex[3].x += 0.05f;
		}
		cout << "������ ���� �̵�" << endl;
	}

	else if (key == 'c') {
		points.clear();
		lines.clear();
		triangles.clear();
		rects.clear();
	}

	glutPostRedisplay();
}

//--- ���̴� ���α׷� �����ϱ�
GLuint make_shaderProgram() {
	//vertex, frament shader�� �̹� �������� �� ��Ȳ
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- ���̴� ���α׷� ����� - �� ���̴� �پ�ߵ�, vertex - fragment�� ¦�� �¾ƾߵ�
	glAttachShader(shaderID, vertexShader); //--- ���̴� ���α׷��� ���ؽ� ���̴� ���̱�
	glAttachShader(shaderID, fragmentShader); //--- ���̴� ���α׷��� �����׸�Ʈ ���̴� ���̱�
	glLinkProgram(shaderID); //--- ���̴� ���α׷� ��ũ�ϱ�
	glDeleteShader(vertexShader); //--- ���̴� ��ü�� ���̴� ���α׷��� ��ũ��������, ���̴� ��ü ��ü�� ���� ����
	glDeleteShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---���̴��� �� ����Ǿ����� üũ�ϱ�
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program ���� ����\n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID); //--- ������� ���̴� ���α׷� ����ϱ�
	return shaderID;
}

//--- ���ؽ� ���̴� ��ü �����
void make_vertexShaders()
{
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex.glsl");
		//--- ���ؽ� ���̴� ��ü �����
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- ���ؽ� ���̴� �������ϱ�
	glCompileShader(vertexShader);
	//--- �������� ����� ���� ���� ���: ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if(!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader ������ ����\n" << errorLog << endl;
		return;
	}
}

//--- �����׸�Ʈ ���̴� ��ü �����
void make_fragmentShaders()
{
	GLchar* fragmentSource;
	fragmentSource = filetobuf("fragment.glsl");
	//--- �����׸�Ʈ ���̴� ��ü �����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- �����׸�Ʈ ���̴� ������
	glCompileShader(fragmentShader);
	//--- �������� ����� ���� ���� ���: ������ ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: fragment shader ������ ����\n" << errorLog << endl;
		return;
	}
}