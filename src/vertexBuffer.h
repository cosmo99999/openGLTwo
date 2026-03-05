#pragma once
#include "includes.h"

class VertexBuffer {
private:
    unsigned int m_RendererID;
public:
    VertexBuffer(){}
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    void Bind();
    void Unbind();
};