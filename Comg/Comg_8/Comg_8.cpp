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

void Mouse(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();


//--- �ʿ��� ���� ����
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader, fragmentShader; //--- ���̴� ��ü


struct VertexData {
	string filename;

	GLuint VAO;
	GLuint VBOVertex;
	GLuint VBOColor;

	vector <glm::vec3> vertex;
	vector <glm::vec3> color;
};


class Importer {
public:
	vector<VertexData*> VertexBuffers;

	void GetShapeData() {
	}

	void ReadObj() {
		VertexData* newVertexData = new VertexData;

		newVertexData->filename = "Triangle";

		glm::vec3 temp_vertex = { 0.0f, 0.15f, 0.0f };
		glm::vec3 temp_color = { 1.0f, 0.0f, 0.0f };
		newVertexData->vertex.push_back(temp_vertex);
		newVertexData->color.push_back(temp_color);

		temp_vertex = { -0.1f, -0.05f, 0.0f };
		temp_color = { 1.0f, 0.0f, 0.0f };
		newVertexData->vertex.push_back(temp_vertex);
		newVertexData->color.push_back(temp_color);

		temp_vertex = { 0.1f, -0.05f, 0.0f };
		temp_color = { 1.0f, 0.0f, 0.0f };
		newVertexData->vertex.push_back(temp_vertex);
		newVertexData->color.push_back(temp_color);

		VertexBuffers.push_back(newVertexData);
		SetupMesh(VertexBuffers[0]);

		VertexData* newVertexData2 = new VertexData;

		newVertexData2->filename = "Line";

		temp_vertex = { -1.0f, 0.0f, 0.0f };
		temp_color = { 0.0f, 0.0f, 0.0f };
		newVertexData2->vertex.push_back(temp_vertex);
		newVertexData2->color.push_back(temp_color);

		temp_vertex = { 1.0f, 0.0f, 0.0f };
		temp_color = { 0.0f, 0.0f, 0.0f };
		newVertexData2->vertex.push_back(temp_vertex);
		newVertexData2->color.push_back(temp_color);

		VertexBuffers.push_back(newVertexData2);
		SetupMesh(VertexBuffers[1]);
	}

	void SetupMesh(VertexData* VD) {
		glGenVertexArrays(1, &VD->VAO);
		glGenBuffers(1, &VD->VBOVertex);
		glGenBuffers(1, &VD->VBOColor);

		glBindVertexArray(VD->VAO);


		glBindBuffer(GL_ARRAY_BUFFER, VD->VBOVertex);
		glBufferData(GL_ARRAY_BUFFER, VD->vertex.size() * sizeof(glm::vec3), &VD->vertex[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, VD->VBOColor);
		glBufferData(GL_ARRAY_BUFFER, VD->color.size() * sizeof(glm::vec3), &VD->color[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

		glBindVertexArray(0);
	}
};

Importer importer;


class Object {
public:
	GLuint objVAO{};

	glm::vec3 location{};	// ��ġ(translate ����)
	glm::vec3 color{};		// ����

	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };  // ũ�� (����Ʈ�� 1)
	float rotationAngle = 0.0f;  // ȸ�� ���� (����)

	void initialize_Object(string name, glm::vec3 pos) {
		for (auto& v : importer.VertexBuffers) {
			if (name == v->filename) {
				objVAO = v->VAO;
				location = pos;
				color.r = (float)(rand() % 256) / 255.0f;
				color.g = (float)(rand() % 256) / 255.0f;
				color.b = (float)(rand() % 256) / 255.0f;
				scale = glm::vec3(1.0f, 1.0f, 1.0f);  // �⺻ ũ�� ����
				rotationAngle = 0.0f;  // �⺻ ȸ�� ���� ����
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
		rotationAngle = glm::radians(angle);  // �Է��� ����, ���ο����� ���� ���
	}

	void make_Color_random() {
		color.r = (float)(rand() % 256) / 255.0f;
		color.g = (float)(rand() % 256) / 255.0f;
		color.b = (float)(rand() % 256) / 255.0f;
	}

	void Draw_triangle() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));  // Z�� ���� ȸ��
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(shaderProgramID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color�� ���̴��� ����
		unsigned int ObjectColor = glGetUniformLocation(shaderProgramID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
	}

	void Draw_line() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));  // Z�� ���� ȸ��
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(shaderProgramID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color�� ���̴��� ����
		unsigned int ObjectColor = glGetUniformLocation(shaderProgramID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}

	void Draw_triangle_onlyLine() {
		glm::mat4 transform_Matrix = glm::mat4(1.0f);

		transform_Matrix = glm::translate(transform_Matrix, location);
		transform_Matrix = glm::rotate(transform_Matrix, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));  // Z�� ���� ȸ��
		transform_Matrix = glm::scale(transform_Matrix, scale);

		unsigned int ObjectTransform = glGetUniformLocation(shaderProgramID, "transform");
		glUniformMatrix4fv(ObjectTransform, 1, GL_FALSE, glm::value_ptr(transform_Matrix));

		// color�� ���̴��� ����
		unsigned int ObjectColor = glGetUniformLocation(shaderProgramID, "fColor");
		glUniform3fv(ObjectColor, 1, glm::value_ptr(color));

		glBindVertexArray(objVAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
	}
};

vector<Object> triangle;
Object line[2];

bool check_line{ false };
bool check_fill{ true };

int main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	srand(static_cast<int>(time(NULL)));

	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Example8");
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

	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	shaderProgramID = make_shaderProgram();

	line[0].initialize_Object("Line", { 0.0f, 0.0f, 0.0f });
	line[1].initialize_Object("Line", { 0.0f, 0.0f, 0.0f });
	line[1].setRotation(90);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);

	glutMouseFunc(Mouse);
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
	
	for (int i = 0; i < 2; ++i) line[i].Draw_line();
	if(check_fill) for(auto& v : triangle) v.Draw_triangle();
	else if(check_line) for(auto& v : triangle) v.Draw_triangle_onlyLine();
	
	glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
}
//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}


void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		auto mouse = ConvertWinToGL(x, y);

		// 1 ������� 
		if (-1.0f <= mouse.first && mouse.first <= 0.0f && 0.0f <= mouse.second && mouse.second <= 1.0f) {
			for (int it = triangle.size() - 1; it >= 0; --it) {  // �������� ��ȸ
				if (-1.0f <= triangle[it].location.x && triangle[it].location.x <= 0.0f && 0.0f <= triangle[it].location.y && triangle[it].location.y <= 1.0f) {
					triangle.erase(triangle.begin() + it);
				}
			}
		}
		// 2 �������
		else if (0.0f <= mouse.first && mouse.first <= 1.0f && 0.0f <= mouse.second && mouse.second <= 1.0f) {
			for (int it = triangle.size() - 1; it >= 0; --it) {  // �������� ��ȸ
				if (0.0f <= triangle[it].location.x && triangle[it].location.x <= 1.0f && 0.0f <= triangle[it].location.y && triangle[it].location.y <= 1.0f) {
					triangle.erase(triangle.begin() + it);
				}
			}
		}
		// 3 �����ϴ�
		else if (-1.0f <= mouse.first && mouse.first <= 0.0f && -1.0f <= mouse.second && mouse.second <= 0.0f) {
			for (int it = triangle.size() - 1; it >= 0; --it) {  // �������� ��ȸ
				if (-1.0f <= triangle[it].location.x && triangle[it].location.x <= 0.0f && -1.0f <= triangle[it].location.y && triangle[it].location.y <= 0.0f) {
					triangle.erase(triangle.begin() + it);
				}
			}
		}
		// 4 �����ϴ�
		else if (0.0f <= mouse.first && mouse.first <= 1.0f && -1.0f <= mouse.second && mouse.second <= 0.0f) {
			for (int it = triangle.size() - 1; it >= 0; --it) {  // �������� ��ȸ
				if (0.0f <= triangle[it].location.x && triangle[it].location.x <= 1.0f && -1.0f <= triangle[it].location.y && triangle[it].location.y <= 0.0f) {
					triangle.erase(triangle.begin() + it);
				}
			}
		}

		// �� �ﰢ�� �߰�
		Object temp_triangle;
		temp_triangle.initialize_Object("Triangle", { mouse.first, mouse.second, 0.0f });
		temp_triangle.setScale_random();
		triangle.push_back(temp_triangle);
	}

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		auto mouse = ConvertWinToGL(x, y);

		int what_area[4]{};

		for (auto& t : triangle) {
			// 1 ������� 
			if (-1.0f <= t.location.x && t.location.x <= 0.0f && 0.0f <= t.location.y && t.location.y <= 1.0f) ++what_area[0];
			// 2 �������
			else if (0.0f <= t.location.x && t.location.x <= 1.0f && 0.0f <= t.location.y && t.location.y <= 1.0f) ++what_area[1];
			// 3 �����ϴ�
			else if (-1.0f <= t.location.x && t.location.x <= 0.0f && -1.0f <= t.location.y && t.location.y <= 0.0f)  ++what_area[2];
			// 4 �����ϴ�
			else if (0.0f <= t.location.x && t.location.x <= 1.0f && -1.0f <= t.location.y && t.location.y <= 0.0f) ++what_area[3];
		}

		// 1 ������� 
		if (-1.0f <= mouse.first && mouse.first <= 0.0f && 0.0f <= mouse.second && mouse.second <= 1.0f) {
			if (what_area[0] >= 3) return;

			Object temp_triangle;
			temp_triangle.initialize_Object("Triangle", { mouse.first, mouse.second, 0.0f });
			triangle.push_back(temp_triangle);
		}
		// 2 �������
		else if (0.0f <= mouse.first && mouse.first <= 1.0f && 0.0f <= mouse.second && mouse.second <= 1.0f) {
			if (what_area[1] >= 3) return;

			Object temp_triangle;
			temp_triangle.initialize_Object("Triangle", { mouse.first, mouse.second, 0.0f });
			triangle.push_back(temp_triangle);
		}
		// 3 �����ϴ�
		else if (-1.0f <= mouse.first && mouse.first <= 0.0f && -1.0f <= mouse.second && mouse.second <= 0.0f) {
			if (what_area[2] >= 3) return;

			Object temp_triangle;
			temp_triangle.initialize_Object("Triangle", { mouse.first, mouse.second, 0.0f });
			triangle.push_back(temp_triangle);
		}
		// 4 �����ϴ�
		else if (0.0f <= mouse.first && mouse.first <= 1.0f && -1.0f <= mouse.second && mouse.second <= 0.0f) {
			if (what_area[3] >= 3) return;

			Object temp_triangle;
			temp_triangle.initialize_Object("Triangle", { mouse.first, mouse.second, 0.0f });
			triangle.push_back(temp_triangle);
		}
	}
	
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	if (key == 'q') {
		glutDestroyWindow(true);
	}

	else if (key == 'a') {
		check_line = true;
		check_fill = false;
	}
	else if (key == 'b') {
		check_fill = true;
		check_line = false;
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
	if (!result)
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