#include <enet/enet.h>
#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <string.h>
#include <cstring>

char* ParseData(char* data){
    char* read = nullptr; 
    // sscanf(data, "%[^\n]", &read);
    return read;
}
int main() {
    if (enet_initialize () != 0) {
        std::cout << "An error occurred while initializing ENet.\n";
        return 1;
    }

    ENetAddress address = {0};

    address.host = ENET_HOST_ANY; /* Bind the server to the default localhost.     */
    address.port = 7777; /* Bind the server to port 7777. */

    #define MAX_CLIENTS 32

    /* create a server */
    ENetHost * server = enet_host_create(&address, MAX_CLIENTS, 2, 0, 0);

    if (server == NULL) {
        std::cout << "An error occurred while trying to create an ENet server host.\n";
        return 1;
    }

     std::cout << "Started a server...\n";

     
     /* Wait up to 1000 milliseconds for an event. (WARNING: blocking) */
     int playerCounter = 0;
     while(true){
         ENetEvent event;
         ENetPacket* returnNumber = nullptr;
         char pNum[] = "";
         while (enet_host_service(server, &event, 1000) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    std::cout << "A new client connected from" << event.peer->address.port << "\n",  event.peer->address.host, event.peer->address.port;
                    /* Store any relevant client information here. */
                    event.peer->data = "Client information";
                    strcat(pNum, std::to_string(playerCounter).c_str());
                    returnNumber = enet_packet_create(pNum, strlen(pNum) + 1, ENET_PACKET_FLAG_RELIABLE);
                    enet_host_broadcast(server, 0, returnNumber);
                    // enet_packet_destroy(returnNumber);
                    break;
    
                case ENET_EVENT_TYPE_RECEIVE:
                std::cout<< event.packet->data << "\n";
                //enet_host_broadcast(server, 0, event.packet);
                enet_packet_destroy(event.packet);
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
// printf ("A packet of length %u containing %s was received from %x:%u on channel %u.\n",
//     event.packet -> dataLength,
//     event.packet -> data,
//     event.peer -> address.host,
//     event.peer -> address.port,
//     event.channelID);