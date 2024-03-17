
#version 330

in vec2 vTexCoords;
in vec3 vPosition_vs; // Position du sommet transformée dans l'espace View (vs)
in vec3 vNormal_vs; // Normale du sommet transformée dans l'espace View (vs)

uniform sampler2D uTexture; 

out vec4 fFragColor;

void main() {
    fFragColor = (texture(uTexture, vTexCoords));
}
