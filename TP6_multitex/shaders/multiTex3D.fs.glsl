
#version 330

in vec2 vTexCoords;
in vec3 vPosition_vs; // Position du sommet transformée dans l'espace View (vs)
in vec3 vNormal_vs; // Normale du sommet transformée dans l'espace View (vs)

uniform sampler2D uTexture;
uniform sampler2D uTextureSecondary;

out vec4 fFragColor;

void main() {
    vec4 color = (texture(uTexture, vTexCoords));
    vec4 colorSecondary = (texture(uTextureSecondary, vTexCoords));
    fFragColor = color+colorSecondary;
}
