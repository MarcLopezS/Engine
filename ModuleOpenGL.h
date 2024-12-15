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
	void* GetContext() { return _context; }
	void WindowResized(unsigned width, unsigned height);

	void DrawWindowOptions();
	void DrawOpenGLInfo();

private:

	void* _context;

	//window options
	bool is_fullscreen = false;
	bool is_resizable = true;
	bool is_borderless = false;
	bool is_full_desktop = false;
	int _window_width = SCREEN_WIDTH;
	int _window_height = SCREEN_HEIGHT;
	float _brightness = 1.0f;
};
