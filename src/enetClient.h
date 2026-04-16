#include "camera.h"
#include <any>
#include <chrono>
#include "enetHelpers.h"
#include "includes.h"
#include <atomic>
#include <cstdio>
#include <enet/enet.h>
#include <iostream>
#include <mutex>
#include <regex>
#include <stdio.h>
#include <string>
#include <thread>

inline std::string FloatToString2Dec(float in) {
  char temp[50];
  std::snprintf(temp, sizeof(temp), "%.2f", in);
  std::string output(temp);
  return output;
}

class Player {

public:
  int id;
  float x;
  float y;
  float z;
  Player(int ID) { id = ID; }
};
class GameManager {
public:
  std::vector<Player> players;
  int id = -1;
  GameManager() {}
  void Update(char *info) {}
};
class EnetClient {
public:
  std::thread t_listener;
  std::thread t_sender;
  std::mutex pMtx;
  GameManager gm = GameManager();
  Camera *camera = nullptr;
  ENetHost *client = {0};
  ENetAddress address;
  ENetPeer *peer = {0};
  ENetEvent event = {ENET_EVENT_TYPE_NONE};
  ENetPacket *currentPacket = nullptr;
  int clientID = -1;
  EnetClient(Camera *cam) {
    camera = cam;
    if (enet_initialize() != 0) {
      std::cout << "cant initiliaze enet" << "\n";
    }

    client = enet_host_create(NULL, 1, 2, 0, 0);
    if (client == NULL) {
      std::cout << "Error creating client ENET" << "\n";
    }
    address = {ENET_HOST_ANY};
    event = {ENET_EVENT_TYPE_CONNECT};
    enet_address_set_host(&address, "127.0.0.5");
    address.port = 9999;
    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == NULL) {
      std::cout << "Error connecting with peer" << "\n";
    }
    if (enet_host_service(client, &event, 500) > 0) {
      if (event.type == ENET_EVENT_TYPE_CONNECT) {
      }
    }
    if (enet_host_service(client, &event, 1000) > 0) {
      if (event.type == ENET_EVENT_TYPE_RECEIVE) {
        std::cout << event.packet->data << "\n";
        char *thisID = new char[50];
        thisID[0] = static_cast<char>(event.packet->data[0]);
        gm.id = thisID[0] - '0';
        enet_packet_destroy(event.packet);
      }
    }
    std::cout << "This ID: " << gm.id << "\n";
  }
  void startListening() {
    t_listener = std::thread(&EnetClient::listenerListen, this);
  }
  void stopListening() { t_listener.join(); }
  void listenerListen() {
    while (true) {
      Listen();
    }
  }
  void startSending(Camera* cam) {
    t_sender = std::thread(&EnetClient::senderSend, this, cam);
  }
  void stopSending() { t_sender.join(); }
  void senderSend(Camera* cam) {
    while (true) {
      pMtx.lock();
      SendPacket(*cam);
      pMtx.unlock();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }

  void SendPacket(Camera cam) {
    event = {ENET_EVENT_TYPE_NONE};
    std::string packetString =
        std::to_string(gm.id) + FloatToString2Dec(cam.Position.x) +
        FloatToString2Dec(cam.Position.y) + FloatToString2Dec(cam.Position.z);

    char packetData[100] = "";
    strcat(packetData, packetString.c_str());
    ENetPacket *pack = enet_packet_create(&packetData, strlen(packetData) + 1,
                                          ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, pack);
  }
  void Listen() {
    char *temp = new char[100];
    if (enet_host_service(client, &event, 100) > 0) {
      if (event.type == ENET_EVENT_TYPE_RECEIVE) {
        pMtx.lock();
        std::string data = ReadBroadcastPlayerData(event.packet->data);
        std::cout << data << "\n";
        UpdatePlayersLists(data);
        pMtx.unlock();
      }
    }
  }
  void UpdatePlayersLists(std::string data) {
    gm.players.clear();
    int pCount = CharToInt(data[0]);
    int playerByteSize = 13;
    for (int i = 0; i < pCount; i++) {
      int position = (i * playerByteSize) + 1;
      int pId = CharToInt(data[position]);
      float pX = FourByteCharToFloat(data, position + 1);
      float pY = FourByteCharToFloat(data, position + 5);
      float pZ = FourByteCharToFloat(data, position + 9);
      // if(pId == gm.id) continue;
      Player p = Player(pId);
      p.x = pX;
      p.y = pY;
      p.z = pZ;
      gm.players.emplace_back(p);
      PrintPlayers();
    }
  }
  void PrintPlayers() {
    for (int i = 0; i < gm.players.size(); i++) {
      Player &p = gm.players[i];
      std::cout << "ID: " << p.id << "\n";
      std::cout << "X: " << p.x << "\n";
      std::cout << "Y: " << p.y << "\n";
      std::cout << "Z: " << p.z << "\n";
      std::cout << "\n";
    }
  }
};
