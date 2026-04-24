#include "gameState.h"
#include <memory>
#include <string>

int GameState::GetPlayerCount(){
  int result = 0;
  for(auto& e : entities){
    if(e->id != -1){
      result++;
    }
  }
  return result;
}
std::string GameState::SerializeEntities(){
  std::string result = "";

  for(auto& e : entities){
    result += e->Serialize();
  }
  return result;
}
void GameState::SetEntities(std::vector<std::shared_ptr<Entity>> in){
  entities.clear();
  entities = in;
}
void GameState::DeserializePacketIntoEntities(std::string data){
  entities.clear();
  lightSources.clear();
  std::string eData = "";
  
  for(int i = 0; i < data.length(); i++){

    if(data[i] == '\n'){
      std::shared_ptr<Entity> tempE = DeserializeEntity(eData);
      std::cout << "trying to add entity" << "\n";
      entities.emplace_back(tempE);
      std::cout << "added entity" << "\n";
      if(tempE->lightSource == true){
       lightSources.emplace_back(tempE); 
      }
      continue;
    }else{
      eData += data[i];
    }
  }
   
}

std::shared_ptr<Entity> GameState::DeserializeEntity(std::string input){

  std::shared_ptr<Entity> result = nullptr; 
  glm::vec3 p;
  glm::vec3 v;
  glm::vec3 s;

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
  
  std::cout << "reached before deserialize loop : " << "\n";
  for(int i = 0; i < input.length(); i++){
    if(input[i] == '['){
      readingKey = true;
      continue;
    } 
    if(input[i] == ']'){
      readingKey = false;
      continue;
    } 
    if(input[i] == '('){
      readingValue = true;
      continue;
    } 
    if(input[i] == ')'){
      readingValue = false;
      setVariable = true;
      continue;
    } 

    if(readingKey) key += input[i];
    else if (readingValue) value += input[i];

    if(setVariable){

  std::cout << "key : "<< key << "\n";
  std::cout << "value :"<< value << "\n";
      if(key == "entityname")     eName = value;
      if(key == "lightsource")    iSlightSource = std::stoi(value);
      if(key == "id")             eId = std::stoi(value);
      if(key == "shader")         sName = value;
      if(key == "position")       p = vec3FromString(value);
      if(key == "velocity")       v = vec3FromString(value);
      if(key == "scale")          s = vec3FromString(value);
      if(key == "stackAndSector") sAs = std::stoi(value);
      if(key == "radius")         eRadius = std::stof(value);

      key = "";
      value = "";
      setVariable = false;
    }  
  }

  std::cout << "ename: "<< eName << "\n";
  if     (eName == "Cube"){
    result = std::make_shared<Cube>(p, v, sName);
  }
  else if(eName == "Plane"){
    result = std::make_shared<Plane>(p, v, sName);
  }
  else if(eName == "Sphere"){
    result = std::make_shared<Sphere>(p, v, sName, sAs, eRadius);
  }
  else if(eName == "Player"){
    result = std::make_shared<Player>(eId, p, v, sName);
  }
  std::cout << "reached before setting light source : " << "\n";
  if(iSlightSource != -1){
    result->lightSource = iSlightSource;
  }
  if(result == nullptr){
    std::cout << "result from deserialize is null" << "\n";
  }
  std::cout << "returning : " << "\n";
  std::cout << "entity: "<< result->entityName << "\n";
  return result;
}
void GameState::UpdatePlayerFromPacket(std::string input){
  int pId = -2;
  glm::vec3 p;
  glm::vec3 v;

  
  std::string key = "";
  std::string value = "";
  bool readingKey = false;
  bool readingValue = false;
  bool setVariable = false;
  
  for(int i = 0; i < input.length(); i++){
    if(input[i] == '['){
      readingKey = true;
      continue;
    } 
    if(input[i] == ']'){
      readingKey = false;
      continue;
    } 
    if(input[i] == '('){
      readingValue = true;
      continue;
    } 
    if(input[i] == ')'){
      readingValue = false;
      setVariable = true;
      continue;
    } 

    if(readingKey) key += input[i];
    else if (readingValue) value += input[i];

    if(setVariable){
      if(key == "id")             pId = std::stoi(value);
      if(key == "position")       p = vec3FromString(value);
      if(key == "velocity")       v = vec3FromString(value);
      key = "";
      value = "";
    }  
  }
  bool found = false;
  for(auto& e : entities){
    if(e->id == pId){
      e->position = p;
      e->velocity = v;
      found = true;
    }
  }
  if(!found){
    entities.emplace_back(std::make_shared<Player>(pId, p, v, "Lighting"));
  }
}
