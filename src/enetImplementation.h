#include "enetTemplates.h"
#include <cstring>
#include <enet/enet.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include "entity.h"
#include "gameState.h"
class GamePacket {
public:
  ENetPacket *packet = nullptr;
  const char *packetData;

  int id = -1;
  std::string x;
  std::string y;
  std::string z;

  GamePacket(int id, Entity* p) {
    if (id == -1)
      return;
    std::string packetString = "[id]" + wrapInQuotes(std::to_string(p->id)) +
                               "[position]" + wrapInQuotes(stringFromVec3(p->position)) +
                               "[velocity]" + wrapInQuotes(stringFromVec3(p->velocity)) + "\n";
    packetData = packetString.c_str();
    packet = enet_packet_create(packetData, strlen(packetData) + 1,
                                ENET_PACKET_FLAG_RELIABLE);
  }
  GamePacket(std::string in) {
    packetData = in.c_str();
    packet = enet_packet_create(packetData, strlen(packetData) + 1,
                                ENET_PACKET_FLAG_RELIABLE);
  }
  GamePacket(int num) {
    std::string packetString = std::to_string(num);
    packetData = packetString.c_str();
    packet = enet_packet_create(packetData, strlen(packetData) + 1,
                                ENET_PACKET_FLAG_RELIABLE);
  }
};

class ServerPacketManager : public ENetManager {
public:
  std::mutex pMtx;
  GameState gameState;
  GamePacket *outgoingPacket = nullptr;

  ServerPacketManager(int frequency) {
    host = new ENetServer(9999, "0.0.0.0");
    sendFrequency = frequency;
  }
  void startListening() override {
    t_listener = std::thread(&ServerPacketManager::Listen, this);
  }
  void stopListening() override { t_listener.join(); }
  void startSending() override {
    t_sender = std::thread(&ServerPacketManager::Send, this);
  }
  void stopSending() override { t_sender.join(); }

  void Send() override {
    while (true) {
      pMtx.lock();
      CreateBroadCastPacket();
      pMtx.unlock();
      if (outgoingPacket->packet != nullptr) {
        host->SendPacket(outgoingPacket->packet);
      }
      DeleteBroadCastPacket();
      std::this_thread::sleep_for(std::chrono::milliseconds(sendFrequency));
    }
  }
  void Listen() override {
    while (true) {
      ENetEvent event = host->Listen();
      switch (event.type) {
      case ENET_EVENT_TYPE_NONE:
        break;
      case ENET_EVENT_TYPE_CONNECT:
        std::cout << "User Connected from " << event.peer->address.host << "\n";
        ManageNewConnection(event.peer);
        host->SendPacket(outgoingPacket->packet, event.peer);
        DeleteBroadCastPacket();
        break;
      case ENET_EVENT_TYPE_RECEIVE:
        // std::cout << "recieved packet: " << event.packet->data << "\n";
        pMtx.lock();
        HandleIncomingPacket(event.packet);
        pMtx.unlock();
        enet_packet_destroy(event.packet);
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        std::cout << "User disconnected from : " << event.peer->address.host
                  << "\n";
        event.peer->data = NULL;
        break;
      }
    }
  }
  void HandleIncomingPacket(ENetPacket *packet) {
    std::string packetString = StringFromUChar(packet->data);
    gameState.UpdatePlayerFromPacket(packetString);
  }

  void CreateBroadCastPacket() {
     std::string entString = gameState.SerializeEntities();
     outgoingPacket = new GamePacket(entString);
   }
  void ManageNewConnection(ENetPeer* peer) {
    int id = gameState.GetPlayerCount() + 1;
    outgoingPacket = new GamePacket(id);
    peer->data = &id;
  }
  void DeleteBroadCastPacket() { delete outgoingPacket; }
};
class ClientPacketManager : public ENetManager {
public:
  std::mutex cMtx;
  GameState gameState;
  bool firstPacket = true;
  int myID = -1;
  GamePacket *outgoingPacket = nullptr;
  Entity* me;
  ClientPacketManager(int frequency, Entity* m) {
    std::string targetIP;
    std::cout << "Enter IP to connect to: " << "\n";
    std::cin >> targetIP;
    host = new ENetClient(9999, targetIP);
    me = m;
    sendFrequency = frequency;
  }
  void startListening() override {
    t_listener = std::thread(&ClientPacketManager::Listen, this);
  }
  void stopListening() override { t_listener.join(); }
  void startSending() override {
    t_sender = std::thread(&ClientPacketManager::Send, this);
  }
  void stopSending() override { t_sender.join(); }

  void Send() override {
    while (true) {
      cMtx.lock();
      if (!firstPacket) {
        CreateBroadCastPacket();
        std::cout << "sending packet" << outgoingPacket->packet->data << "\n";
        host->SendPacket(outgoingPacket->packet);
        DeleteBroadCastPacket();
      }
      cMtx.unlock();
      std::this_thread::sleep_for(std::chrono::milliseconds(sendFrequency));
    }
  }
  void Listen() override {
    while (true) {
      ENetEvent event = host->Listen();
      switch (event.type) {
      case ENET_EVENT_TYPE_NONE:
        break;
      case ENET_EVENT_TYPE_CONNECT:
        break;
      case ENET_EVENT_TYPE_RECEIVE:
        // std::cout << event.packet->data << "\n";
        cMtx.lock();
        if (firstPacket) {
          RecieveConnectionPacket(event.packet);
          std::cout << "first packet: " << event.packet->data << "\n";
          firstPacket = false;
        } else {
          std::cout << "recieved packet: " << event.packet->data << "\n";
          HandleIncomingPacket(event.packet);
        }
        cMtx.unlock();
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        break;
      }
    }
  }
  void HandleIncomingPacket(ENetPacket *packet) {
    std::string packetString = StringFromUChar(packet->data);
    UpdateGameStateFromPacket(packetString);
  }
  void RecieveConnectionPacket(ENetPacket *packet) {
    std::string packetString = StringFromUChar(packet->data);
    myID = (char)packetString[0] - '0';
  }

  void UpdateGameStateFromPacket(std::string data) {
    gameState.DeserializePacketIntoEntities(data);
  }
  void CreateBroadCastPacket() {
    outgoingPacket = new GamePacket(myID, me);
  }
  void DeleteBroadCastPacket() { delete outgoingPacket; }
  void PrintEntities() {
    for (auto &p : gameState.entities) {
      std::cout << "ID : " << p->id << "\n";
      std::cout << "name : " << p->entityName << "\n";
      std::cout << "\n";
    }
  }
};
