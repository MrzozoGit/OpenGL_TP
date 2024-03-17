
#version 330

in vec2 vTexCoords;
in vec3 vPosition_vs; // Position du sommet transformée dans l'espace View (vs)
in vec3 vNormal_vs; // Normale du sommet transformée dans l'espace View (vs)

uniform sampler2D uEarthTexture;
uniform sampler2D uCloudTexture;

out vec4 fFragColor;

void main() {
    vec4 earthColor = (texture(uEarthTexture, vTexCoords));
    vec4 cloudColor = (texture(uCloudTexture, vTexCoords));
    fFragColor = earthColor + cloudColor;
}
