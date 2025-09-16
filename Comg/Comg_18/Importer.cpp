#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
#include "Importer.h"

using namespace std;

void Importer::read_newline(char* str) {
	char* pos;
	if ((pos = strchr(str, '\n')) != NULL)
		*pos = '\0';
}


void Importer::read_obj_file(const std::string& filename, VertexData* vertexData) {
    FILE* file;
    fopen_s(&file, filename.c_str(), "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        read_newline(line);

        // Vertex information (v x y z)
        if (line[0] == 'v' && line[1] == ' ') {
            glm::vec3 temp_vertex;
            int result = sscanf_s(line + 2, "%f %f %f", &temp_vertex.x, &temp_vertex.y, &temp_vertex.z);
            if (result == 3) {
                vertexData->vertex.push_back(temp_vertex);
                glm::vec3 temp_color = { (float)(rand() % 256) / 255.0f, (float)(rand() % 256) / 255.0f, (float)(rand() % 256) / 255.0f }; // Assigning a default color (red)
                
                vertexData->color.push_back(temp_color);
            }
        }
        // Face information (f v1 v2 v3)
        else if (line[0] == 'f' && line[1] == ' ') {
            unsigned int v1, v2, v3;
            int result = sscanf_s(line + 2, "%u/%*u/%*u %u/%*u/%*u %u/%*u/%*u", &v1, &v2, &v3);
            if (result == 3) {
                // OBJ indices start at 1, adjust to 0-based index
                v1--; v2--; v3--;

                // Store the face as a set of vertex indices in the face vector
                vertexData->face.push_back(glm::ivec3(v1, v2, v3));
            }
        }
    }

    fclose(file);
    vertexData->filename = filename;
}

void Importer::rearrangeVerticesByFace(VertexData* vertexData) {
    // ���Ӱ� �����ĵ� vertex�� color �����͸� ������ ����
    vector<glm::vec3> rearrangedVertices;
    vector<glm::vec3> rearrangedColors;

    // face�� �ε����� ������� vertex�� color �����͸� ������
    for (const auto& face : vertexData->face) {
        rearrangedVertices.push_back(vertexData->vertex[face.x]);
        rearrangedVertices.push_back(vertexData->vertex[face.y]);
        rearrangedVertices.push_back(vertexData->vertex[face.z]);

        rearrangedColors.push_back(vertexData->color[face.x]);
        rearrangedColors.push_back(vertexData->color[face.y]);
        rearrangedColors.push_back(vertexData->color[face.z]);
    }

    // ���� vertex�� color ���͸� �����ĵ� ���ͷ� ��ü
    vertexData->vertex = rearrangedVertices;
    vertexData->color = rearrangedColors;
}

void Importer::ReadObj() {
	//VertexData* newVertexData = new VertexData;
	//
    //read_obj_file("cube.obj", newVertexData);
    //rearrangeVerticesByFace(newVertexData);
    //
	//VertexBuffers.push_back(newVertexData);
	//SetupMesh(VertexBuffers.back());


    VertexData* newVertexData1 = new VertexData;

    read_obj_file("sphere.obj", newVertexData1);
    rearrangeVerticesByFace(newVertexData1);

    VertexBuffers.push_back(newVertexData1);
    SetupMesh(VertexBuffers.back());


   //VertexData* newVertexData6 = new VertexData;
   //
   //read_obj_file("pyramid.obj", newVertexData6);
   //rearrangeVerticesByFace(newVertexData6);
   //
   //VertexBuffers.push_back(newVertexData6);
   //SetupMesh(VertexBuffers.back());


    VertexData* newVertexData3 = new VertexData;

    newVertexData3->filename = "x_Line";

    glm::vec3 temp_vertex = { -100.0f, 0.0f, 0.0f };
    glm::vec3 temp_color = { 1.0f, 0.0f, 0.0f };
    newVertexData3->vertex.push_back(temp_vertex);
    newVertexData3->color.push_back(temp_color);

    temp_vertex = { 100.0f, 0.0f, 0.0f };
    temp_color = { 1.0f, 0.0f, 0.0f };
    newVertexData3->vertex.push_back(temp_vertex);
    newVertexData3->color.push_back(temp_color);

    VertexBuffers.push_back(newVertexData3);
    SetupMesh(VertexBuffers.back());


    VertexData* newVertexData4 = new VertexData;

    newVertexData4->filename = "y_Line";

    temp_vertex = { 0.0f, -100.0f, 0.0f };
    temp_color = { 0.0f, 1.0f, 0.0f };
    newVertexData4->vertex.push_back(temp_vertex);
    newVertexData4->color.push_back(temp_color);

    temp_vertex = { 0.0f, 100.0f, 0.0f };
    temp_color = { 0.0f, 1.0f, 0.0f };
    newVertexData4->vertex.push_back(temp_vertex);
    newVertexData4->color.push_back(temp_color);

    VertexBuffers.push_back(newVertexData4);
    SetupMesh(VertexBuffers.back());


    VertexData* newVertexData5 = new VertexData;

    newVertexData5->filename = "z_Line";

    temp_vertex = { 0.0f, 0.0f, -100.0f };
    temp_color = { 0.0f, 0.0f, 1.0f };
    newVertexData5->vertex.push_back(temp_vertex);
    newVertexData5->color.push_back(temp_color);

    temp_vertex = { 0.0f, 0.0f, 100.0f };
    temp_color = { 0.0f, 0.0f, 1.0f };
    newVertexData5->vertex.push_back(temp_vertex);
    newVertexData5->color.push_back(temp_color);

    VertexBuffers.push_back(newVertexData5);
    SetupMesh(VertexBuffers.back());
}

void Importer::SetupMesh(VertexData* VD) {
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
