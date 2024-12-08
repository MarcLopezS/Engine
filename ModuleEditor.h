#pragma once
#include "Module.h"
#include "imgui.h"
#include <string>
#include <vector>


class ModuleEditor :
    public Module
{
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void AddLog(const char* log);
	void RenderLogWindow();
	void DrawMenu();

	ImGuiIO* io;

private:
	void UpdatePerformanceLogs();
	void DrawPerformanceGraphs();
	void DrawWindowOptions();
	
	std::vector<float> fps_log;
	std::vector<float> ms_log;
	const int max_log_size = 100;

	bool show_config_window;
	bool show_window_options = false;
	bool show_demo_window;//demo
	bool show_another_window;//demo
	ImVec4 clear_color;//demo
	
	std::vector<std::string> logs;
	bool show_log_window = true;

	//window menu
	bool fullscreen = false;
	bool resizable = true;
	bool borderless = false;
	bool full_desktop = false;
	int window_width = 1280;
	int window_height = 720;
	float brightness = 1.0f;



	
};

