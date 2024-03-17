
#version 330

in vec2 vTexCoords;
in vec3 vPosition_vs; // Position du sommet transformée dans l'espace View (vs)
in vec3 vNormal_vs; // Normale du sommet transformée dans l'espace View (vs)

// Matériau de l'objet en cours de dessin
uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

// Infos sur la lumière
uniform vec3 uLightDir_vs; // vs = view space, il faut multiplier par la View Matrix
uniform vec3 uLightIntensity;

out vec4 fFragColor;

vec3 blinnPhong() {
    vec3 N = normalize(vNormal_vs);
    vec3 L = normalize(uLightDir_vs);
    vec3 V = normalize(-vPosition_vs);

    vec3 H = normalize(L + V);

    float diff = max(dot(N, L), 0.0);

    float spec = pow(max(dot(N, H), 0.0), uShininess);

    vec3 color = uLightIntensity * (uKd * diff + uKs * spec);

    return color;
}

void main() {
    fFragColor = vec4(blinnPhong(), 1);
}
