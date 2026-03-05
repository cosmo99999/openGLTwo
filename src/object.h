#include "globals.h"
#include "mesh.h"
#include "material.h"
#include "camera.h"

class Object {
public:
    Mesh mesh;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale = glm::vec3(1.0f);
    
    Object(glm::vec3 pos);
    void Draw(Renderer& renderer, Camera& camera, glm::vec3 lightPos);
    void Update(Camera& camera, glm::vec3 lightPos);
    glm::mat4 GetModelMatrix();

};

class ObjectCollection {
public:
    std::vector<Object*> objects;
    std::vector<glm::vec3> objectPositions;
    int dASize = 0;
    ObjectCollection(){}

    void Add(Object* obj);
    void DrawAll(Camera camera, glm::vec3& lightPosition);
};