#include "FreeflyCamera.hpp"
#include <glm/gtc/matrix_transform.hpp>
// #include <iostream>

const float pi = glm::pi<float>();

FreeflyCamera::FreeflyCamera()
    : m_Position(0.f, 0.f, 0.f), m_Phi(pi), m_FrontVector(0.f, 0.f, -1.f)
{
    computeDirectionVectors();
}

void FreeflyCamera::computeDirectionVectors()
{
    m_FrontVector = glm::vec3(glm::cos(m_Theta) * glm::sin(m_Phi), glm::sin(m_Theta), glm::cos(m_Theta) * glm::cos(m_Phi));
    m_LeftVector  = glm::vec3(glm::sin(m_Phi + (pi / 2)), 0, glm::cos(m_Phi + (pi / 2)));
    m_UpVector    = glm::cross(m_FrontVector, m_LeftVector);
}

void FreeflyCamera::moveLeft(float t)
{
    m_Position += t * m_LeftVector;
}

void FreeflyCamera::moveFront(float t)
{
    m_Position += t * m_FrontVector;
}

void FreeflyCamera::rotateLeft(float degrees)
{
    m_Phi += glm::radians(degrees);
}

void FreeflyCamera::rotateUp(float degrees)
{
    m_Theta += glm::radians(degrees);
}

glm::mat4 FreeflyCamera::getViewMatrix() const
{
    return glm::lookAt(m_Position, m_FrontVector + m_Position, m_UpVector);
}
