#include "assetManager.h"
#include "enetImplementation.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <memory>
#include "glfwSetup.h"
#include "widgets.h"
bool levelEditor = true;
GLFWInstance* game = nullptr;
int main() {
    if(levelEditor){
        game = new GLFWInstance();
        game->mouseEnabled = true;
    }
  std::shared_ptr<Sphere> light = std::make_shared<Sphere>(
      0, glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f),
      glm::vec3(0.0f), "Regular", 200, 5);
  std::shared_ptr<Plane> floor =
      std::make_shared<Plane>(0, glm::vec3(0.0f, -10.0f, 0.0f), glm::vec3(0.0f),
                              glm::vec3(1.0f), glm::vec3(0.0f), "Lighting");

  std::shared_ptr<Plane> wall1 =
      std::make_shared<Plane>(0, glm::vec3(50.0f, -10.0f, 0.0f), glm::vec3(0.0f),
                              glm::vec3(1.0f), glm::vec3(0.0f), "Lighting");
  std::shared_ptr<Plane> wall2 =
      std::make_shared<Plane>(0, glm::vec3(-50.0f, -10.0f, 0.0f), glm::vec3(0.0f),
                              glm::vec3(1.0f), glm::vec3(0.0f), "Lighting");
  std::shared_ptr<Plane> wall3 =
      std::make_shared<Plane>(0, glm::vec3(0.0f, -10.0f, 50.0f), glm::vec3(0.0f),
                              glm::vec3(1.0f), glm::vec3(0.0f), "Lighting");
  std::shared_ptr<Plane> wall4 =
      std::make_shared<Plane>(0, glm::vec3(0.0f, -10.0f, -50.0f), glm::vec3(0.0f),
                              glm::vec3(1.0f), glm::vec3(0.0f), "Lighting");
  light->lightSource = true;
  wall1->rotation.z = 90.0f;
  wall2->rotation.z = 90.0f;
  wall3->rotation.x = 90.0f;
  wall4->rotation.x = 90.0f;
  ServerPacketManager server = ServerPacketManager(100);
  server.gameState.AddEntity((light));
  server.gameState.AddEntity((floor));
  server.gameState.AddEntity((wall1));
  server.gameState.AddEntity((wall2));
  server.gameState.AddEntity((wall3));
  server.gameState.AddEntity((wall4));
  server.startListening();
  server.startSending();

  if(game == nullptr){
      while(true){}
  }else{
    while(!glfwWindowShouldClose(game->window)){
        game->FrameBegin();
        entityWidget(wall1);
        entityWidget(wall2);
        entityWidget(wall3);
        entityWidget(wall4);
        game->FrameEnd();
    }      
  }
  
  server.stopListening();
  server.stopSending();

  return -1;
}

