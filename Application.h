#pragma once

#include<list>
#include "Globals.h"
#include "Module.h"
#include "SDL.h"

class ModuleOpenGL;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleRenderExercise;
class ModuleDebugDraw;
class ModuleCamera;
class ModuleEditor;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

    ModuleOpenGL* GetOpenGL() { return render; }
    ModuleWindow* GetWindow() { return window; }
    ModuleInput*  GetInput() { return input; }
    ModuleOpenGL* GetRender() { return render; }
    ModuleRenderExercise* GetModuleRenderExcercise() { return renderEx; }
    ModuleCamera* GetModuleCamera() { return camera; }
    ModuleDebugDraw* GetModuleDebugDraw() { return debugDraw; }
    ModuleInput* GetModuleInput() { return input; }
    ModuleEditor* GetModuleEditor() { return editor; }

    float GetDeltaTime() { return deltaTime; }

private:
    void CalculateDeltaTime();
    Uint32 lastFrameTime;
    float deltaTime;

    ModuleOpenGL* render = nullptr;
    ModuleRenderExercise* renderEx = nullptr;
    ModuleWindow* window = nullptr;
    ModuleInput* input = nullptr;
    ModuleDebugDraw* debugDraw = nullptr;
    ModuleCamera* camera = nullptr;
    ModuleEditor* editor = nullptr;

    std::list<Module*> modules;

};

extern Application* App;
