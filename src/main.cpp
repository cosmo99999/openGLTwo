#include "includes.h"
#include "cube.h"
#include <iostream>
extern Globals globals;
GLFWwindow* window;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = globals.SCR_WIDTH / 2.0f;
float lastY = globals.SCR_HEIGHT / 2.0f;
bool firstMouse = true;

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
    return 0;
}
int main()
{
    if(Config() == -1) return -1;
    glEnable(GL_DEPTH_TEST);
    Cube cube = Cube(10);
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        globals.deltaTime = currentFrame - globals.lastFrame;
        globals.lastFrame = currentFrame;
        processInput(window);
        glfwSetCursorPosCallback(window, mouse_callback);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
        // create transformations
        
        cube.UpDownAnimation();
        cube.Draw(camera);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
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