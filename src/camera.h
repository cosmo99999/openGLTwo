#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#include "includes.h"
#include "glfw/glfw3.h"
// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    NONE
};
enum class MovementStatus {
    Accelerating,
    Decelerating,
    InitAcceleration,
    None
};
inline std::string getStatusName(MovementStatus ms){
    if(ms == MovementStatus::Accelerating)
        return "Accelerating";
    if(ms == MovementStatus::InitAcceleration)
        return "Init Acceleration";
    if(ms == MovementStatus::Decelerating)
        return "Decelerating";
    if(ms == MovementStatus::None)
        return "None";
    return "NULL";
}
inline bool shouldClamp(float speed, float max){
    if(speed > 0 && speed > max ||
       speed < 0 && speed < max ){
        return true;
    }
    return false;
};
inline bool shouldClamp(float speed){
    if(speed > 0 && speed - 5 < 0 ||
       speed < 0 && speed + 5 > 0 ){
        return true;
    }
    return false;
};
// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float moveSpeed = 10.0f;
    float MouseSensitivity;
    float Zoom;
    //movement
    glm::vec3 velocity;
    glm::vec3 wishDir = glm::vec3(0.0f);
    float moveBias = 0.5f;
    float mBBase = 0.5f;
    float mBMin = 0.1f;
    float mBMax = 0.9f;
    float acceleration = 100.0f;
    float deceleration = 20.0f;
    float speedMulti = 0.0f;
    float maxSpeed = 20.0f;
    float scrWidth = 0.0f;
    float scrHeight = 0.0f;
    float FOV = 90;
    MovementStatus mStatus = MovementStatus::None;
    Camera(glm::vec3 position, float width, float height) 
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
      moveSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        Yaw = YAW;
        Pitch = PITCH;
        velocity = glm::vec3(0.0f);
        wishDir = Front;
        scrWidth = width;
        scrHeight = height;
        updateCameraVectors();
    }
    glm::mat4 GetProjection(){
        float nearPlane = 0.1f;
        float farPlane = 200.0f;
        float aspectRatio = scrWidth/scrHeight;

        return glm::perspective(glm::radians(FOV), aspectRatio, nearPlane, farPlane);
    }
    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }
    
    glm::vec3 GetWishDirection(std::vector<Camera_Movement> directions){
        
        glm::vec3 flatFront = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));
        glm::vec3 flatRight = glm::normalize(glm::vec3(Right.x, 0.0f, Right.z));
    
        glm::vec3 wishDir = glm::vec3(0.0f);

        for(auto& dir : directions){
            if (dir == Camera_Movement::FORWARD) wishDir += flatFront;
            if (dir == Camera_Movement::BACKWARD) wishDir -= flatFront;
            if (dir == Camera_Movement::RIGHT) wishDir += flatRight;
            if (dir == Camera_Movement::LEFT) wishDir -= flatRight;
        }
        if (glm::length(wishDir) > 0)
        wishDir = glm::normalize(wishDir);

        return wishDir;
    }
    void UpdatePosition(std::vector<Camera_Movement> directions, float deltaTime){
        
        moveSpeed = glm::length(velocity);
        wishDir = GetWishDirection(directions);
        float speedModifier = 0.0f;
        float currentSpeed = 1.0f;
        if(glm::length(velocity) > 0)
            currentSpeed = glm::dot(glm::normalize(velocity), wishDir);

        if(glm::length(wishDir) > 0.0f){
            speedModifier = (maxSpeed - currentSpeed) * acceleration * deltaTime;
        }else{
            velocity *= 0.95f;
            if(glm::length(velocity) < 0.5f)
                velocity = glm::vec3(0.0f);
        }
        velocity += speedModifier * wishDir;
        if(glm::length(velocity) > maxSpeed){
            velocity = glm::normalize(velocity) * maxSpeed;
        }
        Position += velocity * deltaTime;
    }
    
    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(std::vector<Camera_Movement> directions, float deltaTime)
    {
        UpdatePosition(directions, deltaTime);
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif