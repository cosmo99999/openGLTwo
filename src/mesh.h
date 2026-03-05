#include "material.h"
#include "vertexArray.h"
#include "indexBuffer.h"
#include "renderer.h"
#include "camera.h"

class Mesh {
public:
    VertexArray* va = nullptr;
    IndexBuffer* ib = nullptr;
    VertexBuffer* vb = nullptr; 
    Material material;

    Mesh(){}
    Mesh(Shader* sh, IndexBuffer* indexBuffer, VertexArray* vertexArray, VertexBuffer* vertexBuffer){
        material = Material(sh);
        va = vertexArray;
        ib = indexBuffer;
        vb = vertexBuffer;
    }
    Mesh(Shader* sh, VertexArray* vertexArray, VertexBuffer* vertexBuffer){
        material = Material(sh);
        va = vertexArray;
        vb = vertexBuffer;
    }
    void Draw(Renderer& renderer, Camera& camera, glm::mat4 model){
        material.Bind();
        
        material.shader->setMat4("u_model", model);
        material.shader->setMat4("u_view", camera.GetViewMatrix());
        material.shader->setMat4("u_projection", camera.GetProjection());
        va->Bind();
        if(ib == nullptr){
            renderer.Draw(*va, *material.shader);
        }
        else{
            ib->Bind();
            renderer.Draw(*va, *ib, *material.shader);
        }
    }
};
