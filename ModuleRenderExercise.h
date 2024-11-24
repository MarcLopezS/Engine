#pragma once
#include "Module.h"
#include "Globals.h"
#include "SDL/include/SDL.h"
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
    bool CleanUp();

    void RenderVBO();
    void CreateTriangleVBO();
    void DestroyVBO();

    ModuleProgram* GetProgram() { return program; }

private:
    SDL_Renderer* renderer = nullptr;
    ModuleProgram* program = nullptr;
    unsigned int id_program;
    unsigned int vbo;
    std::string vertexShader;
    std::string fragmentShader;

};

