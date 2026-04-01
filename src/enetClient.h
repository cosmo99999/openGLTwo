#include <enet/enet.h>
#include <iostream>
#include "includes.h"
#include <stdio.h>
#include <cstdio>

class Packet {
public:
    float x;
    float y;
    float z;
    Packet(){};
    Packet(float X, float Y, float Z){
        x = X;
        y = Y;
        z = Z;
    }
};
class EnetClient {
public:
    ENetHost* client = { 0 };
    ENetAddress address;
    ENetPeer* peer = {0};
    ENetEvent event = {ENET_EVENT_TYPE_NONE};
    ENetPacket* currentPacket = nullptr;
    int clientID = -1;
    EnetClient(){
        if(enet_initialize() != 0){
            std::cout << "cant initiliaze enet" << "\n";
        }

        client = enet_host_create(NULL, 1, 2, 0, 0);
        if(client == NULL){
            std::cout << "Error creating client ENET" << "\n";
        }
        address = { ENET_HOST_ANY };
        event = { ENET_EVENT_TYPE_CONNECT };
        enet_address_set_host(&address, "127.0.0.1");
        address.port = 7777;
        peer = enet_host_connect(client, &address, 2, 0);
        if(peer == NULL){
            std::cout << "Error connecting with peer" << "\n";
        }
        if(enet_host_service(client, &event, 1000) > 0){
            if(event.type == ENET_EVENT_TYPE_RECEIVE){
                std::cout << event.packet->data << "\n";
                enet_packet_destroy(event.packet);
            }
        }
        std::cout << clientID << "\n";
    }
    void SendPacket(Camera cam){
        event = { ENET_EVENT_TYPE_NONE };
        std::string packetString = std::to_string(cam.Position.x) + std::to_string(cam.Position.y) + std::to_string(cam.Position.z);
        char packetData[100] = "";
        strcat(packetData, packetString.c_str());
        ENetPacket* pack = enet_packet_create(&packetData, strlen(packetData) + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, pack);
        if(enet_host_service(client, &event, 100) > 0){
            if(event.type == ENET_EVENT_TYPE_RECEIVE){
                enet_packet_destroy(event.packet);
            }
        }
    }
};