#include <cmath>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <vector>
#include "../src-common/TrackballCamera.hpp"
#include "../src-common/glimac/common.hpp"
#include "../src-common/glimac/sphere_vertices.hpp"
#include "glm/fwd.hpp"
#include "p6/p6.h"

const float pi = glm::radians(180.0f);

struct EarthProgram {
    p6::Shader m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uEarthTexture;
    GLint uCloudTexture;

    EarthProgram()
        : m_Program{p6::load_shader("shaders/3D.vs.glsl", "shaders/multiTex3D.fs.glsl")}
    {
        uMVPMatrix    = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix     = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
        uEarthTexture = glGetUniformLocation(m_Program.id(), "uEarthTexture");
        uCloudTexture = glGetUniformLocation(m_Program.id(), "uCloudTexture");
    }
};

struct MoonProgram {
    p6::Shader m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uTexture;

    MoonProgram()
        : m_Program{p6::load_shader("shaders/3D.vs.glsl", "shaders/tex3D.fs.glsl")}
    {
        uMVPMatrix    = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix     = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
        uTexture      = glGetUniformLocation(m_Program.id(), "uTexture");
    }
};

int main()
{
    auto ctx = p6::Context{{1280, 720, "TP3 EX1"}};
    ctx.maximize_window();

    TrackballCamera camera;
    ctx.mouse_moved = [&](p6::MouseMove event) {
        std::cout << event.delta.x << std::endl;
        const float sensitivity = 150.f; // Ajustez à votre convenance
        camera.rotateLeft(event.delta.x * sensitivity);
        camera.rotateUp(-event.delta.y * sensitivity); // Inversion de Y pour une rotation intuitive
    };

    ctx.mouse_scrolled = [&](p6::MouseScroll event) {
        camera.moveFront(event.dy);
    };

    // Charger les shaders
    EarthProgram earthProgram{};
    MoonProgram  moonProgram{};

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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, moonTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, moonTex.width(), moonTex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, moonTex.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    auto   cloudTex = p6::load_image_buffer("assets/textures/CloudMap.jpg");
    GLuint cloudTexId;
    glGenTextures(1, &cloudTexId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cloudTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cloudTex.width(), cloudTex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, cloudTex.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    // Generate moons
    int                    nbOfMoons = 32;
    std::vector<glm::vec3> moons;
    for (int i = 0; i < nbOfMoons; i++)
    {
        moons.push_back(glm::sphericalRand(2.f));
        std::cout << "[" << i << "] x: " << moons[i].x << ", y: " << moons[i].y << ", z: " << moons[i].z << std::endl;
    }

    /***************************
     * INFINITE UPDATE LOOP *
     ***************************/
    ctx.update = [&]() {
        // Clear the window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Binding the VAO
        glBindVertexArray(VAO);

        glfwPollEvents();

        // Use earth shader
        earthProgram.m_Program.use();

        // Define texture ID in earth uniform variables
        glUniform1i(earthProgram.uEarthTexture, 0);
        glUniform1i(earthProgram.uCloudTexture, 1);

        // Instantiate global matrix
        glm::mat4       viewMatrix       = camera.getViewMatrix();
        const glm::mat4 globalProjMatrix = glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), .1f, 100.f);
        const glm::mat4 globalMVMatrix   = viewMatrix;

        // Earth transformation matrix
        glm::mat4 earthMVMatrix = glm::rotate(globalMVMatrix, ctx.time(), {0.f, 5.f, 0.f});
        earthMVMatrix           = glm::rotate(earthMVMatrix, glm::radians(180.0f), {1.f, 0.f, 1.f});

        // Define matrix in earth uniform variables
        glUniformMatrix4fv(earthProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(earthMVMatrix));
        glUniformMatrix4fv(earthProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(earthMVMatrix))));
        glUniformMatrix4fv(earthProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(globalProjMatrix * earthMVMatrix));

        // Earth textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, earthTexId);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cloudTexId);

        // Draw earth
        glDrawArrays(GL_TRIANGLES, 0, size(sphereVertices));

        // Unbind cloud texture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Use moon shader
        moonProgram.m_Program.use();

        // Define texture ID in moon uniform variables
        glUniform1i(moonProgram.uTexture, 0);

        // Lunes
        for (int i = 0; i < moons.size(); i++)
        {
            // Moons transformation matrix
            glm::mat4 moonMVMatrix = glm::rotate(globalMVMatrix, ctx.time(), {0.f, 1.f, 0.f});
            moonMVMatrix           = glm::translate(moonMVMatrix, moons[i]);
            moonMVMatrix           = glm::rotate(moonMVMatrix, ctx.time(), {0.f, -1.f, 0.f});
            moonMVMatrix           = glm::scale(moonMVMatrix, glm::vec3{0.2f});

            // Define matrix in earth uniform variables
            glUniformMatrix4fv(moonProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(moonMVMatrix));
            glUniformMatrix4fv(moonProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(moonMVMatrix))));
            glUniformMatrix4fv(moonProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(globalProjMatrix * moonMVMatrix));

            // Earth textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, moonTexId);

            // Draw moon
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