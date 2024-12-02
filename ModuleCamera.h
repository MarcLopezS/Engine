#pragma once
#include "Module.h"
#include "SDL/include/SDL.h"

#include "Math/float4x4.h"

class ModuleCamera :
    public Module
{
public: 
    ModuleCamera();
    ~ModuleCamera();

    bool Init();
    update_status Update();
    bool CleanUp();
    // detect keyboard event in this module

    void LookAt(const float3& eye, const float3& target, const float3& up);
    void CalcProjMatrix(const float aspectRatio, const float nearPlane, const float farPlane);

    void SetFOV(); //Set horizontal FOV, keeping aspect ratio
    void SetAspectRatio(); //Change vertical FOV to meet new aspect ratio
    void SetPlaneDistances(); // Set near and far planes
    void SetPosition(); //sets the position where the camera is placed.
    void Setorientation(); //sets the orientation of the camera
    void SetLookAt();// change the name of the function, tells where is looking the camera

    void MoveCamera(const float3& movement);
    float4x4 GetViewMatrix() const { return viewMatrix; }
    float4x4 GetProjMatrix() const { return projMatrix; }


private:

    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    float3 position;
    float3 target;

    float4x4 viewMatrix;
    float4x4 projMatrix;

    float deltaTime;
    float lastFrame;

 




};

