#pragma once
#include "Module.h"
#include "imgui.h"
#include <string>
#include <vector>
#include <queue>

class Model;


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
	void FlushPendingLogs();

	void RenderLogWindow();
	void DrawMenu();
	//void DrawProperties(const Model* model);
	void ShowInput();

	ImGuiIO* io;

private:
	void ShowAbout();
	void ShowSoftware();

	bool is_config_window;
	bool is_about_window;
		
	std::vector<std::string> _logs;
	std::queue<std::string> _pendingLogs;

	bool is_initialized = false;
	bool is_log_window = false;
	bool is_texture_window = false;
	bool is_properties_window = false;
	bool is_input_window = false;

	bool is_exit = false;

};

