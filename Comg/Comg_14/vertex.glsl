//--- vertex shader: vertex.glsl 파일에 저장

#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor; // 색상 속성

vec3 fColor; // 색상 전달을 위한 변수 추가
uniform mat4 transform; 

void main(){
   gl_Position = transform * vec4(vPos, 1.0);
   fColor = vColor; // 색상 값을 프래그먼트 셰이더로 전달
}

