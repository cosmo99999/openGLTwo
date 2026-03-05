#include "object.h"
#include "globals.h"
#include <cmath>

extern Globals globals;

class Cube : public Object{
    
public:
    float vertices[36*6] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    Cube(Shader* sh, glm::vec3 pos) : Object(pos){
        std::cout << "sizeof(vertices): " << sizeof(vertices) << "\n";
        std::cout << "first few verts: " 
                  << vertices[0] << "," << vertices[1] << "," << vertices[2] << " | "
                  << vertices[3] << "," << vertices[4] << "," << vertices[5] << "\n";
        
        VertexBuffer* vb = new VertexBuffer(vertices, sizeof(vertices));
        VertexArray* va  = new VertexArray();
        VertexBufferLayout vl;
        vl.Push<float>(3);
        vl.Push<float>(3);
        va->AddBuffer(*vb, vl);
        va->SetCount(36);
        mesh = Mesh(sh, va);
        
        std::cout << "va count after setup: " << va->GetCount() << "\n";
        std::cout << "mesh.va count: " << mesh.va->GetCount() << "\n";
    }
};

class Sphere : public Object{
    
public:
    // memeber vars
    float radius;
    int sectorCount;                        // longitude, # of slices
    int stackCount;                         // latitude, # of stacks
    bool smooth;
    int upAxis;                             // +X=1, +Y=2, +z=3 (default)
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> lineIndices;

    // interleaved
    std::vector<float> interleavedVertices;
    int interleavedStride;                  // # of bytes to hop to the next vertex (should be 32 bytes)
    // getters/setters
    float getRadius() const                 { return radius; }
    int getSectorCount() const              { return sectorCount; }
    int getStackCount() const               { return stackCount; }
    int getUpAxis() const                   { return upAxis; }
    void set(float radius, int sectorCount, int stackCount, bool smooth=true, int up=3);
    void setRadius(float radius);
    void setSectorCount(int sectorCount);
    void setStackCount(int stackCount);
    void setSmooth(bool smooth);
    void setUpAxis(int up);
    void reverseNormals();

    // for vertex data
    unsigned int getVertexCount() const     { return (unsigned int)vertices.size() / 3; }
    unsigned int getNormalCount() const     { return (unsigned int)normals.size() / 3; }
    unsigned int getTexCoordCount() const   { return (unsigned int)texCoords.size() / 2; }
    unsigned int getIndexCount() const      { return (unsigned int)indices.size(); }
    unsigned int getLineIndexCount() const  { return (unsigned int)lineIndices.size(); }
    unsigned int getTriangleCount() const   { return getIndexCount() / 3; }
    unsigned int getVertexSize() const      { return (unsigned int)vertices.size() * sizeof(float); }
    unsigned int getNormalSize() const      { return (unsigned int)normals.size() * sizeof(float); }
    unsigned int getTexCoordSize() const    { return (unsigned int)texCoords.size() * sizeof(float); }
    unsigned int getIndexSize() const       { return (unsigned int)indices.size() * sizeof(unsigned int); }
    unsigned int getLineIndexSize() const   { return (unsigned int)lineIndices.size() * sizeof(unsigned int); }
    const float* getVertices() const        { return vertices.data(); }
    const float* getNormals() const         { return normals.data(); }
    const float* getTexCoords() const       { return texCoords.data(); }
    const unsigned int* getIndices() const  { return indices.data(); }
    const unsigned int* getLineIndices() const  { return lineIndices.data(); }
    
    // for interleaved vertices: V/N/T
    unsigned int getInterleavedVertexCount() const  { return getVertexCount(); }    // # of vertices
    unsigned int getInterleavedVertexSize() const   { return (unsigned int)interleavedVertices.size() * sizeof(float); }    // # of bytes
    int getInterleavedStride() const                { return interleavedStride; }   // should be 32 bytes
    const float* getInterleavedVertices() const     { return interleavedVertices.data(); }

    glm::vec3 lightPos = glm::vec3(-5.0f, 5.0f, 0.0f);
    Sphere(int stackCount, int sectorCount, float r, glm::vec3 pos) : Object(pos){
        radius = r;
        
        GenSphere(stackCount, sectorCount);
        //vb = new VertexBuffer(getInterleavedVertices(), getInterleavedVertexSize());
        //ib = new IndexBuffer(getIndices(),getIndexCount());
        //va = new VertexArray();
        // vb->Bind();
        // VertexBufferLayout vl;
        // vl.Push<float>(3);
        // vl.Push<float>(3);
        // va->AddBuffer(*vb, vl);
    }
    void GenSphere(int stackCount, int sectorCount){
        float x, y, z, xy;                              // vertex position
        float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
        //float s, t;                                     // vertex texCoord

        float sectorStep = 2 * M_PI / sectorCount;
        float stackStep = M_PI / stackCount;
        float sectorAngle, stackAngle;

        for(int i = 0; i <= stackCount; ++i)
        {
            stackAngle = M_PI  / 2 - i * stackStep;        // starting from pi/2 to -pi/2
            xy = radius * cosf(stackAngle);             // r * cos(u)
            z = radius * sinf(stackAngle);              // r * sin(u)
        
            // add (sectorCount+1) vertices per stack
            // first and last vertices have same position and normal, but different tex coords
            for(int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;           // starting from 0 to 2pi
            
                // vertex position (x, y, z)
                x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
                y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            
                // normalized vertex normal (nx, ny, nz)
                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);
            
                // vertex tex coord (s, t) range between [0, 1]
                //s = (float)j / sectorCount;
                //t = (float)i / stackCount;
                //texCoords.push_back(s);
                //texCoords.push_back(t);
            }
        }
        
        int k1, k2;
        for(int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // beginning of current stack
            k2 = k1 + sectorCount + 1;      // beginning of next stack
        
            for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding first and last stacks
                // k1 => k2 => k1+1
                if(i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }
            
                // k1+1 => k2 => k2+1
                if(i != (stackCount-1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            
                // store indices for lines
                // vertical lines for all stacks, k1 => k2
                lineIndices.push_back(k1);
                lineIndices.push_back(k2);
                if(i != 0)  // horizontal lines except 1st stack, k1 => k+1
                {
                    lineIndices.push_back(k1);
                    lineIndices.push_back(k1 + 1);
                }
            }
        }
        
        // build interleaved vertices (V/N per vertex)
        interleavedStride = 6 * sizeof(float); // 3 pos + 3 normal, no texcoords
        interleavedVertices.clear();
        for(int i = 0; i < (int)vertices.size() / 3; ++i)
        {
            interleavedVertices.push_back(vertices[i*3]);
            interleavedVertices.push_back(vertices[i*3+1]);
            interleavedVertices.push_back(vertices[i*3+2]);
            interleavedVertices.push_back(normals[i*3]);
            interleavedVertices.push_back(normals[i*3+1]);
            interleavedVertices.push_back(normals[i*3+2]);
        }
    }
    // void Update(Camera camera, glm::vec3& lightPosition) override {
    //     if(type == ShaderType::Lighting){
    //         lightPosition.x += 0.005f;
    //         if(lightPosition.x > 5.0f){
    //             lightPosition.x = -5.0f;
    //         }
    //         shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    //         shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    //         shader.setVec3("lightPos", lightPosition);
    //         shader.setVec3("viewPos", camera.Position);
    //     }
    //     if(type == ShaderType::Regular){
    //         shader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
    //     }
    // }
};