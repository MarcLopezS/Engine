#pragma once
#include "Module.h"
#include "SDL/include/SDL.h"

#include "Math/float4x4.h"
#include "Geometry/Frustum.h"


class ModuleCamera :
    public Module
{
public: 
    ModuleCamera();
    ~ModuleCamera();

    bool Init();
    update_status Update();
    bool CleanUp();

    void LookAt(const float3& eye, const float3& target, const float3& up);
    void CalcProjMatrix();

    void SetFOV(); //Set horizontal FOV, keeping aspect ratio
    void SetAspectRatio(float newAspectRatio) { aspectRatio = newAspectRatio;  }
    void SetPlaneDistances(); // Set near and far planes
    void SetPosition(); //sets the position where the camera is placed.
    void Setorientation(); //sets the orientation of the camera
    void SetLookAt();// change the name of the function, tells where is looking the camera

    void MovKeyboardController(); //Movement controller by keyboard
    void MovMouseController(); //Movement controller by mouse

    float4x4 GetViewMatrix() const { return viewMatrix; }
    float4x4 GetProjMatrix() const { return projMatrix; }


private:

    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    Frustum frustum;

    float yaw;
    float pitch;

    float4x4 viewMatrix;
    float4x4 projMatrix;
    float4x4 rotationMatrix;

    float deltaTime;
    float lastFrame;
    float cameraSpeed;
    float rotationSpeed;
    float sensitivity;

};

