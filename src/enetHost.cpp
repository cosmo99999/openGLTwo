#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <enet/enet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "enetHelpers.h"
#include <thread>
#include <mutex>
class GLGamePlayer {
public:
  int id;
  std::string x = "";
  std::string y = "";
  std::string z = "";
  GLGamePlayer(int ID) { id = ID; }
};
class GLGameManager {
public:
  std::mutex pMtx;
  std::vector<GLGamePlayer> players;
  GLGameManager() {}
  ENetHost* host;
  void Update(unsigned char *data) {
    std::string strData = ReadPlayerData(data);    
    int pID = CharToInt(strData[0]);
    std::cout << "playerID from packet : " << pID << "\n";
    if (GetPlayerFromID(pID) != nullptr) {
      UpdatePlayerData(GetPlayerFromID(pID), strData);
    } else {
      std::cout << "Packet Recieved player unknown" << "\n";
    }
  }
  void UpdatePlayerData(GLGamePlayer *p, std::string data) {
    p->x = data.substr(1, 4);
    p->y = data.substr(5, 4);
    p->z = data.substr(9, 4);
  }

  void Broadcast() {
    if(players.size() < 1) return;
    std::string data = std::to_string(players.size());
    
    for (auto p : players) {
      if(p.x == "") return;
      std::cout << "playerID: " << p.id << "\n";
      std::cout << "x: " << p.x << "\n";
      std::cout << "y: " << p.y << "\n";
      std::cout << "z: " << p.z << "\n";
      data += std::to_string(p.id) +
              std::string(p.x) +
              std::string(p.y) +
              std::string(p.z);
    }
    const char *dataC = data.c_str();
    std::cout << "sending packet broadcast: " << dataC << "\n";
    std::cout << "players: " << std::to_string(players.size()) << "\n";
    ENetPacket *packet =
        enet_packet_create(dataC, strlen(dataC) + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(host, 0, packet);
    std::cout << "sent! " << "\n";
  }
  void SendNewPlayerID(ENetPeer *peer) {
    int newID = players.size() + 1;
    char idChar[50] = "";
    idChar[0] = newID + '0';
    std::cout << "sending new ID : " << idChar << "\n";
    ENetPacket *packet = enet_packet_create(idChar, strlen(idChar) + 1,
                                            ENET_PACKET_FLAG_RELIABLE);
    players.emplace_back(GLGamePlayer(newID));
    enet_peer_send(peer, 0, packet);
  }
  GLGamePlayer *GetPlayerFromID(int ID) {
    for (auto &player : players) {
      if (player.id == ID) {
        return &player;
      }
    }
    return nullptr;
  }
  void BroadcasterLoop(){
      while(true){
        pMtx.lock();
        Broadcast();
        pMtx.unlock();        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
  }
};

int main() {
  if (enet_initialize() != 0) {
    std::cout << "An error occurred while initializing ENet.\n";
    return 1;
  }

  ENetAddress address = {0};

  address.host = {
      ENET_HOST_ANY};  /* Bind the server to the default localhost.     */
  address.port = 9999; /* Bind the server to port 7777. */
  enet_address_set_host(&address, "127.0.0.5");
  /* create a server */
  ENetHost *server = enet_host_create(&address, 32, 2, 0, 0);

  if (server == NULL) {
    std::cout
        << "An error occurred while trying to create an ENet server host.\n";
    return 1;
  }

  std::cout << "Started a server...\n";

  /* Wait up to 1000 milliseconds for an event. (WARNING: blocking) */
  GLGameManager gManager = GLGameManager();
  gManager.host = server;
  int playerCounter = 0;
  std::thread t_broadcast = std::thread(&GLGameManager::BroadcasterLoop,std::ref(gManager));
  while (true) {
    ENetEvent event;

    while (enet_host_service(server, &event, 1000) > 0) {
      switch (event.type) {
      case ENET_EVENT_TYPE_NONE:
        std::cout << "event none" << "\n";
        break;
      case ENET_EVENT_TYPE_CONNECT:
        std::cout << "A new client connected from: " << event.peer->address.port
                  << "\n";
        gManager.SendNewPlayerID(event.peer);
        break;

      case ENET_EVENT_TYPE_RECEIVE:
        // std::cout << event.packet->data << "\n";
        gManager.pMtx.lock();
        gManager.Update(event.packet->data);
        gManager.pMtx.unlock();
        break;

      case ENET_EVENT_TYPE_DISCONNECT:
        std::cout << "%s disconnected.\n", event.peer->data;
        /* Reset the peer's client information. */
        event.peer->data = NULL;
        break;
      }
    }
  }
  t_broadcast.join();
  enet_host_destroy(server);
  enet_deinitialize();
  return 0;
}
// printf ("A packet of length %u containing %s was received from %x:%u on
// channel %u.\n",
//     event.packet -> dataLength,
//     event.packet -> data,
//     event.peer -> address.host,
//     event.peer -> address.port,
//     event.channelID);
