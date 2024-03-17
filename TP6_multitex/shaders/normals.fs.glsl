
#version 330

in vec2 vTexCoords;
in vec3 vPosition_vs; // Position du sommet transformée dans l'espace View (vs)
in vec3 vNormal_vs; // Normale du sommet transformée dans l'espace View (vs)

out vec4 fFragColor;

void main() {
    vec3 normalizedNormal = normalize(vNormal_vs);
    fFragColor = vec4(normalizedNormal, 1.);
}
