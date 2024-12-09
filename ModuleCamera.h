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
    void SetAspectRatio(float newAspectRatio) { _aspectRatio = newAspectRatio;  }
    void SetPlaneDistances(); // Set near and far planes
    void SetPosition(); //sets the position where the camera is placed.
    void SetOrientation(); //sets the orientation of the camera
    void SetLookAt();// change the name of the function, tells where is looking the camera

    void MovKeyboardController(); //Movement controller by keyboard
    void MovMouseController(); //Movement controller by mouse

    float4x4 GetViewMatrix() const { return _viewMatrix; }
    float4x4 GetProjMatrix() const { return _projMatrix; }


private:

    float _fov;
    float _aspectRatio;
    float _nearPlane;
    float _farPlane;

    Frustum _frustum;

    float _yaw;
    float _pitch;

    float4x4 _viewMatrix;
    float4x4 _projMatrix;
    float4x4 _rotationMatrix;

    float _deltaTime;
    float _cameraSpeed;
    float _rotationSpeed;
    float _sensitivity;

};

