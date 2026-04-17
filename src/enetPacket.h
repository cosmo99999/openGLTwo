#include "enetTemplates.h"
#include <cstring>
#include <enet/enet.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class Player {
public:
  int id;
  float x;
  float y;
  float z;
  mutable std::mutex* mtx;
  Player(int ID) {
     id = ID;
     mtx = new std::mutex;
  }
  ~Player(){
    delete mtx;
  }
};

class GamePacket {
public:
  ENetPacket *packet = nullptr;
  const char *packetData;

  int id = -1;
  std::string x;
  std::string y;
  std::string z;

  GamePacket(int id, float x, float y, float z) {
    if (id == -1)
      return;
    std::string packetString = std::to_string(id) + "," + std::to_string(x) +
                               "," + std::to_string(y) + "," +
                               std::to_string(z) + ",";
    packetData = packetString.c_str();
    packet = enet_packet_create(packetData, strlen(packetData) + 1,
                                ENET_PACKET_FLAG_RELIABLE);
  }
  GamePacket(std::vector<Player> players) {
    if (players.size() < 1)
      return;
    std::string packetString = std::to_string(players.size());
    packetString += ",";
    for (auto p : players) {
      if (p.x == -1)
        continue;
      packetString += std::to_string(p.id) + "," + std::to_string(p.x) + "," +
                      std::to_string(p.y) + "," + std::to_string(p.z) + ",";
    }
    packetData = packetString.c_str();
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
  std::vector<Player> players;
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
        std::cout << "sending packet with player count:  " << players.size() << "\n";
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
        ManageConnections(event.peer);
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
    UpdatePlayerFromPacketString(packetString);
  }

  void UpdatePlayerFromPacketString(std::string data) {
    int tempID = -1;
    float tempX = -1;
    float tempY = -1;
    float tempZ = -1;
    std::string tempData = "";
    for (int i = 0; i < data.length(); i++) {
      char thisChar = data[i];
      if (thisChar == ',') {
        if (tempID == -1) {
          tempID = std::stoi(tempData);
        } else if (tempX == -1) {
          tempX = std::stof(tempData);
        } else if (tempY == -1) {
          tempY = std::stof(tempData);
        } else if (tempZ == -1) {
          tempZ = std::stof(tempData);
        }
        tempData = "";
        continue;
      }
      tempData += data[i];
      // std::cout << "tempData " << tempData  << "\n";
    }

    // std::cout << "tempX " << tempX << "\n";
    // std::cout << "tempY " << tempY << "\n";
    // std::cout << "tempZ " << tempZ << "\n";
    for (int i = 0; i < players.size(); i++) {
      Player &thisPlayer = players[i];
      if (thisPlayer.id == tempID) {
        thisPlayer.x = tempX;
        thisPlayer.y = tempY;
        thisPlayer.z = tempZ;
        // std::cout << "Updated player" << tempID << "\n";
      }
    }
  }
  void CreateBroadCastPacket() { outgoingPacket = new GamePacket(players); }
  void ManageConnections(ENetPeer* peer) {
    int id = players.size() + 1;
    outgoingPacket = new GamePacket(id);
    peer->data = &id; 
    players.emplace_back(Player(id));
  }
  void DeleteBroadCastPacket() { delete outgoingPacket; }
};
class ClientPacketManager : public ENetManager {
public:
  std::mutex cMtx;
  std::vector<Player> players;
  bool firstPacket = true;
  int myID = -1;
  GamePacket *outgoingPacket = nullptr;
  float *pX;
  float *pY;
  float *pZ;
  ClientPacketManager(int frequency, float *x, float *y, float *z) {
    std::string targetIP;
    std::cout << "Enter IP to connect to: " << "\n";
    std::cin >> targetIP;
    host = new ENetClient(9999, targetIP);
    pX = x;
    pY = y;
    pZ = z;
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
        cMtx.lock();
        if (firstPacket) {
          RecieveConnectionPacket(event.packet);
          std::cout << "first packet: " << event.packet->data << "\n";
          firstPacket = false;
        } else {
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
    UpdatePlayersFromPacketString(packetString);
  }
  void RecieveConnectionPacket(ENetPacket *packet) {
    std::string packetString = StringFromUChar(packet->data);
    myID = (char)packetString[0] - '0';
  }

  void UpdatePlayersFromPacketString(std::string data) {
    Player::clientAccessingPlayerData.lock();
    players.clear();
    Player::clientAccessingPlayerData.unlock();
    int tempID = -1;
    float tempX = -1;
    float tempY = -1;
    float tempZ = -1;
    std::string tempData = "";
    // std::string first = data.substr(0, 1);
    // int playerAmount = std::stoi(first);c
    for (int i = 2; i < data.length(); i++) {
      char thisChar = data[i];
      // std::cout << "thischar : " << thisChar << "\n";
      if (thisChar == ',') {
        if (tempID == -1) {
          tempID = std::stoi(tempData);
        } else if (tempX == -1) {
          tempX = std::stof(tempData);
        } else if (tempY == -1) {
          tempY = std::stof(tempData);
        } else if (tempZ == -1) {
          tempZ = std::stof(tempData);

          Player p = Player(tempID);
          p.x = tempX;
          p.y = tempY;
          p.z = tempZ;
          players.emplace_back(p);
          tempX = -1;
          tempY = -1;
          tempZ = -1;
          tempID = -1;
          // std::cout << "p: " << p.id << "\n";
          // std::cout << "x: " << p.x << "\n";
          // std::cout << "z: " << p.z << "\n";
        }
        tempData = "";
        continue;
      }
      tempData += data[i];
      // std::cout << "tempData " << tempData << "\n";
    }
  }
  void CreateBroadCastPacket() {
    outgoingPacket = new GamePacket(myID, *pX, *pY, *pZ);
  }
  void DeleteBroadCastPacket() { delete outgoingPacket; }
  void PrintPlayers() {
    for (auto &p : players) {
      std::cout << "ID : " << p.id << "\n";
      std::cout << "x : " << p.x << "\n";
      std::cout << "y : " << p.y << "\n";
      std::cout << "z : " << p.z << "\n";
      std::cout << "\n";
    }
  }
};
