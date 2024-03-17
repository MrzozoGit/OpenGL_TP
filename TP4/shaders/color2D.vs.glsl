#version 330 core

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;

out vec3 vFragColor;
out vec2 originalPosition;

mat3 translate(float tx, float ty)
{
  return mat3(
    vec3(1, 0, 0),
    vec3(0, 1, 0),
    vec3(tx, ty, 1)
  );
}

mat3 scale(float sx, float sy)
{
  return mat3(
    vec3(sx, 0, 0),
    vec3(0, sy, 0),
    vec3(0, 0, 1)
  );
}

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

  // Scaling
  mat3 Ms = scale(.5, .5);
  transformed = (Ms * vec3(transformed, 1.)).xy;

  // Rotation
  mat3 Mr = rotate(radians(45));
  transformed = (Mr * vec3(transformed, 1.)).xy;

  // Translation
  mat3 Mt = translate(.5, 0);
  transformed = (Mt * vec3(transformed, 1.)).xy;

  gl_Position = vec4(transformed, 0., 1.);

  originalPosition = aVertexPosition;

  vFragColor = aVertexColor;
}