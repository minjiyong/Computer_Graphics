//--- fragment shader: fragment.glsl ���Ͽ� ����

#version 330 core

in vec3 fColor; // �����׸�Ʈ ���̴��� ���޵� ����
out vec4 color;

void main (){
    color = vec4(fColor, 1.0); // ���޵� �������� ���
}