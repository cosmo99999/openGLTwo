#include "includes.h"
#include "shader.h"
#include "camera.h"
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distrib(1, 50);
extern Globals globals;
class Cube {

public:
    float vertices[36*5] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    
    std::vector<glm::vec3> positions;
    Shader shader = Shader("../shaders/vertexOne.txt", "../shaders/fragmentOne.txt");
    unsigned int VBO, VAO;
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    //animation
    float animationTimer = 0;
    bool animationUp = true;
    
    Cube(int amount){
        glm::vec3 newCube = glm::vec3(0.0f);
        for(int i = 0; i < amount; i++){
            newCube.x = distrib(rd);
            newCube.y = distrib(rd);
            newCube.z = distrib(rd);
            positions.emplace_back(newCube);
        }

        //generate vertex arrays and object
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    
        glBindVertexArray(VAO);
    
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void Draw(Camera camera){
        glm::mat4 view = camera.GetViewMatrix(); 
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)globals.SCR_WIDTH / (float)globals.SCR_HEIGHT, 0.1f, 100.0f);
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        glBindVertexArray(VAO);
        for(int i = 0; i < positions.size(); i++){
            model = glm::mat4(1.0f);
            model = glm::translate(model, positions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    void UpDownAnimation(){

        animationTimer += globals.deltaTime;
        float dif = 0.2f;
        
        for(int i = 0; i < positions.size(); i++){
            
            if(animationUp)
                positions[i].y += dif/100;
            if(!animationUp)
                positions[i].y -= dif/100;
            if(animationTimer > 2){
                animationTimer = 0;
                animationUp = !animationUp;
            }
        }
    }

};




// glm::vec3 Positions[10] = {
//     glm::vec3( 0.0f,  0.0f,  0.0f),
//     glm::vec3( 2.0f,  5.0f, -15.0f),
//     glm::vec3(-1.5f, -2.2f, -2.5f),
//     glm::vec3(-3.8f, -2.0f, -12.3f),
//     glm::vec3( 2.4f, -0.4f, -3.5f),
//     glm::vec3(-1.7f,  3.0f, -7.5f),
//     glm::vec3( 1.3f, -2.0f, -2.5f),
//     glm::vec3( 1.5f,  2.0f, -2.5f),
//     glm::vec3( 1.5f,  0.2f, -1.5f),
//     glm::vec3(-1.3f,  1.0f, -1.5f)
// };


// static double lastPrint = 0.0;
// double now = glfwGetTime();
// if (now - lastPrint > 0.2) { // 5 times a second
// std::cout << deltaTime << "\n";
// lastPrint = now;
// }