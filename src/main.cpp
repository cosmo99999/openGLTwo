#include "globals.h"
#include "shapes.h"
#include <iostream>
#include "camera.h"
#include "assetManager.h"
#include <filesystem>
#include "widgets.h"
#include "includes.h"
#include "enetClient.h"
#include <thread>
Globals globals = Globals();
GLFWwindow* window;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, Object& obj);
void RendererEffects(Renderer& rn);
void GLFrameBegin();
void GLFrameEnd(GLFWwindow* window);
Camera camera(
    glm::vec3(0.0f, 5.0f, 10.0f), // position
    globals.SCR_WIDTH,             // scrWidth
    globals.SCR_HEIGHT             // scrHeight
);
AssetManager am;
float lastX = globals.SCR_WIDTH / 2.0f;
float lastY = globals.SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseEnabled = false;
float ambientLight = 0.1f;
float ambUp = true;
float backColour[4] = {0.5f, 0.5f, 0.5f, 1.0f};
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    ImGuiIO& io = ImGui::GetIO();
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go bottom->top
    
    lastX = xpos;
    lastY = ypos;
    if(!mouseEnabled){
        camera.ProcessMouseMovement(xoffset, yoffset, GL_TRUE);
        io.MouseDrawCursor = false;
    }
    else{
        io.MouseDrawCursor = true;
    }
};
void IMGUIStart();
void IMGUIEnd();
void ENETCLIENTUPDATE();
int Config(){
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
    window = glfwCreateWindow(globals.SCR_WIDTH, globals.SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glViewport(0, 0, globals.SCR_WIDTH, globals.SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    std::cout << "current working dir " << std::filesystem::current_path() << "\n"; 
    am.LoadShader("Regular","../../shaders/basic.shader");
    am.LoadShader("Lighting","../../shaders/lighting.shader");
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    return 0;
}
int main()
{
    if(Config() == -1) return -1;
    EnetClient eClient = EnetClient(&camera);
    Renderer renderer;
    Sphere light = Sphere(am.GetShader("Regular"), 20, 20, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    ObjectCollection c;
    
    
    
    c.Add(&light);
    for(int i = 0; i < 20; i++){
        Sphere* temp = new Sphere(am.GetShader("Lighting"), 10, 10, 1.0f, glm::vec3(0.0f));
        c.Add(temp);
    }
    c.RandomiseObjectColours();
    c.RandomiseScale();
    c.RandomiseObjectPositions(glm::vec2(-100.0f, 100.0f));
    eClient.startListening();
    eClient.startSending(&camera);
    while (!glfwWindowShouldClose(window))
    {
        GLFrameBegin();
        processInput(window, light);
        c.DrawAll(renderer, camera, light.position);
        colourWidget(backColour);
        GLFrameEnd(window);
    }
    eClient.stopListening();
    eClient.stopSending();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
void GLFrameBegin(){
    glClearColor(backColour[0], backColour[1], backColour[2], backColour[4]);
    IMGUIStart();
    float currentFrame = glfwGetTime();
    globals.deltaTime = currentFrame - globals.lastFrame;
    globals.lastFrame = currentFrame;
    if(globals.deltaTime > 0.5)
        globals.deltaTime = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
}
void ENETCLIENTUPDATE(){
}
void GLFrameEnd(GLFWwindow *window){
    IMGUIEnd();
    glfwSwapBuffers(window);
    glfwPollEvents();
}
void RendererEffects(Renderer& rn){
    Shader* sh = am.GetShader("Lighting");
    sh->use();
    if(ambUp){
        ambientLight += 0.0005f;
    }
    else{
        ambientLight -= 0.0005f;
    }
    if(ambientLight > 0.95f){
        ambUp = false;
    } 
    if(ambientLight < 0.1f){
        ambUp = true;
    } 
    sh->setFloat("ambientStrength", ambientLight);
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, Object& obj)
{
    std::vector<Camera_Movement> directions;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        obj.position.y = 20.0f;
        obj.moving = ObjectDirection::NONE;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, 1);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        directions.emplace_back(Camera_Movement::FORWARD);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        directions.emplace_back(Camera_Movement::BACKWARD);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        directions.emplace_back(Camera_Movement::LEFT);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        directions.emplace_back(Camera_Movement::RIGHT);
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS){
        if(mouseEnabled == false){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseEnabled = true;
        }else{
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseEnabled = false;
        }
    }
    camera.ProcessKeyboard(directions, globals.deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void IMGUIStart(){
    // (Your code calls glfwPollEvents())
    // ...
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    //ImGui::ShowDemoWindow(); // Show demo window! :)
}
void IMGUIEnd(){
        // Rendering
    // (Your code clears your framebuffer, renders your other stuff etc.)
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // (Your code calls glfwSwapBuffers() etc.)
};
