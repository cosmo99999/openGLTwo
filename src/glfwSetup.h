#include "assetManager.h"
#include "camera.h"
#include "includes.h"
#include "widgets.h"
#include <GLFW/glfw3.h>
#include <filesystem>
#include <glm/exponential.hpp>
#include <iostream>
#include <thread>


void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

class GLFWInstance {
public:
  Camera *camera = nullptr;
  GLFWwindow *window = nullptr;
  AssetManager *am = nullptr;
  const unsigned int SCR_WIDTH = 1280;
  const unsigned int SCR_HEIGHT = 960;
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;
  float lastX = SCR_WIDTH / 2.0f;
  float lastY = SCR_HEIGHT / 2.0f;
  bool firstMouse = true;
  bool mouseEnabled = false;
  float ambientLight = 0.1f;
  float backColour[4] = {0.5f, 0.5f, 0.5f, 1.0f};

  int numFrames = -1;
  double lastTime = -1;
  double fps = -1;

  GLFWInstance() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    camera = new Camera(glm::vec3(0.0f, 5.0f, 10.0f), // position
                  SCR_WIDTH,            // scrWidth
                  SCR_HEIGHT            // scrHeight
    );
    
    camera->scrWidth = SCR_WIDTH;
    camera->scrHeight = SCR_HEIGHT;

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "Failed to initialize GLAD\n";
    }
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
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
    lastTime = glfwGetTime();
  }
  void FrameBegin() {
    glClearColor(backColour[0], backColour[1], backColour[2], backColour[4]);
    IMGUIStart();
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    if (deltaTime > 0.5)
      deltaTime = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  void FrameEnd() {
    IMGUIEnd();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  void IMGUIStart() {
    // (Your code calls glfwPollEvents())
    // ...
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    GetFrameRate();
    // ImGui::ShowDemoWindow(); // Show demo window! :)
  }
  void IMGUIEnd() {
    // Rendering
    // (Your code clears your framebuffer, renders your other stuff etc.)
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // (Your code calls glfwSwapBuffers() etc.)
  };
  void mouseInputHandler(double xpos, double ypos) {

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
      if(camera == nullptr){
        std::cout << "camera null" << "\n";
      }
      camera->ProcessMouseMovement(xoffset, yoffset, true);
      io.MouseDrawCursor = false;
    } else {
      io.MouseDrawCursor = true;
    }
  }
  void processInput(Entity *me) {
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

    camera->ProcessKeyboard(directions, deltaTime);
    me->position = camera->Position;
    me->velocity = camera->velocity;
  }
  void GetFrameRate(){
    double cTime = glfwGetTime();
    numFrames++;
    if(cTime - lastTime >= 1.0){
      fps = double(numFrames);
      numFrames = 0;
      lastTime += 1.0;  
    }
  }
};

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  GLFWInstance *instance = nullptr;
  instance = (GLFWInstance *)glfwGetWindowUserPointer(window);
  
  if(instance == nullptr){
    std::cout << "instance null" << "\n";
  }
  instance->mouseInputHandler(xpos, ypos);
};
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}
