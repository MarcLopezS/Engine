#pragma once

#include<list>
#include <vector>
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

    void ShowPerformanceInfo();
    void ShowHardwareInfo();

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

    //performance methods & variables
    void UpdatePerformanceLogs();
    void DrawPerformanceGraphs();
    std::vector<float> fps_log;
    std::vector<float> ms_log;
    const int max_log_size = 100;

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
