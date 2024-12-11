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
	//void ShowInput();

	ImGuiIO* io;

private:
	void ShowAbout();

	bool is_config_window;
	bool is_about_window;
		
	std::vector<std::string> _logs;
	bool is_log_window;
	bool is_texture_window;

};

