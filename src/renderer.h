#include "includes.h"
#include "vertexArray.h"
#include "indexBuffer.h"
#include "shader.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GlClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

inline void GlClearError();

inline bool GLLogCall(const char* function, const char* file, int line);

class Renderer {
public:
    Renderer() {};
    void Clear();
    void Draw(VertexArray& va, IndexBuffer& ib, Shader& shader) const;
    void Draw(VertexArray& va, Shader& shader) const;
};