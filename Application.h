#pragma once

#include<list>
#include "Globals.h"
#include "Module.h"

class ModuleOpenGL;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleRenderExercise;
class ModuleDebugDraw;
class ModuleCamera;

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

private:

    ModuleOpenGL* render = nullptr;
    ModuleRenderExercise* renderEx = nullptr;
    ModuleWindow* window = nullptr;
    ModuleInput* input = nullptr;
    ModuleDebugDraw* debugDraw = nullptr;
    ModuleCamera* camera = nullptr;

    std::list<Module*> modules;

};

extern Application* App;
