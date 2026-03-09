#include "object.h"

extern Globals globals;

void ObjectCollection::Add(Object *obj)
{
    objects.emplace_back(obj);
    objectPositions.emplace_back(obj->position);
}

void ObjectCollection::DrawAll(Renderer& renderer, Camera camera, glm::vec3 &lightPosition)
{
    for(Object* obj : objects){
        obj->Draw(renderer, camera, lightPosition);
    }
}

void ObjectCollection::RandomiseObjectPositions(glm::vec2 bounds)
{
    std::mt19937 engine(rd()); 
    std::uniform_real_distribution<float> dist(bounds.x, bounds.y); 
    for(Object* obj : objects){
        obj->position.x = dist(engine);
        obj->position.y = dist(engine);
        obj->position.z = dist(engine); 
    }
}
void ObjectCollection::RandomiseObjectColours()
{
    std::mt19937 engine(rd()); 
    std::uniform_real_distribution<float> dist(0.0f, 1.0f); 
    for(int i = 1; i < objects.size(); i++){
        auto* obj = objects[i];
        obj->mesh.material.colour.r = dist(engine);
        obj->mesh.material.colour.g = dist(engine);
        obj->mesh.material.colour.b = dist(engine); 
    }
}
void ObjectCollection::RandomiseScale()
{
    std::mt19937 engine(rd()); 
    std::uniform_real_distribution<float> dist(5.0f, 15.0f); 
    for(int i = 0; i < objects.size(); i++){
        Object* obj = objects[i];
        obj->scale = glm::vec3(dist(engine));
    }
}

Object::Object(glm::vec3 pos){
    position = pos;
}
void Object::GravityEffect()
{
    if(moving == ObjectDirection::NONE){
        if(position.y > 0){
            moving = ObjectDirection::DOWN;
        }
    }
    
    if(moving == ObjectDirection::DOWN){
        if(position.y > 0){
            velocity += acceleration * globals.deltaTime;
        }
        if(position.y <= 0){
            maxVelocity *= 0.85;
            velocity = -maxVelocity;
            moving = ObjectDirection::UP;
        }
    }
    if(moving == ObjectDirection::UP){
        velocity -= decceleration * globals.deltaTime;
        if(velocity <= 0){
            moving = ObjectDirection::DOWN;
        }
    }
    if(maxVelocity < 1.0f){
        moving = ObjectDirection::ONGROUND;
        maxVelocity = 20.0f;
        velocity = 0;
    }
    if(moving != ObjectDirection::ONGROUND)
        position += direction * velocity * globals.deltaTime;
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
