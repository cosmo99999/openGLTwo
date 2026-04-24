#include "entity.h"



class GameState{
public:
  std::vector<std::shared_ptr<Entity>> entities;
  std::vector<std::shared_ptr<Entity>> lightSources;
    
  GameState(){}
  void SetEntities(std::vector<std::shared_ptr<Entity>> ent);
  std::string SerializeEntities();
  void DeserializePacketIntoEntities(std::string);
  std::shared_ptr<Entity> DeserializeEntity(std::string);
  void UpdatePlayerFromPacket(std::string);
  int GetPlayerCount();
};
