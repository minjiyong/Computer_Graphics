//--- fragment shader: fragment.glsl 파일에 저장

#version 330 core

in vec3 fColor; // 프래그먼트 셰이더로 전달된 색상
out vec4 color;

void main (){
    color = vec4(fColor, 1.0); // 전달된 색상으로 출력
}