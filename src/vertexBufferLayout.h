#pragma once
struct VertexBufferElement{
    unsigned int type;
    unsigned int count; 
    unsigned char normalised;

    static unsigned int GetSizeOfType(unsigned int type){
        switch(type){
        
            case GL_FLOAT: return 4;
            case GL_UNSIGNED_INT : return 4;
            case GL_UNSIGNED_BYTE : return 1;
        }
        assert(false);
        return 0;
    }
};

class VertexBufferLayout{

private: 
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride = 0;
public:
    VertexBufferLayout(){};

    template<typename T>
    void Push(unsigned int count){
        static_assert(sizeof(T) == 0, "Unsupported type");
    };
    inline unsigned int GetStride() {return m_Stride;}
    inline unsigned int GetElementsCount() {return m_Elements.size();}
    inline std::vector<VertexBufferElement> GetElements() {return m_Elements;}
    
};
template<>
inline void VertexBufferLayout::Push<float>(unsigned int count){
        m_Elements.push_back({GL_FLOAT, count, GL_FALSE});
        m_Stride += sizeof(float) * count;
    };
template<>
inline void VertexBufferLayout::Push<unsigned int>(unsigned int count){
        m_Elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
        m_Stride += sizeof(unsigned int ) * count;
    };