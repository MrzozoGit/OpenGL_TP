#include <cmath>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <vector>
#include "../src-common/glimac/common.hpp"
#include "../src-common/glimac/sphere_vertices.hpp"
#include "glm/fwd.hpp"
#include "p6/p6.h"

const float pi = glm::radians(180.0f);

int main()
{
    auto ctx = p6::Context{{1280, 720, "TP3 EX1"}};
    ctx.maximize_window();

    // Charger les shaders et les compiler
    const p6::Shader shader = p6::load_shader(
        "shaders/3D.vs.glsl",
        "shaders/tex3D.fs.glsl"
    );

    // Générer les vertex de la sphere
    int                                    Nlat           = 32;   // Nombre de triangles en latitude
    int                                    Nlong          = 16;   // Nombre de triangles en longitude
    float                                  radius         = 1.0f; // Rayon
    const std::vector<glimac::ShapeVertex> sphereVertices = glimac::sphere_vertices(radius, Nlat, Nlong);

    /***************************
     * VERTEX BUFFER OBJECT *
     ***************************/
    // Créer un VBO
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Envoyer les données des vertex au buffer
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(glimac::ShapeVertex), sphereVertices.data(), GL_STATIC_DRAW);

    // Débinder le VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /***************************
     * VERTEX ARRAY OBJECT *
     ***************************/
    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    /***************************
     * VBO ATTRIBUTES *
     ***************************/
    // Activation des attributs de vertex
    static constexpr GLuint vertex_attr_position = 0;
    glEnableVertexAttribArray(vertex_attr_position);
    static constexpr GLuint vertex_attr_normal = 1;
    glEnableVertexAttribArray(vertex_attr_normal);
    static constexpr GLuint vertex_attr_texcoords = 2;
    glEnableVertexAttribArray(vertex_attr_texcoords);

    // Spécification des attributs de vertex
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(vertex_attr_position, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, position));
    glVertexAttribPointer(vertex_attr_normal, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, normal));
    glVertexAttribPointer(vertex_attr_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, texCoords));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Enable depth for 3D
    glEnable(GL_DEPTH_TEST);

    // Generate moons
    int                    nbOfMoons = 32;
    std::vector<glm::vec3> moons;
    for (int i = 0; i < nbOfMoons; i++)
    {
        moons.push_back(glm::sphericalRand(2.f));
        std::cout << "[" << i << "] x: " << moons[i].x << ", y: " << moons[i].y << ", z: " << moons[i].z << std::endl;
    }

    // Bind textures
    auto   earthTex = p6::load_image_buffer("assets/textures/EarthMap.jpg");
    GLuint earthTexId;
    glGenTextures(1, &earthTexId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, earthTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, earthTex.width(), earthTex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, earthTex.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    auto   moonTex = p6::load_image_buffer("assets/textures/MoonMap.jpg");
    GLuint moonTexId;
    glGenTextures(1, &moonTexId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, moonTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, moonTex.width(), moonTex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, moonTex.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    /***************************
     * INFINITE UPDATE LOOP *
     ***************************/
    ctx.update = [&]() {
        // Nettoyer la fenêtre
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);

        // Activer les shaders
        shader.use();

        // Variables uniformes
        int uMVPMatrix    = glGetUniformLocation(shader.id(), "uMVPMatrix");
        int uMVMatrix     = glGetUniformLocation(shader.id(), "uMVMatrix");
        int uNormalMatrix = glGetUniformLocation(shader.id(), "uNormalMatrix");
        int uTextureId    = glGetUniformLocation(shader.id(), "uTexture");

        // Texture earth
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, earthTexId);
        glUniform1i(uTextureId, 0);

        // Planete
        glm::mat4 ProjMatrix   = glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), .1f, 100.f);
        glm::mat4 MVMatrix     = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -5.f));
        MVMatrix               = glm::rotate(MVMatrix, ctx.time(), {0.f, -5.f, 0.f});
        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glm::mat4 MVPMatrix    = ProjMatrix * MVMatrix;
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glDrawArrays(GL_TRIANGLES, 0, size(sphereVertices));

        // Texture moons
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, moonTexId);
        glUniform1i(uTextureId, 1);

        // Lunes
        for (int i = 0; i < moons.size(); i++)
        {
            ProjMatrix = glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), .1f, 100.f);
            MVMatrix   = glm::translate(glm::mat4{1.f}, glm::vec3(0.f, 0.f, -5.f));
            MVMatrix   = glm::rotate(MVMatrix, ctx.time(), {0.f, 1.f, 0.f});
            MVMatrix   = glm::translate(MVMatrix, moons[i]);
            MVMatrix   = glm::rotate(MVMatrix, ctx.time(), {0.f, -1.f, 0.f});
            // MVMatrix     = glm::rotate(MVMatrix, ctx.time(), {1.f, 1.f, 1.f});
            MVMatrix     = glm::scale(MVMatrix, glm::vec3{0.2f});
            NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
            MVPMatrix    = ProjMatrix * MVMatrix;
            glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
            glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
            glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
            glDrawArrays(GL_TRIANGLES, 0, size(sphereVertices));
        }

        glBindVertexArray(0);
    };

    // Should be done last. It starts the infinite loop.
    ctx.start();

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteTextures(1, &earthTexId);
}

// https://julesfouchy.github.io/Learn--OpenGL/TP6/les-fonctions-sph%C3%A8re-et-cone