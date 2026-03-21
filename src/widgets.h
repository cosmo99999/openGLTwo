#pragma once
#include "includes.h"
#include "camera.h"
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

