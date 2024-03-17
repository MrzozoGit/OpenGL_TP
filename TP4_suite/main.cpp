#include <cmath>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
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

struct Vertex2DUV {
    glm::vec2 position;
    glm::vec2 texture;
    ~Vertex2DUV()
    {
    }
};

glm::mat3 translate(float tx, float ty)
{
    return glm::mat3(
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(tx, ty, 1)
    );
}

glm::mat3 scale(float sx, float sy)
{
    return glm::mat3(
        glm::vec3(sx, 0, 0),
        glm::vec3(0, sy, 0),
        glm::vec3(0, 0, 1)
    );
}

glm::mat3 rotate(float a)
{
    return glm::mat3(
        glm::vec3(cos(a), sin(a), 0),
        glm::vec3(-sin(a), cos(a), 0),
        glm::vec3(0, 0, 1)
    );
}

const float pi = glm::radians(180.0f);

int main()
{
    auto ctx = p6::Context{{1280, 720, "TP3 EX1"}};
    ctx.maximize_window();

    // Charger les textures
    // 1: Charger l'image
    auto triforceTexture = p6::load_image_buffer("assets/textures/triforce.png");
    // 2: Générer un id de texture
    GLuint triforceTextureID;
    glGenTextures(1, &triforceTextureID);
    // 3: Binder la texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, triforceTextureID);
    // 4: Appliquer des filtres de texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 5: Envoyer les données à OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, triforceTexture.width(), triforceTexture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, triforceTexture.data());
    // 6: Debinder la texture
    glBindTexture(GL_TEXTURE_2D, 0);

    glm::mat3 triforceTextureMat = glm::mat3();

    // Charger les shaders et les compiler
    const p6::Shader shader = p6::load_shader(
        "shaders/text2D.vs.glsl",
        "shaders/text2D.fs.glsl"
    );

    /*********************************
     * INITIALIZATION
     *********************************/
    float rotation = 0;

    /*********************************
     * LE VERTEX BUFFER OBJECT *
     *********************************/

    // Création d'un seul VBO
    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    // Binding de ce VBO sur la cible GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Remplissage du VBO
    std::vector<Vertex2DUV> vertices;
    vertices.push_back(Vertex2DUV{glm::vec2(-1.f, -1.f), glm::vec2(0.f, 0.f)});
    vertices.push_back(Vertex2DUV{glm::vec2(1.f, -1.f), glm::vec2(1.f, 0.f)});
    vertices.push_back(Vertex2DUV{glm::vec2(0.f, 1.f), glm::vec2(0.5f, 1.f)});

    // glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2DUV), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /*********************************
     * LE VERTEX ARRAY OBJECT *
     *********************************/

    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Activation des attributs de vertex
    static constexpr GLuint vertex_attr_position = 0;
    glEnableVertexAttribArray(vertex_attr_position);
    static constexpr GLuint vertex_attr_uv = 1;
    glEnableVertexAttribArray(vertex_attr_uv);

    // Spécification des attributs de vertex
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(vertex_attr_position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DUV), (const GLvoid*)offsetof(Vertex2DUV, position));
    glVertexAttribPointer(vertex_attr_uv, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DUV), (const GLvoid*)offsetof(Vertex2DUV, texture));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Declare your infinite update loop.
    ctx.update = [&]() {
        // Nettoyer la fenêtre
        glClear(GL_COLOR_BUFFER_BIT);

        // Dessinez le disque
        glBindVertexArray(VAO);

        shader.use();
        int uModelMatrixId = glGetUniformLocation(shader.id(), "uModelMatrix");
        int uColorId       = glGetUniformLocation(shader.id(), "uColor");
        int uTextureId     = glGetUniformLocation(shader.id(), "uTexture");

        // Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, triforceTextureID);
        glUniform1i(uTextureId, 0);

        rotation += 0.01;

        glm::mat3 matrixT1 = rotate(rotation) * scale(.1, .1);
        glUniformMatrix3fv(uModelMatrixId, 1, GL_FALSE, glm::value_ptr(matrixT1));
        // glm::vec3 colorT1 = glm::vec3(.5, .1, rotation * .5 * .1);
        // glUniform3fv(uColorId, 1, glm::value_ptr(colorT1));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glm::mat3 matrixT2 = rotate(rotation * .5) * translate(.5, .5) * rotate(-rotation * 2) * scale(.1, .1);
        glUniformMatrix3fv(uModelMatrixId, 1, GL_FALSE, glm::value_ptr(matrixT2));
        // glm::vec3 colorT2 = glm::vec3(rotation * .5 * .4, .9, .1);
        // glUniform3fv(uColorId, 1, glm::value_ptr(colorT2));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // glimac::bind_default_shader();
        glBindVertexArray(0);
    };

    // Should be done last. It starts the infinite loop.
    ctx.start();

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteTextures(1, &triforceTextureID);
}