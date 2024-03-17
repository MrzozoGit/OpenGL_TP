#version 330 core

in vec3 vFragColor;
in vec2 originalPosition;

out vec4 fFragColor;

void main() {
  float alpha = 1.0f;
  float beta = 15.0f; 
  vec2 center = vec2(0, 0);

  float distance = distance(originalPosition, center);

  float attenuation = alpha * exp(-beta * distance * distance);
  // float formule1 = length(fract(5.0 * originalPosition));
  // float formule2 = length(abs(fract(5.0 * originalPosition) * 2.0 - 1.0));
  // float formule3 = mod(floor(10.0 * originalPosition.x) + floor(10.0 * originalPosition.y), 2.0);
  // float formule4 = smoothstep(0.3, 0.32, length(fract(5.0 * originalPosition) - 0.5));
  // float formule5 = smoothstep(0.4, 0.5, max(abs(fract(8.0 * originalPosition.x - 0.5 * mod(floor(8.0 * originalPosition.y), 2.0)) - 0.5), abs(fract(8.0 * originalPosition.y) - 0.5)));

  // vec3 modifiedColor = vFragColor *formule3 * formule4;
  vec3 modifiedColor = vFragColor * attenuation;

  fFragColor = vec4(modifiedColor, 1.);
}