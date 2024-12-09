#pragma once
#include "Module.h"
#include "Globals.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleOpenGL : public Module
{
public:
	ModuleOpenGL();
	~ModuleOpenGL();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void* GetContext() { return context; }
	void WindowResized(unsigned width, unsigned height);

	void DrawWindowOptions();

private:

	void* context;

	//window options
	bool fullscreen = false;
	bool resizable = true;
	bool borderless = false;
	bool full_desktop = false;
	int window_width = SCREEN_WIDTH;
	int window_height = SCREEN_HEIGHT;
	float brightness = 1.0f;
};
