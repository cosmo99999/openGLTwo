#include "assetManager.h"
#include "enetImplementation.h"
#include <imgui.h>
#include <memory>
int main() {
  Sphere* light = new Sphere(glm::vec3(0.0f), glm::vec3(0.0f), "Regular", 20, 5);
  Plane* plane = new Plane(glm::vec3(0.0f), glm::vec3(10.0f), "Lighting");
      
  ServerPacketManager server = ServerPacketManager(25);
  server.gameState.entities.emplace_back(light);
  server.gameState.entities.emplace_back(plane);
  server.startListening();
  server.startSending();

  while (true) {
    std::string in;
    std::cin >> in;
    if (in == "stop")
      break;
  }

  server.stopListening();
  server.stopSending();

  return 0;
}
