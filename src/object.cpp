#include "object.h"

extern Globals globals;

void ObjectCollection::Add(Object *obj)
{
    objects.emplace_back(obj);
    objectPositions.emplace_back(obj->position);
}

void ObjectCollection::DrawAll(Camera camera, glm::vec3 &lightPosition)
{
    // for(Object* obj : objects){
    //     obj->Draw(camera, lightPosition);
    // }
}

Object::Object(glm::vec3 pos){
    position = pos;
}
glm::mat4 Object::GetModelMatrix()
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, {1,0,0});
    model = glm::scale(model, scale);
    return model;
}

void Object::Draw(Renderer &renderer, Camera& camera, glm::vec3 lightpos)
{
    Update(camera, lightpos);
    mesh.Draw(renderer, camera, GetModelMatrix());
}

void Object::Update(Camera &camera, glm::vec3 lightPos)
{
    mesh.material.shader->use();
    mesh.material.shader->setVec3("objectColor", mesh.material.colour);
    mesh.material.shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    mesh.material.shader->setVec3("lightPos", lightPos);
    mesh.material.shader->setVec3("viewPos", camera.Position);
}
