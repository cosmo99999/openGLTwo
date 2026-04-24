#include "assetManager.h"
#include "camera.h"
#include "enetImplementation.h"
#include "entity.h"
#include "globals.h"
#include "includes.h"
#include "widgets.h"
#include <filesystem>
#include <iostream>
#include <thread>
Globals globals = Globals();
GLFWwindow *window;
AssetManager* am;
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, Entity *me);
void RendererEffects(Renderer &rn);
void GLFrameBegin();
void GLFrameEnd(GLFWwindow *window);
Camera camera(glm::vec3(0.0f, 5.0f, 10.0f), // position
              globals.SCR_WIDTH,            // scrWidth
              globals.SCR_HEIGHT            // scrHeight
);
float lastX = globals.SCR_WIDTH / 2.0f;
float lastY = globals.SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseEnabled = false;
float ambientLight = 0.1f;
float ambUp = true;
float backColour[4] = {0.5f, 0.5f, 0.5f, 1.0f};
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  ImGuiIO &io = ImGui::GetIO();
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // reversed since y-coordinates go bottom->top

  lastX = xpos;
  lastY = ypos;
  if (!mouseEnabled) {
    camera.ProcessMouseMovement(xoffset, yoffset, true);
    io.MouseDrawCursor = false;
  } else {
    io.MouseDrawCursor = true;
  }
};
void IMGUIStart();
void IMGUIEnd();

void ENETCLIENTUPDATE();
int Config() {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  camera.scrWidth = globals.SCR_WIDTH;
  camera.scrHeight = globals.SCR_HEIGHT;

  // glfw window creation
  // --------------------
  window = glfwCreateWindow(globals.SCR_WIDTH, globals.SCR_HEIGHT,
                            "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD\n";
    return -1;
  }
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glViewport(0, 0, globals.SCR_WIDTH, globals.SCR_HEIGHT);
  glEnable(GL_DEPTH_TEST);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  std::cout << "current working dir " << std::filesystem::current_path()
            << "\n";
  // Setup Dear ImGui context
  am = new AssetManager();
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(
      window, true); // Second param install_callback=true will install GLFW
                     // callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init();

  return 0;
}
int main() {
  if (Config() == -1)
    return -1;

  Player me = Player(-100, glm::vec3(0.0f), glm::vec3(0.0f), "Lighting");
  Renderer renderer;
  ClientPacketManager cManager = ClientPacketManager(25, &me);
  cManager.startListening();
  cManager.startSending();

  std::cout << "host address" << cManager.host->address.host << "\n";
  std::cout << "host port" << cManager.host->address.port << "\n";

  while (!glfwWindowShouldClose(window)) {
    GLFrameBegin();
    processInput(window, &me);
    cManager.cMtx.lock();
    if (cManager.gameState.lightSources.size() > 0) {
      glm::vec3 lightpos = cManager.gameState.lightSources[0]->position;
      for (auto &e : cManager.gameState.entities) {
        e->LoadMesh(am);
        e->Draw(renderer, camera, lightpos);
      }
    }

    cManager.cMtx.unlock();
    colourWidget(backColour);
    GLFrameEnd(window);
  }
  cManager.stopListening();
  cManager.stopSending();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();
  return 0;
}
void GLFrameBegin() {
  glClearColor(backColour[0], backColour[1], backColour[2], backColour[4]);
  IMGUIStart();
  float currentFrame = glfwGetTime();
  globals.deltaTime = currentFrame - globals.lastFrame;
  globals.lastFrame = currentFrame;
  if (globals.deltaTime > 0.5)
    globals.deltaTime = 0;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void ENETCLIENTUPDATE() {}
void GLFrameEnd(GLFWwindow *window) {
  IMGUIEnd();
  glfwSwapBuffers(window);
  glfwPollEvents();
}
// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, Entity *me) {
  std::vector<Camera_Movement> directions;

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    directions.emplace_back(Camera_Movement::FORWARD);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    directions.emplace_back(Camera_Movement::BACKWARD);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    directions.emplace_back(Camera_Movement::LEFT);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    directions.emplace_back(Camera_Movement::RIGHT);
  }
  if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
    if (mouseEnabled == false) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      mouseEnabled = true;
    } else {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      mouseEnabled = false;
    }
  }
  camera.ProcessKeyboard(directions, globals.deltaTime);
  me->position = camera.Position;
  me->velocity = camera.velocity;
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}
void IMGUIStart() {
  // (Your code calls glfwPollEvents())
  // ...
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  // ImGui::ShowDemoWindow(); // Show demo window! :)
}
void IMGUIEnd() {
  // Rendering
  // (Your code clears your framebuffer, renders your other stuff etc.)
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  // (Your code calls glfwSwapBuffers() etc.)
};
