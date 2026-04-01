#pragma once
#include "includes.h"
#include "camera.h"
#include "shapes.h"
#include <iostream>
#include "globals.h"
extern Globals globals;
float widgetTimer = 0.0f;
void cameraWidget(Camera& camera){

    int counter = 0;
    ImGui::Begin("Camera Widget");                          // Create a window called "Hello, world!" and append into it.
    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    ImGui::SliderFloat("FOV", &camera.FOV, 10.0f, 150.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::SliderFloat("BIAS", &camera.moveBias, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    // ImGui::Text("Camera velocity: ");
    // std::string camVelocity = "x: " + std::to_string(camera.velocity.x) + " y: " + std::to_string(camera.velocity.y) + " z: " +  std::to_string(camera.velocity.z);
    // ImGui::Text(camVelocity.c_str());
    // ImGui::Text("Camera pos: ");
    // std::string camPos = "x: " + std::to_string(camera.Position.x) + " y: " + std::to_string(camera.Position.y) + " z: " +  std::to_string(camera.Position.z);
    // ImGui::Text(camPos.c_str());
    ImGui::Text("Camera Speed: ");
    std::string speed = std::to_string(camera.moveSpeed);
    ImGui::Text(speed.c_str());
    // ImGui::Text("Camera SpeedMulti: ");
    // std::string speedMult = std::to_string(camera.speedMulti);
    // ImGui::Text(speedMult.c_str());
    // ImGui::Text("Camera STATUS: ");
    // std::string status = getStatusName(camera.mStatus);
    // ImGui::Text(status.c_str());
    ImGui::End();
    
}
void colourWidget(float* c){

    int counter = 0;
    ImGui::Begin("BackColour");                              
    ImGui::SliderFloat("R", &c[0], 0.0f, 1.0f);                  
    ImGui::SliderFloat("G", &c[1], 0.0f, 1.0f);                  
    ImGui::SliderFloat("B", &c[2], 0.0f, 1.0f);                  
    ImGui::SliderFloat("A", &c[3], 0.0f, 1.0f);                  
    ImGui::End();
    
}
void shapeWidget(Object* obj){

    int counter = 0;
    ImGui::Begin("Obj");                              
    ImGui::SliderFloat("Rotation X: ", &obj->rotation.x, -10.0f, 10.0f);                  
    ImGui::SliderFloat("Rotation Y: ", &obj->rotation.y, 0.0f, 1.0f);                  
    ImGui::SliderFloat("Rotation Z: ", &obj->rotation.z, 0.0f, 1.0f);                                    
    ImGui::End();
    
}
