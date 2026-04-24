#include "entity.h"
#include "assetManager.h"
#include <memory>
extern Globals globals;
std::string wrapInQuotes(std::string in){
  std::string result = "(";
  result += in;
  result += ")";
  return result;
}

glm::vec3 vec3FromString(std::string data){
  float x = -1;
  float y = -1;
  float z = -1;
  std::string tempString = "";
  
  for(int i = 0; i < data.length(); i++){
    char c = data[i];
    if(c == ','){
      if(x == -1){
        x = std::stof(tempString);
      }
      if(y == -1){
        x = std::stof(tempString);
      }
      if(z == -1){
        x = std::stof(tempString);
      }
    }else{
      tempString += c;
    }
  }
  return glm::vec3(x,y,z);
}
std::string stringFromVec3(glm::vec3 vec3){
  std::string result = "";
  result += std::to_string(vec3.x) + ",";
  result += std::to_string(vec3.y) + ",";
  result += std::to_string(vec3.z) + ",";
  return result;
}
Entity::Entity(glm::vec3 pos, glm::vec3 vel, std::string eName,
               std::string sName) {
  position = pos;
  velocity = vel;
  entityName = eName;
  shaderName = sName;
}
glm::mat4 Entity::GetModelMatrix() {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  model = glm::rotate(model, rotation.x, {1, 0, 0});
  model = glm::scale(model, scale);
  return model;
}

void Entity::Draw(Renderer &renderer, Camera &camera, glm::vec3 lightpos) {
  Update(camera, lightpos);
  mesh.Draw(renderer, camera, GetModelMatrix());
}

void Entity::Update(Camera &camera, glm::vec3 lightPos) {
  mesh.material.shader->use();
  mesh.material.shader->setVec3("objectColor", mesh.material.colour);
  mesh.material.shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
  mesh.material.shader->setVec3("lightPos", lightPos);
  mesh.material.shader->setVec3("viewPos", camera.Position);
}

std::string Entity::Serialize() {
  std::string result = "";
  result += "[entityname]" + wrapInQuotes(entityName);
  result += "[lightsource]" + wrapInQuotes(std::to_string(lightSource));
  result += "[id]" + wrapInQuotes(std::to_string(id));
  result += "[shader]" + wrapInQuotes(shaderName);
  result += "[position]" + wrapInQuotes(stringFromVec3(position));
  result += "[velocity]" + wrapInQuotes(stringFromVec3(velocity));
  result += "[scale]" + wrapInQuotes(stringFromVec3(scale));
  result += "\n";
  return result;
}
std::string Sphere::Serialize() {

  std::string result = "";
  result += "[entityname]" + wrapInQuotes(entityName);
  result += "[lightsource]" + wrapInQuotes(std::to_string(lightSource));
  result += "[id]" + wrapInQuotes(std::to_string(id));
  result += "[shader]" + wrapInQuotes(shaderName);
  result += "[position]" + wrapInQuotes(stringFromVec3(position));
  result += "[velocity]" + wrapInQuotes(stringFromVec3(velocity));
  result += "[scale]" + wrapInQuotes(stringFromVec3(scale));
  result += "[stackAndSector]" + wrapInQuotes(std::to_string(stackAndSector));
  result += "[radius]" + wrapInQuotes(std::to_string(radius));
  result += "\n";
  return result;
}
Player::Player(int i, glm::vec3 pos, glm::vec3 vel, std::string shaderName)
    : Entity(pos, vel, "Player", shaderName) {
  i = id;
}
Cube::Cube(glm::vec3 pos, glm::vec3 vel, std::string shaderName)
    : Entity(pos, vel, "Cube", shaderName) {}
Sphere::Sphere(glm::vec3 pos, glm::vec3 vel, std::string shaderName, int sAs,
               float r)
    : Entity(pos, vel, "Sphere", shaderName) {
  stackAndSector = sAs;
  radius = r;
}
Plane::Plane(glm::vec3 pos, glm::vec3 sc, std::string shaderName)
    : Entity(pos, glm::vec3(0.0f), "Plane", shaderName) {
      scale = sc;
    }
void Cube::LoadMesh(AssetManager *am) {
  VertexBuffer *vb = new VertexBuffer(vertices, sizeof(vertices));
  VertexArray *va = new VertexArray();
  VertexBufferLayout vl;

  Shader *shader = am->GetShader(shaderName);
  vl.Push<float>(3);
  vl.Push<float>(3);
  va->AddBuffer(*vb, vl);
  va->SetCount(36);
  mesh = Mesh(shader, va, vb);
  entityName = "cube";
}
void Sphere::LoadMesh(AssetManager *am) {
  GenSphere(stackAndSector);
  VertexBuffer *vb =
      new VertexBuffer(getInterleavedVertices(), getInterleavedVertexSize());
  IndexBuffer *ib = new IndexBuffer(getIndices(), getIndexCount());
  VertexArray *va = new VertexArray();
  Shader *shader = am->GetShader(shaderName);
  vb->Bind();
  VertexBufferLayout vl;
  vl.Push<float>(3);
  vl.Push<float>(3);
  va->AddBuffer(*vb, vl);
  va->SetCount(getIndexCount());
  mesh = Mesh(shader, ib, va, vb);
  entityName = "sphere";
}
void Plane::LoadMesh(AssetManager *am) {
  VertexBuffer *vb = new VertexBuffer(vertices, sizeof(vertices));
  VertexArray *va = new VertexArray();
  VertexBufferLayout vl;
  Shader *shader = am->GetShader(shaderName);
  vl.Push<float>(3);
  vl.Push<float>(3);
  va->AddBuffer(*vb, vl);
  va->SetCount(6);
  mesh = Mesh(shader, va, vb);
  mesh.material.colour = glm::vec3(0.5f, 0.5f, 0.5f);
  entityName = "plane";
}
void Sphere::GenSphere(int stackAndSector) {
  float x, y, z, xy;                           // vertex position
  float nx, ny, nz, lengthInv = 1.0f / radius; // vertex normal
  // float s, t;                                     // vertex texCoord

  float sectorStep = 2 * PI / stackAndSector;
  float stackStep = PI / stackAndSector;
  float sectorAngle, stackAngle;

  for (int i = 0; i <= stackAndSector; ++i) {
    stackAngle = PI / 2 - i * stackStep; // starting from pi/2 to -pi/2
    xy = radius * cosf(stackAngle);      // r * cos(u)
    z = radius * sinf(stackAngle);       // r * sin(u)

    // add (sectorCount+1) vertices per stack
    // first and last vertices have same position and normal, but different
    // tex coords
    for (int j = 0; j <= stackAndSector; ++j) {
      sectorAngle = j * sectorStep; // starting from 0 to 2pi

      // vertex position (x, y, z)
      x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
      y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
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
      // s = (float)j / sectorCount;
      // t = (float)i / stackCount;
      // texCoords.push_back(s);
      // texCoords.push_back(t);
    }
  }

  int k1, k2;
  for (int i = 0; i < stackAndSector; ++i) {
    k1 = i * (stackAndSector + 1); // beginning of current stack
    k2 = k1 + stackAndSector + 1;  // beginning of next stack

    for (int j = 0; j < stackAndSector; ++j, ++k1, ++k2) {
      // 2 triangles per sector excluding first and last stacks
      // k1 => k2 => k1+1
      if (i != 0) {
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);
      }

      // k1+1 => k2 => k2+1
      if (i != (stackAndSector - 1)) {
        indices.push_back(k1 + 1);
        indices.push_back(k2);
        indices.push_back(k2 + 1);
      }

      // store indices for lines
      // vertical lines for all stacks, k1 => k2
      lineIndices.push_back(k1);
      lineIndices.push_back(k2);
      if (i != 0) // horizontal lines except 1st stack, k1 => k+1
      {
        lineIndices.push_back(k1);
        lineIndices.push_back(k1 + 1);
      }
    }
  }

  // build interleaved vertices (V/N per vertex)
  interleavedStride = 6 * sizeof(float); // 3 pos + 3 normal, no texcoords
  interleavedVertices.clear();
  for (int i = 0; i < (int)vertices.size() / 3; ++i) {
    interleavedVertices.push_back(vertices[i * 3]);
    interleavedVertices.push_back(vertices[i * 3 + 1]);
    interleavedVertices.push_back(vertices[i * 3 + 2]);
    interleavedVertices.push_back(normals[i * 3]);
    interleavedVertices.push_back(normals[i * 3 + 1]);
    interleavedVertices.push_back(normals[i * 3 + 2]);
  }
}
