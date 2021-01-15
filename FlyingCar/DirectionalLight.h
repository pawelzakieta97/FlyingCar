#pragma once
#include "Light.h"
class DirectionalLight :
    public Light
{
public:
    glm::vec3 direction;
    DirectionalLight(glm::vec3 direction, glm::vec3 intensity) {
        this -> direction = glm::normalize(direction);
        this->intensity = intensity;
    }
};

