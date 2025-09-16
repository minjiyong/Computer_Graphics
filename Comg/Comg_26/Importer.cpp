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
        // ���ؽ� �븻
        else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
            glm::vec3 temp_vertex;
            int result = sscanf_s(line + 3, "%f %f %f", &temp_vertex.x, &temp_vertex.y, &temp_vertex.z);
            if (result == 3) {
                vertexData->vertexnormal.push_back(temp_vertex);
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

void Importer::rearrangeVerticesByFace_inCUBE(VertexData* vertexData) {
    // ���Ӱ� �����ĵ� vertex, normal �����͸� ������ ����
    vector<glm::vec3> rearrangedVertices;
    vector<glm::vec3> rearrangedNormals;

    // face�� �ε����� ������� vertex�� normal �����͸� ������
    for (const auto& face : vertexData->face) {
        // ���� face�� ���� ����
        glm::vec3 faceNormal(0.0f, 0.0f, 0.0f);

        // face.x, face.y, face.z�� �ش��ϴ� ���� ��������
        glm::vec3 v1 = vertexData->vertex[face.x];
        glm::vec3 v2 = vertexData->vertex[face.y];
        glm::vec3 v3 = vertexData->vertex[face.z];

        // ���� ��� (v1, v2, v3�� ����)
        glm::vec3 edge1 = v2 - v1;
        glm::vec3 edge2 = v3 - v1;
        faceNormal = glm::normalize(glm::cross(edge1, edge2));

        // ���� �߰�
        rearrangedVertices.push_back(v1);
        rearrangedVertices.push_back(v2);
        rearrangedVertices.push_back(v3);

        // ������ ������ �� ������ ����
        rearrangedNormals.push_back(faceNormal);
        rearrangedNormals.push_back(faceNormal);
        rearrangedNormals.push_back(faceNormal);
    }

    // ���� vertex�� normal ���͸� �����ĵ� ���ͷ� ��ü
    vertexData->vertex = rearrangedVertices;
    vertexData->vertexnormal = rearrangedNormals;
}


void Importer::rearrangeVerticesByFace(VertexData* vertexData) {
    // ���Ӱ� �����ĵ� vertex�� color �����͸� ������ ����
    vector<glm::vec3> rearrangedVertices;
    vector<glm::vec3> rearrangedVN;
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
    vertexData->vertexnormal = rearrangedVN;
}

void Importer::rearrangeNormalsByFace(VertexData* vertexData) {
    // ���Ӱ� �����ĵ� ���� �����͸� ������ ����
    vector<glm::vec3> rearrangedNormals;

    // face�� �ε����� ������� ���� �����͸� ������
    for (const auto& face : vertexData->face) {
        // �� face�� x, y, z �ε����� ���� ó��
        glm::vec3 normalX = (face.x < vertexData->vertexnormal.size())
            ? vertexData->vertexnormal[face.x]
            : glm::vec3(0.0f, 0.0f, 0.0f); // ���� �� �⺻ ��
        glm::vec3 normalY = (face.y < vertexData->vertexnormal.size())
            ? vertexData->vertexnormal[face.y]
            : glm::vec3(0.0f, 0.0f, 0.0f); // ���� �� �⺻ ��
        glm::vec3 normalZ = (face.z < vertexData->vertexnormal.size())
            ? vertexData->vertexnormal[face.z]
            : glm::vec3(0.0f, 0.0f, 0.0f); // ���� �� �⺻ ��

        // �����ĵ� ���� ���Ϳ� �߰�
        rearrangedNormals.push_back(normalX);
        rearrangedNormals.push_back(normalY);
        rearrangedNormals.push_back(normalZ);
    }

    // ���� vertexnormal ���͸� �����ĵ� ���ͷ� ��ü
    vertexData->vertexnormal = rearrangedNormals;
}

void Importer::ReadObj() {
	VertexData* newVertexData = new VertexData;
	
    read_obj_file("cube.obj", newVertexData);
    rearrangeVerticesByFace_inCUBE(newVertexData);
    //rearrangeVerticesByFace(newVertexData);
    //rearrangeNormalsByFace(newVertexData);
    
	VertexBuffers.push_back(newVertexData);
	SetupMesh(VertexBuffers.back());

    VertexData* newVertexData1 = new VertexData;

    read_obj_file("long_cube.obj", newVertexData1);
    rearrangeVerticesByFace_inCUBE(newVertexData1);

    VertexBuffers.push_back(newVertexData1);
    SetupMesh(VertexBuffers.back());


    //VertexData* newVertexData1 = new VertexData;
    //
    //read_obj_file("sphere.obj", newVertexData1);
    //rearrangeVerticesByFace(newVertexData1);
    //rearrangeNormalsByFace(newVertexData1);
    //
    //VertexBuffers.push_back(newVertexData1);
    //SetupMesh(VertexBuffers.back());


    //VertexData* newVertexData6 = new VertexData;
    //
    //read_obj_file("pyramid.obj", newVertexData6);
    //rearrangeVerticesByFace(newVertexData6);
    //rearrangeNormalsByFace(newVertexData6);
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
	glGenBuffers(1, &VD->VBOVertexNormal);

	glBindVertexArray(VD->VAO);


	glBindBuffer(GL_ARRAY_BUFFER, VD->VBOVertex);
	glBufferData(GL_ARRAY_BUFFER, VD->vertex.size() * sizeof(glm::vec3), &VD->vertex[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    if (!VD->vertexnormal.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, VD->VBOVertexNormal);
        glBufferData(GL_ARRAY_BUFFER, VD->vertexnormal.size() * sizeof(glm::vec3), &VD->vertexnormal[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    }

	glBindVertexArray(0);
}
