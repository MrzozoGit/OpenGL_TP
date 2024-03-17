#include <cmath>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <vector>
#include "glm/fwd.hpp"
#include "p6/p6.h"

struct Vertex2DColor {
    glm::vec2 position;
    glm::vec3 color;
    ~Vertex2DColor()
    {
    }
};

const float pi = glm::radians(180.0f);

// Generate the disk vertices
std::vector<Vertex2DColor> generateDisk(int N, float radius)
{
    std::vector<Vertex2DColor> vertices;

    // Add the center of the disk
    vertices.push_back(Vertex2DColor{glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)});

    for (int i = 0; i <= N; i++)
    {
        float theta = 2.0f * pi * i / N;
        vertices.push_back(Vertex2DColor{
            glm::vec2(radius * glm::cos(theta), radius * glm::sin(theta)),
            glm::vec3(0.0f, 1.0f, 0.0f)
        });
    }

    std::cout << vertices.size() << std::endl;

    return vertices;
}

int main()
{
    auto ctx = p6::Context{{1280, 720, "TP3 EX3"}};
    ctx.maximize_window();

    // Charger les shaders et les compiler
    const p6::Shader shader = p6::load_shader(
        "shaders/tex2D.vs.glsl",
        "shaders/tex2D.fs.glsl"
    );

    // Générer les vertex du disque
    int   N            = 3;    // Nombre de triangles
    float radius       = 1.0f; // Rayon du disque
    auto  diskVertices = generateDisk(N, radius);

    // Créer un VBO
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Envoyer les données des vertex au buffer
    glBufferData(GL_ARRAY_BUFFER, diskVertices.size() * sizeof(Vertex2DColor), diskVertices.data(), GL_STATIC_DRAW);

    // Débinder le VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Créer l'IBO
    GLuint IBO;
    glGenBuffers(1, &IBO);

    // Binder l'IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Tableau d'indices des sommets à dessiner
    std::vector<uint32_t> indices;
    for (int i = 0; i < N; ++i)
    {
        indices.push_back(0);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
    }

    std::cout << indices.size() << std::endl;

    // Remplir l'IBO avec les indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (N * 3 * sizeof(uint32_t)), indices.data(), GL_STATIC_DRAW);

    // Débinder l'IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Créer un VAO
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Binder l'IBO sur GL_ELEMENT_ARRAY_BUFFER pour l'enregistrer dans le VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Activer l'attribut position
    static constexpr GLuint vertex_attr_position = 0;
    glEnableVertexAttribArray(vertex_attr_position);

    // Activer l'attribut couleur
    static constexpr GLuint vertex_attr_color = 1;
    glEnableVertexAttribArray(vertex_attr_color);

    // Spécification des attributs de vertex
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(vertex_attr_position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)offsetof(Vertex2DColor, position));
    glVertexAttribPointer(vertex_attr_color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)offsetof(Vertex2DColor, color));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Débinder le VAO pour éviter de le modifier par erreur
    glBindVertexArray(0);

    // Declare your infinite update loop.
    ctx.update = [&]() {
        // Nettoyer la fenêtre
        glClear(GL_COLOR_BUFFER_BIT);

        // Dessinez le disque
        glBindVertexArray(VAO); // Liez le VAO avant de dessiner
        shader.use();
        // glDrawArrays(GL_TRIANGLES, 0, (GLsizei)diskVertices.size());
        glDrawElements(GL_TRIANGLES, N * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // Désactivez le VAO après le dessin
    };

    ctx.start();

    // Libérer le VBO et le VAO
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}