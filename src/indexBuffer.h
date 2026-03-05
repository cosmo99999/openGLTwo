#pragma once

class IndexBuffer {
private:
    unsigned int m_RendererID;
public:
    unsigned int m_Count = 0;
    IndexBuffer(){}
    IndexBuffer(const unsigned int* data, unsigned int count);
    ~IndexBuffer();

    void Bind();
    void Unbind();
    inline unsigned int GetCount() const {return m_Count; }
};