#include "renderer.h"
#include <iostream>

void GlClearError()
{
    while(glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char *function, const char *file, int line)
{
    while(GLenum error = glGetError()){

        std::cout << "[Open GL error ] (" << error << "): " << function 
            << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

void Renderer::Clear()
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(VertexArray &va, Shader &shader) const
{
    shader.use();
    va.Bind();
    GLCall(glDrawArrays(GL_TRIANGLES, 0, va.GetCount()));
}

void Renderer::Draw(VertexArray &va,IndexBuffer &ib, Shader &shader) const
{
    shader.use();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
