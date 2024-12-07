#pragma once
#include "Module.h"
#include "imgui.h"


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

	ImGuiIO* io;

private:
	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;
};

