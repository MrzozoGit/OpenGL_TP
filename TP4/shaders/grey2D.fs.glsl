#version 330 core

in vec3 vFragColor;

out vec4 fFragColor;

void main() {
  float grey = (vFragColor.x + vFragColor.y + vFragColor.z) / 3.0;
  fFragColor = vec4(grey, grey, grey, 1.);
}