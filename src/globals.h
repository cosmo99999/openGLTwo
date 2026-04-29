#pragma once
class Globals{
public:
    const unsigned int SCR_WIDTH = 1280;
    const unsigned int SCR_HEIGHT = 960;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool firstMouse = true;
    bool mouseEnabled = false;
    float ambientLight = 0.1f;
    float backColour[4] = {0.5f, 0.5f, 0.5f, 1.0f};
};
extern Globals globals;
