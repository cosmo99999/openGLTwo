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

class GLGamePlayer {
public:
  int id;
  char *x = new char[4];
  char *y = new char[4];
  char *z = new char[4];
  GLGamePlayer(int ID) { id = ID; }
};
class GLGameManager {
public:
  std::vector<GLGamePlayer> players;
  GLGameManager() {}

  void Update(unsigned char *data) {
    char *pData = new char[13];
    for (int i = 0; i < 13; i++) {
      pData[i] = static_cast<char>(data[i]);
    }
    std::cout << "Packet recieved from player : " << pData[0] << "\n";
    int pID = pData[0] - '0';
    if (GetPlayerFromID(pID) != nullptr) {
      UpdatePlayerData(GetPlayerFromID(pID), pData);
    } else {
      std::cout << "Packet Recieved player unknown" << "\n";
    }
  }
  void UpdatePlayerData(GLGamePlayer *p, char *data) {
    char *xChar = new char[4];
    char *yChar = new char[4];
    char *zChar = new char[4];
    for (int i = 1; i < 5; i++) {
      xChar[i] = static_cast<char>(data[i]);
      yChar[i] = static_cast<char>(data[i + 4]);
      zChar[i] = static_cast<char>(data[i + 8]);
    }
    delete[] p->x;
    delete[] p->y;
    delete[] p->z;
    p->x = new char[strlen(xChar)];
    std::strcpy(p->x, xChar);
    p->y = new char[strlen(yChar)];
    std::strcpy(p->y, yChar);
    p->z = new char[strlen(zChar)];
    std::strcpy(p->z, zChar);
    delete[] xChar;
    delete[] yChar;
    delete[] zChar;
  }
  void Broadcast(ENetHost *host) {
    std::string data = std::to_string(players.size());
    for (auto p : players) {
      data += std::to_string(p.id) +
              std::string(p.x) +
              std::string(p.y) +
              std::string(p.z);
    }
    const char *dataC = data.c_str();
    std::cout << "sending packet broadcast: " << dataC << "\n";
    ENetPacket *packet =
        enet_packet_create(dataC, strlen(dataC) + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(host, 0, packet);
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
  int playerCounter = 0;
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
        std::cout << event.packet->data << "\n";
        gManager.Update(event.packet->data);
        gManager.Broadcast(server);
        break;

      case ENET_EVENT_TYPE_DISCONNECT:
        std::cout << "%s disconnected.\n", event.peer->data;
        /* Reset the peer's client information. */
        event.peer->data = NULL;
        break;
      }
    }
  }

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
