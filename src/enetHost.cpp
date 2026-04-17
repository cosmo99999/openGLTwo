#include "enetPacket.h"

int main(){
  ServerPacketManager server = ServerPacketManager(25);

  server.startListening();
  server.startSending();

  while(true){
    std::string in;
    std::cin >> in;
    if(in == "stop") break;
  }

  server.stopListening();
  server.stopSending();

  return 0; 
}
