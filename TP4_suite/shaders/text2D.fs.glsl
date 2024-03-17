#version 330 core

uniform sampler2D uTexture; 

in vec2 vFragTexture;
in vec3 vFragColor;

out vec4 fFragColor;

void main() {
  // fFragColor = vec4(vFragColor, 1.);
  fFragColor = (texture(uTexture, vFragTexture));
}