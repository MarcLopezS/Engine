#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleCamera.h"
#include "ModuleTexture.h"
#include "ModuleRenderExercise.h"
#include "ModuleInput.h"
#include "Application.h"
#include "SDL.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include "glew-2.1.0\include\GL\glew.h"
#include <algorithm>

ModuleEditor::ModuleEditor()
{
	io = nullptr;
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
	LOG("Creating Editor context");
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	ImGui_ImplSDL2_InitForOpenGL(App->GetWindow()->window, App->GetRender()->GetContext());
	ImGui_ImplOpenGL3_Init(GLSL_VERSION);

	is_initialized = true;
	FlushPendingLogs();

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	DrawMenu();

	if (is_exit)
		return UPDATE_STOP;
	//Menu General
	if (is_about_window)
		ShowAbout();
	
	//menu View
	if (is_log_window)
		RenderLogWindow();

	if (is_texture_window)
	{
		ImGui::Begin("Texture Options",&is_texture_window);
		App->GetModuleTexture()->ShowTextureMenu();
		ImGui::End();
	}

	if (is_properties_window)
	{
		ImGui::Begin("Properties", &is_properties_window);
		App->GetModuleRenderExcercise()->DrawPropertiesWindow();
		ImGui::End();
	}

	//Menu Configuration
	if (is_config_window)
	{
		ImGui::Begin("Configuration",&is_config_window);
		ImGui::Text("Options");

		App->ShowPerformanceInfo();

		App->GetOpenGL()->DrawWindowOptions();
		
		App->ShowHardwareInfo();
		ShowSoftware();
		App->GetModuleInput()->DrawInputUIWindow();

		//Show ModuleOpenGL
		App->GetOpenGL()->DrawOpenGLInfo();
		ImGui::End();
	}
	

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp()
{
	LOG("Destroying Editor");
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	
	if (ImGui::GetCurrentContext()) {
		ImGui::DestroyContext();
	}
	return true;
}

void ModuleEditor::AddLog(const char* log)
{
	if (!is_initialized) 
	{
		_pendingLogs.push(log);
	}
	else
	{
		_logs.push_back(log);
		if (_logs.size() > 1000)
		{
			_logs.erase(_logs.begin());
		}
	}
	
}

void ModuleEditor::FlushPendingLogs()
{
	while (!_pendingLogs.empty()) 
	{
		_logs.push_back(_pendingLogs.front());
		_pendingLogs.pop();

		if (_logs.size() > 1000) {
			_logs.erase(_logs.begin());
		}
	}
}

void ModuleEditor::RenderLogWindow()
{
	if (!is_log_window) return;

	ImGui::Begin("Console", &is_log_window);

	for (const auto& log : _logs)
	{
		ImGui::TextUnformatted(log.c_str());
	}

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);

	ImGui::End();
}

void ModuleEditor::DrawMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("General"))
		{
			if (ImGui::MenuItem("Visit GitHub Page"))
			{
				LOG("Opening browser to GitHub");
				
				if (SDL_OpenURL("https://github.com/MarcLopezS/Engine") != 0) {
					SDL_Log("Unable to open URL: %s", SDL_GetError());
				}
			}
			if (ImGui::MenuItem("About"))
			{
				LOG("Showing About details");
				is_about_window = !is_about_window;
			}
			if (ImGui::MenuItem("Exit"))
			{
				LOG("Closing Engine");
				is_exit = !is_exit;
			}

			ImGui::EndMenu(); 
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Console"))
			{
				is_log_window = !is_log_window;
			}
			if (ImGui::MenuItem("Texture Menu"))
			{
				LOG("Opened Texture Editor Menu");
				is_texture_window = !is_texture_window;
			}
			if (ImGui::MenuItem("Properties"))
			{
				LOG("Opened Properties");
				is_properties_window = !is_properties_window;
			}
			ImGui::EndMenu(); 
		}
		if (ImGui::BeginMenu("Configuration"))
		{
			if (ImGui::MenuItem("Show Configuration"))
			{
				LOG("Opened Configuration Window");
				is_config_window = !is_config_window;
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar(); 
	}
}

void ModuleEditor::ShowAbout()
{
	ImGui::Begin("About", &is_about_window);

	ImGui::Text("Engine Name: %s", TITLE);
	ImGui::Text("Description: %s", "A 3D engine using OpenGL, designed for developing a video game project as part of the UPC master's degree");
	ImGui::Text("Author: %s", AUTHOR);

	ImGui::Separator();
	ImGui::Text("Libraries used:");
	ImGui::BulletText("ImGui Version: %s", IMGUI_VERSION);
	ImGui::BulletText("MathGeoLib Version: %s",MATHGEOLIB_VERSION);

	ImGui::BulletText("SDL Version: %d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);

	ImGui::BulletText("OpenGL Version: %s", (const char*)glGetString(GL_VERSION));
	ImGui::BulletText("GLEW Version: %s", (const char*)glewGetString(GLEW_VERSION));
	ImGui::BulletText("DirectXTex Version: October 2024 v.%d", DIRECTX_TEX_VERSION);
	ImGui::BulletText("Tiny_GLTF Version: %s", TINY_GLTF_VERSION);

	ImGui::Separator();

	ImGui::Text("License: %s", LICENSE);

	ImGui::End();

}


void ModuleEditor::ShowSoftware()
{
	if (ImGui::CollapsingHeader("Software"))
	{
		ImGui::Text("SDL Version: %d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
		ImGui::Text("OpenGL Version: %s", (const char*)glGetString(GL_VERSION));
		ImGui::Text("GLEW Version: %s", (const char*)glewGetString(GLEW_VERSION));
		ImGui::Text("DirectXTex Version: October 2024 v.%d", DIRECTX_TEX_VERSION);
		ImGui::Text("Tiny_GLTF Version: %s", TINY_GLTF_VERSION);
	}
}

