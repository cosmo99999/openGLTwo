#include "entity.h"



class GameState{
public:
  double timeStamp = 0.0;
  std::vector<std::shared_ptr<Entity>> entities;
  std::vector<std::shared_ptr<Entity>> lightSources;
    
  GameState(){}
  void AddEntity(std::shared_ptr<Entity> e);
  bool UpdateEntity(std::shared_ptr<Entity> e);
  void CheckLightSources();
  std::string SerializeEntities();
  std::string CreateOutgoingPacketString();
  void ClientDeserialize(std::string);
  void ServerDeserialize(std::string data);
  std::shared_ptr<Entity> DeserializeEntity(std::string);
  int GetPlayerCount();
  void PrintEntities();
  void Equals(GameState& g);
  std::shared_ptr<Entity> GetEntityById(int id);
};
