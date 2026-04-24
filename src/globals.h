#pragma once
class Globals{
public:
    const unsigned int SCR_WIDTH = 1280;
    const unsigned int SCR_HEIGHT = 960;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
};
extern Globals globals;
