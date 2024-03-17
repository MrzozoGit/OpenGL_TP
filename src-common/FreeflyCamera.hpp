#pragma once

#include <glm/glm.hpp>

class FreeflyCamera {
private:
    glm::vec3 m_Position;     // Position de la caméra
    float     m_Phi, m_Theta; // Coordonnées sphériques du vecteur F
    glm::vec3 m_FrontVector;  // Vecteur F
    glm::vec3 m_LeftVector;   // Vecteur L
    glm::vec3 m_UpVector;     // Vecteur U

public:
    // Constructeur
    FreeflyCamera();
    void      computeDirectionVectors();
    void      moveLeft(float t);
    void      moveFront(float t);
    void      rotateLeft(float degrees);
    void      rotateUp(float degrees);
    glm::mat4 getViewMatrix() const;
};