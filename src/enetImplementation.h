#include "enetTemplates.h"
#include "entity.h"
#include "gameState.h"
#include "timer.h"
#include <cstring>
#include <enet/enet.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
class GamePacket {
public:
  ENetPacket *packet = nullptr;
  const char *packetData;

  int id = -1;
  std::string x;
  std::string y;
  std::string z;

  GamePacket(Entity *p) {
    if (p->id == -1)
      return;
    std::string packetString =
        "[id]" + wrapInQuotes(std::to_string(p->id)) + "[position]" +
        wrapInQuotes(stringFromVec3(p->position)) + "[velocity]" +
        wrapInQuotes(stringFromVec3(p->velocity)) + "[rotation]" +
        wrapInQuotes(stringFromVec3(p->rotation)) + "\n";
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
  std::unique_ptr<Timer> timer = std::make_unique<Timer>();

  ServerPacketManager(int frequency) {
    host = new ENetServer(9999, "0.0.0.0");
    sendFrequency = frequency;
  }
  void startListening() override {
    t_listener = std::thread(&ServerPacketManager::Listen, this);
  }
  void stopListening() override { t_listener.join(); }
  void startSending() override {
    timer->Start();
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
    gameState.ServerDeserialize(packetString);
  }

  void CreateBroadCastPacket() {
    gameState.timeStamp = timer->ElapsedMilliseconds();
    std::string entString = gameState.CreateOutgoingPacketString();
    outgoingPacket = new GamePacket(entString);
  }
  void ManageNewConnection(ENetPeer *peer) {
    int id = gameState.entities.size() + 1;
    outgoingPacket = new GamePacket(id);
    peer->data = &id;
  }
  void DeleteBroadCastPacket() { delete outgoingPacket; }
};
class ClientPacketManager : public ENetManager {
public:
  std::unique_ptr<Timer> frameTimer = std::make_unique<Timer>();
  std::mutex cMtx;
  GameState s1;
  GameState s2;
  GameState* sp1 = nullptr;
  GameState* sp2 = nullptr;
  bool stateOne = true;
  float millisecondsBetweenStates = -1;
  bool firstPacket = true;
  GamePacket *outgoingPacket = nullptr;
  Entity *me;
  ClientPacketManager(int frequency, Entity *m) {
    // std::string targetIP;
    // std::cout << "Enter IP to connect to: " << "\n";
    // std::cin >> targetIP;
    host = new ENetClient(9999, "localhost");
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
        // std::cout << "sending packet" << outgoingPacket->packet->data <<
        // "\n";
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
          // std::cout << "recieved packet: " << event.packet->data << "\n";
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
    me->id = (char)packetString[0] - '0';
    std::cout << "myID: " << me->id << "\n";
  }

  void UpdateGameStateFromPacket(std::string data) {
    // std::cout << "reached update game state " << "\n";
    if(stateOne){
      s1.ClientDeserialize(data);
      stateOne = false;
    }else{
      s2.ClientDeserialize(data);
      stateOne = true;
    }
    SetTimeBetweenStates();
  }
  void SetTimeBetweenStates(){
    frameTimer->Start();
    if(stateOne){
      millisecondsBetweenStates = s2.timeStamp - s1.timeStamp;
      sp1 = &s1;
      sp2 = &s2;
    }else{
      millisecondsBetweenStates = s1.timeStamp - s2.timeStamp;
      sp1 = &s2;
      sp2 = &s1;
    }
  }
  void CreateBroadCastPacket() {
    outgoingPacket = new GamePacket(me->Serialize());
  }
  void DeleteBroadCastPacket() { delete outgoingPacket; }
  
};
