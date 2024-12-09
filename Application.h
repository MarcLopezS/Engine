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

    ModuleOpenGL* GetOpenGL() { return _render; }
    ModuleWindow* GetWindow() { return _window; }
    ModuleInput* GetModuleInput() { return _input; }
    ModuleOpenGL* GetRender() { return _render; }
    ModuleRenderExercise* GetModuleRenderExcercise() { return _renderEx; }
    ModuleCamera* GetModuleCamera() { return _camera; }
    ModuleDebugDraw* GetModuleDebugDraw() { return _debugDraw; }
    ModuleEditor* GetModuleEditor() { return _editor; }

    float GetDeltaTime() { return _deltaTime; }

private:
    void CalculateDeltaTime();
    
    Uint32 _lastFrameTime;
    float _deltaTime;

    //performance methods & variables
    void UpdatePerformanceLogs();
    void DrawPerformanceGraphs();
    std::vector<float> _fps_log;
    std::vector<float> _ms_log;
    const int _max_log_size = 100;

    ModuleOpenGL* _render = nullptr;
    ModuleRenderExercise* _renderEx = nullptr;
    ModuleWindow* _window = nullptr;
    ModuleInput* _input = nullptr;
    ModuleDebugDraw* _debugDraw = nullptr;
    ModuleCamera* _camera = nullptr;
    ModuleEditor* _editor = nullptr;

    std::list<Module*> modules;

};

extern Application* App;
