#pragma once
#include "shader.h"
#include "includes.h"

class Material{
public:
    Shader* shader;
    glm::vec3 colour = glm::vec3(0.5f);
    float shininess;

    Material(){}
    Material(Shader* sh){
        shader = sh;
        colour = glm::vec3(1.0f);
        shininess = 0.1f;
    }
    void Bind(){
        shader->use();
        shader->setVec3("u_Colour", colour);
        shader->setFloat("u_Shininess", shininess);
    }
};

//     if(shaders.size() == 0){
//         shaders.reserve(2);
//         shaders.emplace_back(Shader("../shaders/cubeVert.txt", "../shaders/cubeFrag.txt"));
//         shaders.emplace_back(Shader("../shaders/lightVert.txt", "../shaders/lightFrag.txt"));
//     }
//     Shader GetShaderFromType(ShaderType t)
// {
//     if(t == ShaderType::Lighting)
//         return shaders[1];
//     if(t == ShaderType::Regular){
//         return shaders[0];
//     }
//     return shaders[0]; 
// }