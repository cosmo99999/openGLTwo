#include "vertexArray.h"
#include "renderer.h"   
VertexArray::VertexArray()
{
    GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
    GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(VertexBuffer &vb, VertexBufferLayout &layout)
{
    Bind();
    vb.Bind();
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    //debugging
    GLint maxAttribs = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);
    // std::cout << "[VAO] Stride: " << layout.GetStride()
    //           << " | Element count: " << elements.size()
    //           << " | Max attribs: " << maxAttribs << "\n";
    
    count = layout.GetElementsCount();
    for(unsigned int i = 0; i < elements.size(); i++){
        const auto& element = elements[i];
        //debugging
        // std::cout << "[VAO] Attrib " << i
        //           << " | count: "      << element.count
        //           << " | type: 0x"     << std::hex << element.type << std::dec
        //           << " | normalised: " << (int)element.normalised
        //           << " | offset: "     << offset << "\n";
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalised, layout.GetStride(),  (const void*)(uintptr_t)offset));
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
    count = layout.GetStride()/ sizeof(float);

}

void VertexArray::Bind()
{
    GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind()
{
    GLCall(glBindVertexArray(0));
}

