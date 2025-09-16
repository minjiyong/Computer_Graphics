//--- fragment shader: fragment.glsl 파일에 저장

#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 Fragcolor;


uniform vec3 lightPos;   // 광원의 위치
uniform vec3 lightColor; // 광원의 색상
uniform float lightIntensity; // 광원의 강도
uniform vec3 viewPos;    // 카메라 위치
uniform vec3 fColor;     // 프래그먼트 셰이더로 전달된 색상 == 물체의 색상

void main (){
    // Ambient
    float ambientLight = 0.5;
    vec3 ambient = ambientLight * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 1.0;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // 최종 색상
    vec3 result = lightIntensity * (ambient + diffuse + specular) * fColor;

    // Fragcolor = vec4(Normal * 0.5 + 0.5, 1.0); // 법선 벡터 시각화
    //Fragcolor = vec4(lightDir * 0.5 + 0.5, 1.0); // 빛 방향 시각화
    Fragcolor = vec4(result, 1.0); // 전달된 색상으로 출력
}