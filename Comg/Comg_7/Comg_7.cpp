#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
//--- 메인 함수
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm.hpp>

#include <vector>
#include <random>
#include <utility>

using namespace std;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

//--- 함수 선언 추가하기
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
	float mx = ((float)x - (WINDOW_WIDTH / 2)) / (WINDOW_WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (WINDOW_HEIGHT / 2)) / (WINDOW_HEIGHT / 2); //gl좌표계로 변경
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


//--- 필요한 변수 선언
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader, fragmentShader; //--- 세이더 객체


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
		glGenVertexArrays(1, &m_VAO);//vao 객체 생성, params(갯수, GLuint*)//n개를 받아도 생성되게하려고
		glBindVertexArray(m_VAO);//바인딩할거라고 알려주기
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertex), m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		if (positionAttrib == -1) {
			std::cerr << "ERROR: 'vPos' 속성을 찾을 수 없습니다." << std::endl;
			return; // 에러 발생 시 함수를 종료합니다.
		}
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);

		glGenBuffers(1, &m_VBOColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_color), m_color, GL_STATIC_DRAW);
		int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
		if (colorAttrib == -1) {
			std::cerr << "ERROR: 'vColor' 속성을 찾을 수 없습니다." << std::endl;
			return; // 에러 발생 시 함수를 종료합니다.
		}
		glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttrib);
	}
	void UpdateVAO() {
		// 기존 VAO 바인딩
		glBindVertexArray(m_VAO);

		// 정점 데이터를 업데이트
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertex), m_vertex);

		// 색상 데이터를 업데이트
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
		glGenVertexArrays(1, &m_VAO);//vao 객체 생성, params(갯수, GLuint*)//n개를 받아도 생성되게하려고
		glBindVertexArray(m_VAO);//바인딩할거라고 알려주기
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertex), m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		if (positionAttrib == -1) {
			std::cerr << "ERROR: 'vPos' 속성을 찾을 수 없습니다." << std::endl;
			return; // 에러 발생 시 함수를 종료합니다.
		}
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);

		glGenBuffers(1, &m_VBOColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_color), m_color, GL_STATIC_DRAW);
		int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
		if (colorAttrib == -1) {
			std::cerr << "ERROR: 'vColor' 속성을 찾을 수 없습니다." << std::endl;
			return; // 에러 발생 시 함수를 종료합니다.
		}
		glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttrib);
	}
	void UpdateVAO() {
		// 기존 VAO 바인딩
		glBindVertexArray(m_VAO);

		// 정점 데이터를 업데이트
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertex), m_vertex);

		// 색상 데이터를 업데이트
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
		glGenVertexArrays(1, &m_VAO);//vao 객체 생성, params(갯수, GLuint*)//n개를 받아도 생성되게하려고
		glBindVertexArray(m_VAO);//바인딩할거라고 알려주기
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertex), m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		if (positionAttrib == -1) {
			std::cerr << "ERROR: 'vPos' 속성을 찾을 수 없습니다." << std::endl;
			return; // 에러 발생 시 함수를 종료합니다.
		}
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);

		glGenBuffers(1, &m_VBOColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_color), m_color, GL_STATIC_DRAW);
		int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
		if (colorAttrib == -1) {
			std::cerr << "ERROR: 'vColor' 속성을 찾을 수 없습니다." << std::endl;
			return; // 에러 발생 시 함수를 종료합니다.
		}
		glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttrib);
	}
	void UpdateVAO() {
		// 기존 VAO 바인딩
		glBindVertexArray(m_VAO);

		// 정점 데이터를 업데이트
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertex), m_vertex);

		// 색상 데이터를 업데이트
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
		glGenVertexArrays(1, &m_VAO);//vao 객체 생성, params(갯수, GLuint*)//n개를 받아도 생성되게하려고
		glBindVertexArray(m_VAO);//바인딩할거라고 알려주기
		//vbo객체 생성
		glGenBuffers(1, &m_VBOVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertex), m_vertex, GL_STATIC_DRAW);
		int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
		if (positionAttrib == -1) {
			std::cerr << "ERROR: 'vPos' 속성을 찾을 수 없습니다." << std::endl;
			return; // 에러 발생 시 함수를 종료합니다.
		}
		//어떤 어트리브인지, 세이더에서 vec3면 3(변수 갯수), 데이터 타입, 정규화, 하나의 덩어리 크기?, 시작 위치
		glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionAttrib);

		glGenBuffers(1, &m_VBOColor);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_color), m_color, GL_STATIC_DRAW);
		int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
		if (colorAttrib == -1) {
			std::cerr << "ERROR: 'vColor' 속성을 찾을 수 없습니다." << std::endl;
			return; // 에러 발생 시 함수를 종료합니다.
		}
		glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttrib);
	}
	void UpdateVAO() {
		// 기존 VAO 바인딩
		glBindVertexArray(m_VAO);

		// 정점 데이터를 업데이트
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertex), m_vertex);

		// 색상 데이터를 업데이트
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



int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	srand(static_cast<int>(time(NULL)));

	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Example7");
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

	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);

	glutPassiveMotionFunc(Motion);
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
	glUseProgram(shaderProgramID);
	//--- 삼각형 그리기
	for (auto& p : points) p.Draw();
	for (auto& l : lines) l.Draw();
	for (auto& t : triangles) t.Draw();
	for (auto& r : rects) r.Draw();
	glutSwapBuffers(); //--- 화면에 출력하기
}
//--- 다시그리기 콜백 함수
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
		cout << "성공적으로 점 생성됨" << endl;
	}

	else if (key == 'l') {
		lines.emplace_back(mouse_x, mouse_y);
		cout << "성공적으로 선 생성됨" << endl;
	}

	else if (key == 't') {
		triangles.emplace_back(mouse_x, mouse_y);
		cout << "성공적으로 삼각형 생성됨" << endl;
	}
	
	else if (key == 'r') {
		rects.emplace_back(mouse_x, mouse_y);
		cout << "성공적으로 네모 생성됨" << endl;
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
		cout << "위 방향 이동" << endl;
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
		cout << "왼쪽 방향 이동" << endl;
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
		cout << "아래쪽 방향 이동" << endl;
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
		cout << "오른쪽 방향 이동" << endl;
	}

	else if (key == 'c') {
		points.clear();
		lines.clear();
		triangles.clear();
		rects.clear();
	}

	glutPostRedisplay();
}

//--- 세이더 프로그램 생성하기
GLuint make_shaderProgram() {
	//vertex, frament shader가 이미 컴파일은 된 상황
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기 - 두 세이더 붙어야됨, vertex - fragment는 짝이 맞아야됨
	glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기
	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID); //--- 만들어진 세이더 프로그램 사용하기
	return shaderID;
}

//--- 버텍스 세이더 객체 만들기
void make_vertexShaders()
{
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex.glsl");
		//--- 버텍스 세이더 객체 만들기
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);
	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if(!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << endl;
		return;
	}
}

//--- 프래그먼트 세이더 객체 만들기
void make_fragmentShaders()
{
	GLchar* fragmentSource;
	fragmentSource = filetobuf("fragment.glsl");
	//--- 프래그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);
	//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << endl;
		return;
	}
}