#include "globals.h"
#include "mesh.h"
#include "material.h"
#include "camera.h"
#include <random>

enum class ObjectDirection {
    UP,
    DOWN,
    ONGROUND,
    NONE
};
class Object {
public:
    Mesh mesh;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale = glm::vec3(1.0f);
    
    float acceleration = 50.0f;
    float decceleration = 20.0f;
    float maxVelocity = 30.0f;
    float velocity = 0.0f;
    
    ObjectDirection moving = ObjectDirection::NONE;
    glm::vec3 direction = glm::vec3(0.0f);
    
    Object(glm::vec3 pos);
    void GravityEffect();
    void Draw(Renderer& renderer, Camera& camera, glm::vec3 lightPos);
    void Update(Camera& camera, glm::vec3 lightPos);
    glm::mat4 GetModelMatrix();
    
};

class ObjectCollection {
    public:
    std::vector<Object*> objects;
    std::vector<glm::vec3> objectPositions;

    std::random_device rd;

    ObjectCollection(){}

    void Add(Object* obj);
    void DrawAll(Renderer& renderer, Camera camera, glm::vec3& lightPosition);
    void RandomiseObjectPositions(glm::vec2 bounds);
    void RandomiseObjectColours();
    void RandomiseScale();
};