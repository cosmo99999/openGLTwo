#pragma once
#include "vertexBuffer.h"
#include "vertexBufferLayout.h"

class VertexArray {
private:
    unsigned int m_RendererID;
public:
    unsigned int count;
    VertexArray();
    ~VertexArray();

    void AddBuffer(VertexBuffer& vb, VertexBufferLayout& layout);
    void Bind();
    void Unbind();
    inline unsigned int GetCount() {return count;}
    inline void SetCount(int c) {count = c;}
};