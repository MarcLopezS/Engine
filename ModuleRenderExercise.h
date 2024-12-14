#pragma once
#include "Module.h"
#include "Globals.h"
#include "SDL/include/SDL.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/float3x3.h"
#include <string>


class ModuleProgram;
class Moduletexture;
class Model;

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

    void CreateQuadVBO();
    void DestroyVBO();

    ModuleProgram* GetProgram() { return _program; }
    float4x4 GetViewMatrix() const { return _view; }
    float4x4 GetProjMatrix() const { return _proj; }


private:
    SDL_Renderer* _renderer = nullptr;
    ModuleProgram* _program = nullptr;
    Model* _model = nullptr;
    
    unsigned int _id_program;
    unsigned int _vbo;
    unsigned int _ebo;
   
    float4x4 _view;
    float4x4 _proj;
    
    std::string _vertexShader;
    std::string _fragmentShader;

};

