#include "globals.h"
#include "shapes.h"
#include <iostream>
#include "camera.h"
#include "assetManager.h"

Globals globals = Globals();
GLFWwindow* window;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void RendererEffects(Renderer& rn);

Camera camera(
    glm::vec3(0.0f, 0.0f, 3.0f), // position
    globals.SCR_WIDTH,             // scrWidth
    globals.SCR_HEIGHT             // scrHeight
);
AssetManager am;

float lastX = globals.SCR_WIDTH / 2.0f;
float lastY = globals.SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float ambientLight = 0.1f;
float ambUp = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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
    
    camera.ProcessMouseMovement(xoffset, yoffset, GL_TRUE);
};

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glViewport(0, 0, globals.SCR_WIDTH, globals.SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
    
    am.LoadShader("Regular","../shaders/cubeVert.txt", "../shaders/cubeFrag.txt");
    am.LoadShader("Lighting","../shaders/lightVert.txt", "../shaders/lightFrag.txt");

    return 0;
}
int main()
{
    if(Config() == -1) return -1;

    ObjectCollection objects;
    Cube light = Cube(am.GetShader("Regular"), glm::vec3(0.0f, 0.0f, -2.0f));
    Sphere s1 = Sphere(am.GetShader("Lighting"), 20, 20, 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));
    Sphere s2 = Sphere(am.GetShader("Lighting"), 20, 20, 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));
    Sphere s3 = Sphere(am.GetShader("Lighting"), 20, 20, 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));
    Sphere s4 = Sphere(am.GetShader("Lighting"), 20, 20, 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));
    Sphere s5 = Sphere(am.GetShader("Lighting"), 20, 20, 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));
    Sphere s6 = Sphere(am.GetShader("Lighting"), 20, 20, 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));
    Sphere s7 = Sphere(am.GetShader("Lighting"), 20, 20, 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));
    Sphere s8 = Sphere(am.GetShader("Lighting"), 20, 20, 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));

    objects.Add(&light);
    objects.Add(&s1);
    objects.Add(&s2);
    objects.Add(&s3);
    objects.Add(&s4);
    objects.Add(&s5);
    objects.Add(&s6);
    objects.Add(&s7);
    objects.Add(&s8);

    objects.RandomiseObjectPositions(glm::vec2(-55.0f, 55.0f));
    objects.RandomiseObjectColours();
    objects.RandomiseScale();
    Renderer renderer;
    while (!glfwWindowShouldClose(window))
    {
        RendererEffects(renderer);
        float currentFrame = glfwGetTime();
        globals.deltaTime = currentFrame - globals.lastFrame;
        globals.lastFrame = currentFrame;
        processInput(window);
        glfwSetCursorPosCallback(window, mouse_callback);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
        static bool printed = false;
        objects.DrawAll(renderer, camera, light.position);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
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
void processInput(GLFWwindow *window)
{
    Camera_Movement moveDir = NONE;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, 1);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        moveDir = FORWARD;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        moveDir = BACKWARD;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        if(moveDir == FORWARD) moveDir = FORWARDLEFT;
        else if(moveDir == BACKWARD) moveDir = BACKWARDLEFT;
        else moveDir = LEFT;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        if(moveDir == FORWARD) moveDir = FORWARDRIGHT;
        else if(moveDir == BACKWARD) moveDir = BACKWARDRIGHT;
        else moveDir = RIGHT;
    }
    // static double lastPrint = 0.0;
    // double now = glfwGetTime();
    // if (now - lastPrint > 0.2) { // 5 times a second
    // std::cout << camera.velocity << "\n";
    // lastPrint = now;
    // }
    camera.ProcessKeyboard(moveDir, globals.deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}