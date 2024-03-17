#version 330 core

uniform mat3 uModelMatrix;
uniform vec3 uColor;

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec2 aVertexTexture;
layout(location = 2) in vec3 aVertexColor;

out vec2 vFragTexture;
out vec3 vFragColor;

mat3 rotate(float a)
{
  return mat3(
    vec3(cos(a), sin(a), 0),
    vec3(-sin(a), cos(a), 0),
    vec3(0, 0, 1)
  );
}

void main() {
  vec2 transformed = aVertexPosition;

  transformed = (uModelMatrix * vec3(transformed, 1.)).xy;

  gl_Position = vec4(transformed, 0., 1.);
  vFragTexture = aVertexTexture;
  vFragColor = uColor;
}