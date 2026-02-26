#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../third_party/stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
class Globals{
public:
    const unsigned int SCR_WIDTH = 1280;
    const unsigned int SCR_HEIGHT = 960;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
};

Globals globals = Globals();