//--- vertex shader: vertex.glsl ���Ͽ� ����

#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor; // ���� �Ӽ�

out vec3 fColor; // ���� ������ ���� ���� �߰�

void main(){
   gl_Position = vec4(vPos, 1.0);
   fColor = vColor; // ���� ���� �����׸�Ʈ ���̴��� ����
}

//�����̰��� - scale, ����, �̵�, ����, �θ�