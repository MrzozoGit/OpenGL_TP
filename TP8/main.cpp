#include <cmath>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <vector>
#include "../src-common/FreeflyCamera.hpp"
#include "../src-common/glimac/common.hpp"
#include "../src-common/glimac/sphere_vertices.hpp"
#include "glm/fwd.hpp"
#include "p6/p6.h"

const float   pi = glm::radians(180.0f);
bool          KEY_STATUS_Z, KEY_STATUS_S, KEY_STATUS_Q, KEY_STATUS_D = false;
FreeflyCamera camera;
float         speed = .1f;

struct ShaderProgram {
    p6::Shader m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uKd;
    GLint uKs;
    GLint uShininess;
    GLint uLightPos_vs;
    GLint uLightIntensity;

    ShaderProgram()
        : m_Program{p6::load_shader("shaders/3D.vs.glsl", "shaders/pointLight.fs.glsl")}
    {
        uMVPMatrix      = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix       = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix   = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
        uKd             = glGetUniformLocation(m_Program.id(), "uKd");
        uKs             = glGetUniformLocation(m_Program.id(), "uKs");
        uShininess      = glGetUniformLocation(m_Program.id(), "uShininess");
        uLightPos_vs    = glGetUniformLocation(m_Program.id(), "uLightPos_vs");
        uLightIntensity = glGetUniformLocation(m_Program.id(), "uLightIntensity");
    }
};

void moveCamera()
{
    if (KEY_STATUS_Z)
        camera.moveFront(speed);
    if (KEY_STATUS_S)
        camera.moveFront(-speed);
    if (KEY_STATUS_Q)
        camera.moveLeft(speed);
    if (KEY_STATUS_D)
        camera.moveLeft(-speed);
}

int main()
{
    auto ctx = p6::Context{{1280, 720, "TP3 EX1"}};
    ctx.maximize_window();

    // Initialisation du random
    srand(time(NULL));

    // Initialiser le shader
    ShaderProgram shaderProgram;

    ctx.mouse_moved = [&](p6::MouseMove event) {
        const float sensitivity = 150.f;
        camera.rotateLeft(-event.delta.x * sensitivity); // Inversion de X pour une rotation intuitive
        camera.rotateUp(event.delta.y * sensitivity);
    };

    ctx.key_pressed = [&](p6::Key key) {
        const float speed = .5f;
        if (key.logical == "z")
            KEY_STATUS_Z = true;
        else if (key.logical == "s")
            KEY_STATUS_S = true;
        else if (key.logical == "q")
            KEY_STATUS_Q = true;
        else if (key.logical == "d")
            KEY_STATUS_D = true;
    };
    ctx.key_released = [&](p6::Key key) {
        const float speed = .5f;
        if (key.logical == "z")
            KEY_STATUS_Z = false;
        else if (key.logical == "s")
            KEY_STATUS_S = false;
        else if (key.logical == "q")
            KEY_STATUS_Q = false;
        else if (key.logical == "d")
            KEY_STATUS_D = false;
    };

    // Générer les vertex de la sphere
    int                                    Nlat           = 16;   // Nombre de triangles en latitude
    int                                    Nlong          = 8;    // Nombre de triangles en longitude
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

        // std::cout << "[" << i << "] x: " << moons[i].x << ", y: " << moons[i].y << ", z: " << moons[i].z << std::endl;
    }

    std::vector<glm::vec3> uKdList;
    std::vector<glm::vec3> uKsList;
    std::vector<float>     uShininessList;
    for (int i = 0; i < nbOfMoons + 1; i++)
    {
        uKdList.push_back(glm::vec3(std::rand() / (float)RAND_MAX, std::rand() / (float)RAND_MAX, std::rand() / (float)RAND_MAX));
        uKsList.push_back(glm::vec3(std::rand() / (float)RAND_MAX, std::rand() / (float)RAND_MAX, std::rand() / (float)RAND_MAX));
        uShininessList.push_back((std::rand() / (float)1000));
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
        moveCamera();
        camera.computeDirectionVectors();

        // Use earth shader
        shaderProgram.m_Program.use();

        // Instantiate global matrix
        glm::mat4       viewMatrix       = camera.getViewMatrix();
        const glm::mat4 globalProjMatrix = glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), .1f, 100.f);
        const glm::mat4 globalMVMatrix   = viewMatrix;

        // Earth transformation matrix
        glm::mat4 earthMVMatrix = glm::rotate(globalMVMatrix, ctx.time(), {0.f, 5.f, 0.f});
        earthMVMatrix           = glm::rotate(earthMVMatrix, glm::radians(180.0f), {1.f, 0.f, 1.f});

        // Define matrix in earth uniform variables
        glUniformMatrix4fv(shaderProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(earthMVMatrix));
        glUniformMatrix4fv(shaderProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(earthMVMatrix))));
        glUniformMatrix4fv(shaderProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(globalProjMatrix * earthMVMatrix));

        glUniform3fv(shaderProgram.uKd, 1, glm::value_ptr(uKdList[0]));
        glUniform3fv(shaderProgram.uKs, 1, glm::value_ptr(uKsList[0]));
        glUniform1f(shaderProgram.uShininess, uShininessList[0]);
        // ;
        glm::vec4 lightPosition_world = glm::vec4(1.f, 1.f, 1.f, 1.0); // Position de la lumière en espace monde
        glm::vec3 lightPosition_view  = glm::vec3(viewMatrix * lightPosition_world);
        glUniform3fv(shaderProgram.uLightPos_vs, 1, glm::value_ptr(lightPosition_view));
        glUniform3fv(shaderProgram.uLightIntensity, 1, glm::value_ptr(glm::vec3(1.f, 1.f, 1.f)));

        // Draw earth
        glDrawArrays(GL_TRIANGLES, 0, size(sphereVertices));

        // Lunes
        for (int i = 0; i < moons.size(); i++)
        {
            // Moons transformation matrix
            glm::mat4 moonMVMatrix = glm::rotate(globalMVMatrix, ctx.time(), {0.f, 1.f, 0.f});
            moonMVMatrix           = glm::translate(moonMVMatrix, moons[i]);
            moonMVMatrix           = glm::rotate(moonMVMatrix, ctx.time(), {0.f, -1.f, 0.f});
            moonMVMatrix           = glm::scale(moonMVMatrix, glm::vec3{0.2f});

            // Define matrix in moon uniform variables
            glUniformMatrix4fv(shaderProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(moonMVMatrix));
            glUniformMatrix4fv(shaderProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(moonMVMatrix))));
            glUniformMatrix4fv(shaderProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(globalProjMatrix * moonMVMatrix));
            glUniform3fv(shaderProgram.uKd, 1, glm::value_ptr(uKdList[i + 1]));
            glUniform3fv(shaderProgram.uKs, 1, glm::value_ptr(uKsList[i + 1]));
            glUniform1f(shaderProgram.uShininess, uShininessList[i + 1]);

            // Draw moon
            glDrawArrays(GL_TRIANGLES, 0, size(sphereVertices));
        }

        glBindVertexArray(0);
    };

    // Should be done last. It starts the infinite loop.
    ctx.start();

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

// https://julesfouchy.github.io/Learn--OpenGL/TP6/les-fonctions-sph%C3%A8re-et-cone