#pragma once
#include "assetManager.h"
#include "camera.h"
#include "globals.h"
#include "material.h"
#include "mesh.h"
#include <memory>
#include <random>

const float PI = 3.1415926525f;

glm::vec3 vec3FromString(std::string data);
std::string stringFromVec3(glm::vec3 vec3);
std::string wrapInQuotes(std::string in);
class Entity {
public:
  int id = -1;
  glm::vec3 position;
  glm::vec3 scale = glm::vec3(1.0f);
  glm::vec3 rotation = glm::vec3(0.0f);
  glm::vec3 velocity;
  bool lightSource = false;
  float maxSpeed = 50.0f;
  float acceleration = 50.0f;
  float decelearation = 50.0f;
  std::string entityName;
  std::string shaderName;

  Mesh mesh;

  Entity() {};
  Entity(glm::vec3 pos, glm::vec3 vel, std::string entityName,
         std::string shaderName);
  virtual void LoadMesh(AssetManager* am){};
  void Draw(Renderer &renderer, Camera &camera, glm::vec3 lightPos);
  void Update(Camera &camera, glm::vec3 lightPos);
  glm::mat4 GetModelMatrix();
  virtual std::string Serialize();
};
class Player : public Entity {
public:
  float vertices[36 * 6] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f,
      0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, 0.5f,  -0.5f,
      0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,
      0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f,
      0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, 0.5f,
      0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};
  Player(int id, glm::vec3 pos, glm::vec3 vel, std::string shaderName);
  void LoadMesh(AssetManager* am) override;
};
class Plane : public Entity {
public:
  float vertices[48] = {
      50.0f, 0.0f, 50.0f,  0.0f,   1.0f, 0.0f,   50.0f,  0.0f, -50.0f,
      0.0f,  1.0f, 0.0f,   -50.0f, 0.0f, 50.0f,  0.0f,   1.0f, 0.0f,

      50.0f, 0.0f, -50.0f, 0.0f,   1.0f, 0.0f,   -50.0f, 0.0f, 50.0f,
      0.0f,  1.0f, 0.0f,   -50.0f, 0.0f, -50.0f, 0.0f,   1.0f, 0.0f};
  Plane(glm::vec3 pos, glm::vec3 scale, std::string shaderName);
  void LoadMesh(AssetManager* am) override;
};
class Cube : public Entity {

public:
  float vertices[36 * 6] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f,
      0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, 0.5f,  -0.5f,
      0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,
      0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f,
      0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, 0.5f,
      0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};

  Cube(glm::vec3 pos, glm::vec3 velocity, std::string shaderName);
  void LoadMesh(AssetManager* am) override;
};
class Sphere : public Entity {

public:
  // memeber vars
  bool smooth;
  int stackAndSector;
  float radius;
  int upAxis; // +X=1, +Y=2, +z=3 (default)
  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> texCoords;
  std::vector<unsigned int> indices;
  std::vector<unsigned int> lineIndices;

  // interleaved
  std::vector<float> interleavedVertices;
  int interleavedStride; // # of bytes to hop to the next vertex (should be 32
                         // bytes)
  // getters/setters
  float getRadius() const { return radius; }
  int getSectorCount() const { return stackAndSector; }
  int getStackCount() const { return stackAndSector; }
  int getUpAxis() const { return upAxis; }
  void set(float radius, int sectorCount, int stackCount, bool smooth = true,
           int up = 3);
  void setRadius(float radius);
  void setSectorCount(int sectorCount);
  void setStackCount(int stackCount);
  void setSmooth(bool smooth);
  void setUpAxis(int up);
  void reverseNormals();

  // for vertex data
  unsigned int getVertexCount() const {
    return (unsigned int)vertices.size() / 3;
  }
  unsigned int getNormalCount() const {
    return (unsigned int)normals.size() / 3;
  }
  unsigned int getTexCoordCount() const {
    return (unsigned int)texCoords.size() / 2;
  }
  unsigned int getIndexCount() const { return (unsigned int)indices.size(); }
  unsigned int getLineIndexCount() const {
    return (unsigned int)lineIndices.size();
  }
  unsigned int getTriangleCount() const { return getIndexCount() / 3; }
  unsigned int getVertexSize() const {
    return (unsigned int)vertices.size() * sizeof(float);
  }
  unsigned int getNormalSize() const {
    return (unsigned int)normals.size() * sizeof(float);
  }
  unsigned int getTexCoordSize() const {
    return (unsigned int)texCoords.size() * sizeof(float);
  }
  unsigned int getIndexSize() const {
    return (unsigned int)indices.size() * sizeof(unsigned int);
  }
  unsigned int getLineIndexSize() const {
    return (unsigned int)lineIndices.size() * sizeof(unsigned int);
  }
  const float *getVertices() const { return vertices.data(); }
  const float *getNormals() const { return normals.data(); }
  const float *getTexCoords() const { return texCoords.data(); }
  const unsigned int *getIndices() const { return indices.data(); }
  const unsigned int *getLineIndices() const { return lineIndices.data(); }

  // for interleaved vertices: V/N/T
  unsigned int getInterleavedVertexCount() const {
    return getVertexCount();
  } // # of vertices
  unsigned int getInterleavedVertexSize() const {
    return (unsigned int)interleavedVertices.size() * sizeof(float);
  } // # of bytes
  int getInterleavedStride() const {
    return interleavedStride;
  } // should be 32 bytes
  const float *getInterleavedVertices() const {
    return interleavedVertices.data();
  }
  
  glm::vec3 lightPos = glm::vec3(-5.0f, 5.0f, 0.0f);
  Sphere(glm::vec3 pos, glm::vec3 velocity, std::string shaderName,
         int stackAndSectors, float r);
  void GenSphere(int stackAndSectors);
  void LoadMesh(AssetManager* am) override;
  std::string Serialize() override;
};
