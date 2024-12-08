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
	void DrawWindowOptions();
	
	bool show_config_window;
		
	std::vector<std::string> logs;
	bool show_log_window = true;

	//window menu
	bool fullscreen = false;
	bool resizable = true;
	bool borderless = false;
	bool full_desktop = false;
	int window_width = SCREEN_WIDTH;
	int window_height = SCREEN_HEIGHT;
	float brightness = 1.0f;



	
};

