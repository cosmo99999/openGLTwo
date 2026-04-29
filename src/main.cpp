#include "enetImplementation.h"
#include "entity.h"
#include "glfwSetup.h"
#include <iostream>
#include <thread>

int main() {
  GLFWInstance game = GLFWInstance();
  Player me = Player(-100, glm::vec3(50.0f, 0.0f, 0.0f), glm::vec3(0.0f),
                     glm::vec3(5.0f), glm::vec3(0.0f), "Lighting");
  Renderer renderer;
  ClientPacketManager cManager = ClientPacketManager(100, &me);
  cManager.startListening();
  cManager.startSending();
  std::cout << "host address" << cManager.host->address.host << "\n";
  std::cout << "host port" << cManager.host->address.port << "\n";
  while (!glfwWindowShouldClose(game.window)) {
    if(cManager.sp1 == nullptr || cManager.sp2 == nullptr) continue;
    game.FrameBegin();
    game.processInput(&me);
    double fTime = cManager.frameTimer->ElapsedMilliseconds();
    double alpha = fTime / cManager.millisecondsBetweenStates;
    cManager.cMtx.lock();
    me.UpdateRotation(game.camera);
    if (cManager.sp1->lightSources.size() > 0) {
      glm::vec3 lightpos = cManager.sp1->lightSources[0]->position;
      for (auto &e : cManager.sp1->entities) {
        auto tempE = cManager.sp2->GetEntityById(e->id);
        if (e->id == me.id || tempE == nullptr)
          continue;
        if (!e->meshLoaded) {
          e->LoadMesh(game.am);
          e->meshLoaded = true;
          std::cout << "loading mesh" << "\n";
        }
        e->SetLerps(tempE, alpha);
        e->Draw(renderer, *game.camera, lightpos);
        if (e->entityName == "Player") {
          // std::cout << "s1 pos: " << stringFromVec3(e->position) << "\n";
          // std::cout << "s2 pos: " << stringFromVec3(tempE->position) << "\n";
          // std::cout << "\n";
        }
      }
    }

    cManager.cMtx.unlock();
    game.FrameEnd();
  }
  cManager.stopListening();
  cManager.stopSending();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();
  return 0;
}
