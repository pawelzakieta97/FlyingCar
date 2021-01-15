#pragma once
#include "Light.h"
#include <glm/glm.hpp>
class PointLight :
    public Light
{
public:
    PointLight(glm::vec3 pos, glm::vec3 intensity) {
        position = pos;
        this->intensity = intensity;
    }
};

