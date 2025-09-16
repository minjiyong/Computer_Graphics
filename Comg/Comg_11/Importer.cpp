#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include "Importer.h"

using namespace std;

void Importer::ReadObj() {
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
	SetupMesh(VertexBuffers.back());


	VertexData* newVertexData2 = new VertexData;

	newVertexData2->filename = "xy_Line";

	temp_vertex = { -1.0f, 0.0f, 0.0f };
	temp_color = { 0.0f, 0.0f, 0.0f };
	newVertexData2->vertex.push_back(temp_vertex);
	newVertexData2->color.push_back(temp_color);

	temp_vertex = { 1.0f, 0.0f, 0.0f };
	temp_color = { 0.0f, 0.0f, 0.0f };
	newVertexData2->vertex.push_back(temp_vertex);
	newVertexData2->color.push_back(temp_color);

	VertexBuffers.push_back(newVertexData2);
	SetupMesh(VertexBuffers.back());


	VertexData* newVertexData3 = new VertexData;

	newVertexData3->filename = "Point";

	temp_vertex = { 0.0f, 0.0f, 0.0f };
	temp_color = { 0.0f, 0.0f, 0.0f };
	newVertexData3->vertex.push_back(temp_vertex);
	newVertexData3->color.push_back(temp_color);

	VertexBuffers.push_back(newVertexData3);
	SetupMesh(VertexBuffers.back());


	VertexData* newVertexData4 = new VertexData;

	newVertexData4->filename = "Rectangle";

	temp_vertex = { -0.1f, 0.1f, 0.0f };
	temp_color = { 0.0f, 0.0f, 0.0f };
	newVertexData4->vertex.push_back(temp_vertex);
	newVertexData4->color.push_back(temp_color);

	temp_vertex = { 0.1f, 0.1f, 0.0f };
	temp_color = { 0.0f, 0.0f, 0.0f };
	newVertexData4->vertex.push_back(temp_vertex);
	newVertexData4->color.push_back(temp_color);

	temp_vertex = { -0.1f, -0.1f, 0.0f };
	temp_color = { 0.0f, 0.0f, 0.0f };
	newVertexData4->vertex.push_back(temp_vertex);
	newVertexData4->color.push_back(temp_color);

	temp_vertex = { 0.1f, -0.1f, 0.0f };
	temp_color = { 0.0f, 0.0f, 0.0f };
	newVertexData4->vertex.push_back(temp_vertex);
	newVertexData4->color.push_back(temp_color);

	VertexBuffers.push_back(newVertexData4);
	SetupMesh(VertexBuffers.back());


	VertexData* newVertexData5 = new VertexData;

	newVertexData5->filename = "Pentagon";

	temp_vertex = { 0.0f, 0.08f, 0.0f };
	temp_color = { 0.0f, 0.0f, 0.0f };
	newVertexData5->vertex.push_back(temp_vertex);
	newVertexData5->color.push_back(temp_color);

	temp_vertex = { 0.1f, 0.0f, 0.0f };
	temp_color = { 0.0f, 0.0f, 0.0f };
	newVertexData5->vertex.push_back(temp_vertex);
	newVertexData5->color.push_back(temp_color);

	temp_vertex = { -0.1f, 0.0f, 0.0f };
	temp_color = { 0.0f, 0.0f, 0.0f };
	newVertexData5->vertex.push_back(temp_vertex);
	newVertexData5->color.push_back(temp_color);

	temp_vertex = { 0.06f, -0.11f, 0.0f };
	temp_color = { 0.0f, 0.0f, 0.0f };
	newVertexData5->vertex.push_back(temp_vertex);
	newVertexData5->color.push_back(temp_color);

	temp_vertex = { -0.06f, -0.11f, 0.0f };
	temp_color = { 0.0f, 0.0f, 0.0f };
	newVertexData5->vertex.push_back(temp_vertex);
	newVertexData5->color.push_back(temp_color);

	VertexBuffers.push_back(newVertexData5);
	SetupMesh(VertexBuffers.back());


	VertexData* newVertexData6 = new VertexData;

	newVertexData6->filename = "Line";

	temp_vertex = { -0.1f, -0.05f, 0.0f };
	temp_color = { 0.0f, 0.0f, 0.0f };
	newVertexData6->vertex.push_back(temp_vertex);
	newVertexData6->color.push_back(temp_color);

	temp_vertex = { 0.1f, -0.05f, 0.0f };
	temp_color = { 0.0f, 0.0f, 0.0f };
	newVertexData6->vertex.push_back(temp_vertex);
	newVertexData6->color.push_back(temp_color);

	VertexBuffers.push_back(newVertexData6);
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
