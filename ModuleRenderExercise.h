#pragma once
#include "Module.h"
#include "Globals.h"
#include "SDL/include/SDL.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/float3x3.h"
#include <string>


class ModuleProgram;

class ModuleRenderExercise :
    public Module
{
public: 
    ModuleRenderExercise();
    ~ModuleRenderExercise();

    bool Init();
    update_status Update();
    update_status PostUpdate();
    bool CleanUp();

    void CreateTriangleVBO();
    float4x4 LookAt(const float3& eye, const float3& target, const float3& up);
    float4x4 CalcProjectionMatrix(const float aspectRatio, const float nearPlane, const float farPlane);
    void DestroyVBO();

    ModuleProgram* GetProgram() { return program; }
    float4x4 GetViewMatrix() { return view; }
    float4x4 GetProjMatrix() { return proj; }


private:
    SDL_Renderer* renderer = nullptr;
    ModuleProgram* program = nullptr;
    unsigned int id_program;
    unsigned int vbo;
    float4x4 view;
    float4x4 proj;
    std::string vertexShader;
    std::string fragmentShader;

};

