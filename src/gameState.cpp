#include "gameState.h"
#include "entity.h"
#include <memory>
#include <string>

int GameState::GetPlayerCount() {
  int result = 0;
  for (auto &e : entities) {
    if (e->entityName == "Player") {
      result++;
    }
  }
  return result;
}
void GameState::Equals(GameState &g) {
  for(auto& e : g.entities){
    auto mine = GetEntityById(e->id);
    if(mine != nullptr){
      mine->Equals(e);
    }else{
      AddEntity(e);
    }
  }
  timeStamp = g.timeStamp;
}
void GameState::AddEntity(std::shared_ptr<Entity> e) {
  if (e->id == 0) {
    e->id = entities.size() + 1;
  }
  std::cout << "added entity: " << e->entityName << "\n";
  entities.emplace_back(e);
}
std::string GameState::CreateOutgoingPacketString() {
  std::string result = "";
  result += "[timestamp]" + wrapInQuotes(std::to_string(timeStamp)) + "\n";
  std::string entities = SerializeEntities();
  result += entities;
  return result;
}
std::string GameState::SerializeEntities() {
  std::string result = "";

  for (auto &e : entities) {
    result += e->Serialize();
  }
  // std::cout << "all data: " << result << "\n";
  return result;
}
void GameState::ClientDeserialize(std::string data) {
  std::string eData = "";
  // std::cout << data << "\n";
  bool header = true;
  for (int i = 0; i < data.length(); i++) {

    if (data[i] == '\n') {
      if (header) {
        int s = eData.find('(');
        int e = eData.find(')');
        double time = 0.0;
        std::string temp = "";
        for (int i = s + 1; i < e; i++) {
          temp += data[i];
        }
        time = std::stod(temp);
        timeStamp = time;
        header = false;
        eData = "";
        continue;
      } else {
        std::shared_ptr<Entity> tempE = DeserializeEntity(eData);
        // std::cout << "looking for ent: " << eData << "\n";
        if (!UpdateEntity(tempE)) {
          AddEntity(tempE);
        }
      }
      continue;
    } else {
      eData += data[i];
    }
  }
  CheckLightSources();
}
bool GameState::UpdateEntity(std::shared_ptr<Entity> updatedE) {

  // std::cout << "entity Count " << entities.size() << "\n";
  for (auto &e : entities) {
    // std::cout << "Looking for " << updatedE->id << "\n";
    // std::cout << "but found " << e->id << "\n";
    if (e->id == updatedE->id) {
      e->Equals(updatedE);
      // std::cout << "updated: " << e->id << " "<< e->entityName <<  "\n";
      return true;
    }
  }
  return false;
}
void GameState::CheckLightSources() {
  lightSources.clear();
  for (auto &e : entities) {
    if (e->lightSource == true) {
      lightSources.emplace_back(e);
    }
  }
}
void GameState::ServerDeserialize(std::string data) {
  std::string eData = "";
  std::shared_ptr<Entity> player = nullptr;
  int pId = -1000;
  for (int i = 0; i < data.length(); i++) {

    if (data[i] == '\n') {
      player = DeserializeEntity(eData);
      if (!UpdateEntity(player)) {
        AddEntity(player);
      }
    } else {
      eData += data[i];
    }
  }
}

std::shared_ptr<Entity> GameState::DeserializeEntity(std::string input) {

  std::shared_ptr<Entity> result = nullptr;
  glm::vec3 p;
  glm::vec3 v;
  glm::vec3 s;
  glm::vec3 r;

  int eId = 0;
  std::string sName;
  std::string eName;
  int iSlightSource = -1;

  int sAs = 0;
  int eRadius = 0;

  std::string key = "";
  std::string value = "";
  bool readingKey = false;
  bool readingValue = false;
  bool setVariable = false;

  for (int i = 0; i < input.length(); i++) {
    if (input[i] == '[') {
      readingKey = true;
      continue;
    }
    if (input[i] == ']') {
      readingKey = false;
      continue;
    }
    if (input[i] == '(') {
      readingValue = true;
      continue;
    }
    if (input[i] == ')') {
      readingValue = false;
      setVariable = true;
    }

    if (readingKey)
      key += input[i];
    else if (readingValue)
      value += input[i];

    if (setVariable) {

      if (key == "entityname")
        eName = value;
      if (key == "lightsource")
        iSlightSource = std::stoi(value);
      if (key == "id")
        eId = std::stoi(value);
      if (key == "shader")
        sName = value;
      if (key == "position")
        p = vec3FromString(value);
      if (key == "velocity")
        v = vec3FromString(value);
      if (key == "scale")
        s = vec3FromString(value);
      if (key == "rotation")
        r = vec3FromString(value);
      if (key == "stackAndSector")
        sAs = std::stoi(value);
      if (key == "radius")
        eRadius = std::stof(value);

      key = "";
      value = "";
      setVariable = false;
    }
  }

  if (eName == "Cube") {
    result = std::make_shared<Cube>(eId, p, v, s, r, sName);
  } else if (eName == "Plane") {
    result = std::make_shared<Plane>(eId, p, v, s, r, sName);
    result->scale = s;
  } else if (eName == "Sphere") {
    result = std::make_shared<Sphere>(eId, p, v, s, r, sName, sAs, eRadius);
  } else if (eName == "Player") {
    result = std::make_shared<Player>(eId, p, v, s, r, sName);
    result->rotation = r;
  }
  if (iSlightSource != -1) {
    result->lightSource = iSlightSource;
  }
  if (result == nullptr) {
  }
  return result;
}

void GameState::PrintEntities() {
  std::cout << "EntityCount" << entities.size() << "\n";
  for (auto &p : entities) {
    std::cout << "ID : " << p->id << "\n";
    std::cout << "name : " << p->entityName << "\n";
    std::cout << "\n";
  }
}
std::shared_ptr<Entity> GameState::GetEntityById(int i){
  for(auto& e : entities){
    if(e->id == i)return e;
  }
  return nullptr;
}
