#include "Globals.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleWindow.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"
#include "SDL.h"
#include "glew-2.1.0\include\GL\glew.h"
#include "imgui.h"

ModuleOpenGL::ModuleOpenGL()
{
}

// Destructor
ModuleOpenGL::~ModuleOpenGL()
{
}

// Called before render is available
bool ModuleOpenGL::Init()
{
	LOG("Creating Renderer context");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	if (_context) {
		SDL_GL_DeleteContext(_context);
		_context = nullptr;
	}

	_context = SDL_GL_CreateContext(App->GetWindow()->window);
	
	if (SDL_GL_SetSwapInterval(0) != 0) {
		LOG("Warning: Unable to disable V-Sync! SDL Error: %s", SDL_GetError());
	}
	else {
		LOG("V-Sync disabled");
	}
	
	GLenum err = glewInit();

	if (err != GLEW_OK) {
		LOG("Error initializing GLEW: %s", glewGetErrorString(err));
		return false;
	}

	LOG("Using Glew %s", glewGetString(GLEW_VERSION));

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	return true;
}

update_status ModuleOpenGL::PreUpdate()
{
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleOpenGL::Update()
{

	return UPDATE_CONTINUE;
}

update_status ModuleOpenGL::PostUpdate()
{
	if (App->GetModuleEditor()->io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

	SDL_GL_SwapWindow(App->GetWindow()->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleOpenGL::CleanUp()
{
	LOG("Destroying renderer");

	//Destroy window
	if (_context) {
		SDL_GL_DeleteContext(_context);
		_context = nullptr;
	}

	return true;
}

void ModuleOpenGL::WindowResized(unsigned width, unsigned height)
{
	if (width > 0 && height > 0)
		glViewport(0, 0, width, height);
}

void ModuleOpenGL::DrawWindowOptions()
{
	if (ImGui::CollapsingHeader("Window"))
	{
		if (ImGui::SliderFloat("Brightness", &_brightness, 0.0f, 1.0f))
			SDL_SetWindowBrightness(App->GetWindow()->window, _brightness);

		if (!is_resizable)
			ImGui::BeginDisabled();

		static int temp_width = _window_width;
		static int temp_height = _window_height;

		ImGui::SliderInt("Width", &temp_width, 640, SCREEN_WIDTH);
		ImGui::SliderInt("Height", &temp_height, 480, SCREEN_HEIGHT);

		if (!is_resizable)
			ImGui::EndDisabled();

		if (ImGui::Button("Apply"))
		{
			if ((temp_width != _window_width || temp_height != _window_height) && is_resizable)
			{
				_window_width = temp_width;
				_window_height = temp_height;

				SDL_SetWindowSize(App->GetWindow()->window, _window_width, _window_height);
				SDL_Delay(1000);
				App->GetOpenGL()->WindowResized(_window_width, _window_height);
				App->GetModuleCamera()->SetAspectRatio(static_cast<float>(_window_width) / _window_height);
				App->GetModuleCamera()->CalcProjMatrix();
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset"))
		{
			temp_width = SCREEN_WIDTH;
			temp_height = SCREEN_HEIGHT;
		}

		if (ImGui::Checkbox("Fullscreen", &is_fullscreen))
		{
			if (is_fullscreen)
				SDL_SetWindowFullscreen(App->GetWindow()->window, SDL_WINDOW_FULLSCREEN);
			else
				SDL_SetWindowFullscreen(App->GetWindow()->window, 0);
		}

		ImGui::SameLine();

		if (ImGui::Checkbox("Resizable", &is_resizable))
			SDL_SetWindowResizable(App->GetWindow()->window, (SDL_bool)is_resizable);

		if (ImGui::Checkbox("Borderless", &is_borderless))
			SDL_SetWindowBordered(App->GetWindow()->window, (SDL_bool)!is_borderless);


		ImGui::SameLine();

		if (ImGui::Checkbox("Full Desktop", &is_full_desktop))
		{
			if (is_full_desktop)
				SDL_SetWindowFullscreen(App->GetWindow()->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			else
				SDL_SetWindowFullscreen(App->GetWindow()->window, 0);
		}
	}
}

void ModuleOpenGL::DrawOpenGLInfo()
{
	if (ImGui::CollapsingHeader("OpenGL"))
	{
		ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
		ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
		ImGui::Text("Version: %s", glGetString(GL_VERSION));
		ImGui::Text("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

		ImGui::Separator();
		
		int major, minor, profile;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);

		ImGui::Text("Context Version: %d.%d", major, minor);

		const char* profileStr = "Unknown";
		if (profile == SDL_GL_CONTEXT_PROFILE_CORE)
			profileStr = "Core";
		else if (profile == SDL_GL_CONTEXT_PROFILE_COMPATIBILITY)
			profileStr = "Compatibility";
		else if (profile == SDL_GL_CONTEXT_PROFILE_ES)
			profileStr = "ES";

		ImGui::Text("Context Profile: %s", profileStr);

		ImGui::Separator();

		int depthSize, stencilSize;
		SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depthSize);
		SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencilSize);

		ImGui::Text("Depth Buffer Size: %d bits", depthSize);
		ImGui::Text("Stencil Buffer Size: %d bits", stencilSize);

		ImGui::Separator();

		ImGui::Text("Depth Test: %s", glIsEnabled(GL_DEPTH_TEST) ? "Enabled" : "Disabled");
		ImGui::Text("Blending: %s", glIsEnabled(GL_BLEND) ? "Enabled" : "Disabled");
		ImGui::Text("Cull Face: %s", glIsEnabled(GL_CULL_FACE) ? "Enabled" : "Disabled");

		ImGui::Text("V-Sync: %s", SDL_GL_GetSwapInterval() == 0 ? "Disabled" : "Enabled");

		ImGui::Separator();

		ImGui::Text("Current Viewport:");
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		ImGui::Text("X: %d, Y: %d, Width: %d, Height: %d", viewport[0], viewport[1], viewport[2], viewport[3]);

	}
}


