#include <chrono>
#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <enet/enet.h>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <vector>

std::string StringFromUChar(unsigned char *data) {
  std::string result = "";

  for (int i = 0; i < strlen((char *)data); i++) {
    result += static_cast<char>(data[i]);
  }
  return result;
}

class ENetEntity {
public:
  ENetHost *host;
  ENetAddress address = {0};

  ENetEvent Listen() {
    ENetEvent event;
    std::string output;
    enet_host_service(host, &event, 0);
    return event;
  }
  virtual void SendPacket(ENetPacket *packet) = 0;
  virtual void SendPacket(ENetPacket *packet, ENetPeer* peer) = 0;

  virtual ~ENetEntity() {}
};
class ENetServer : public ENetEntity {
public:
  ENetServer(int port, std::string ip) {
    if (enet_initialize() != 0) {
      std::cout << "An error occurred while initializing ENet.\n";
      return;
    }
    address.host = {ENET_HOST_ANY};
    address.port = port;
    enet_address_set_host(&address, ip.c_str());
    host = enet_host_create(&address, 32, 2, 0, 0);
    if (host == NULL) {
      std::cout
          << "An error occurred while trying to create an ENet server host.\n";
      return;
    }
  }
  void SendPacket(ENetPacket *packet, ENetPeer* peer) override {
     enet_peer_send(peer, 0, packet);
  }
  void SendPacket(ENetPacket *packet) override {
     enet_host_broadcast(host, 0, packet);
  }
};

class ENetClient : public ENetEntity {
public:
  ENetPeer* peer;
  
  ENetClient(int port, std::string ip) {
    if (enet_initialize() != 0) {
      std::cout << "cant initiliaze enet" << "\n";
    }

    host = enet_host_create(NULL, 1, 2, 0, 0);
    if (host == NULL) {
      std::cout << "Error creating client ENET" << "\n";
    }
    address = {ENET_HOST_ANY};
    ENetEvent event;
    event = {ENET_EVENT_TYPE_CONNECT};
    enet_address_set_host(&address, ip.c_str());
    address.port = port;
    peer = enet_host_connect(host, &address, 2, 0);
    if (peer == NULL) {
      std::cout << "Error connecting with peer" << "\n";
    }
    if (enet_host_service (host, &event, 5000) > 0 &&
    event.type == ENET_EVENT_TYPE_CONNECT)
    {
      std::cout << "connected to server" << "\n";
    } 
  }

  void SendPacket(ENetPacket *packet) override {
     enet_peer_send(peer, 0, packet);
  }
  void SendPacket(ENetPacket *packet, ENetPeer* peer) override {
  }
};

class ENetManager {
public:
  ENetEntity *host;
  std::thread t_listener;
  std::thread t_sender;
  int sendFrequency = -1;

  virtual void Listen() = 0;
  virtual void Send() = 0;
  virtual void startListening() = 0;
  virtual void stopListening() = 0;
  virtual void startSending() = 0;
  virtual void stopSending() = 0;
};
