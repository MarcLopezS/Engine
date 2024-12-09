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

	context = SDL_GL_CreateContext(App->GetWindow()->window);
	
	if (SDL_GL_SetSwapInterval(0) != 0) {
		LOG("Warning: Unable to disable V-Sync! SDL Error: %s", SDL_GetError());
	}
	else {
		LOG("V-Sync disabled");
	}
	
	GLenum err = glewInit();

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
	SDL_GL_DeleteContext(context);

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
		if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
			SDL_SetWindowBrightness(App->GetWindow()->window, brightness);

		if (!resizable)
			ImGui::BeginDisabled();

		static int temp_width = window_width;
		static int temp_height = window_height;

		ImGui::SliderInt("Width", &temp_width, 640, SCREEN_WIDTH);
		ImGui::SliderInt("Height", &temp_height, 480, SCREEN_HEIGHT);

		if (!resizable)
			ImGui::EndDisabled();

		if (ImGui::Button("Apply"))
		{
			if ((temp_width != window_width || temp_height != window_height) && resizable)
			{
				window_width = temp_width;
				window_height = temp_height;

				SDL_SetWindowSize(App->GetWindow()->window, window_width, window_height);
				SDL_Delay(1000);
				App->GetOpenGL()->WindowResized(window_width, window_height);
				App->GetModuleCamera()->SetAspectRatio(static_cast<float>(window_width) / window_height);
				App->GetModuleCamera()->CalcProjMatrix();
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset"))
		{
			temp_width = SCREEN_WIDTH;
			temp_height = SCREEN_HEIGHT;
		}

		if (ImGui::Checkbox("Fullscreen", &fullscreen))
		{
			if (fullscreen)
				SDL_SetWindowFullscreen(App->GetWindow()->window, SDL_WINDOW_FULLSCREEN);
			else
				SDL_SetWindowFullscreen(App->GetWindow()->window, 0);
		}

		ImGui::SameLine();

		if (ImGui::Checkbox("Resizable", &resizable))
			SDL_SetWindowResizable(App->GetWindow()->window, (SDL_bool)resizable);

		if (ImGui::Checkbox("Borderless", &borderless))
			SDL_SetWindowBordered(App->GetWindow()->window, (SDL_bool)!borderless);


		ImGui::SameLine();

		if (ImGui::Checkbox("Full Desktop", &full_desktop))
		{
			if (full_desktop)
				SDL_SetWindowFullscreen(App->GetWindow()->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			else
				SDL_SetWindowFullscreen(App->GetWindow()->window, 0);
		}
	}
}


