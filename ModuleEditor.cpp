#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleCamera.h"
#include "Application.h"
#include "SDL.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include "glew-2.1.0\include\GL\glew.h"
#include <algorithm>

ModuleEditor::ModuleEditor() 
{
	show_config_window = false;
	show_about_window = false;
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

	if (show_log_window)
		RenderLogWindow();

	if (show_config_window)
	{
		ImGui::Begin("Configuration",&show_config_window);
		ImGui::Text("Options");

		App->ShowPerformanceInfo();

		App->GetOpenGL()->DrawWindowOptions();
		
		App->ShowHardwareInfo();

		ImGui::End();
	}

	if (show_about_window)
		ShowAbout();

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
	logs.push_back(log); 
	if (logs.size() > 1000) 
	{
		logs.erase(logs.begin()); 
	}
}

void ModuleEditor::RenderLogWindow()
{
	if (!show_log_window) return;

	ImGui::Begin("Console", &show_log_window);

	for (const auto& log : logs)
	{
		ImGui::TextUnformatted(log.c_str());
	}

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);

	ImGui::End();
}

//TODO:Check definitive menu options
void ModuleEditor::DrawMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{
				LOG("New File");
			}
			if (ImGui::MenuItem("Open"))
			{
				LOG("Open file");
			}
			if (ImGui::MenuItem("Save"))
			{
				LOG("Save file");
			}

			ImGui::EndMenu(); 
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Show Console"))
			{
				show_log_window = !show_log_window;
			}
			ImGui::EndMenu(); 
		}
		if (ImGui::BeginMenu("Configuration"))
		{
			if (ImGui::MenuItem("Show Configuration"))
			{
				LOG("Opening Configuration Window");
				show_config_window = !show_config_window;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Documentation"))
			{
				LOG("Opening Documentation");
			}
			if (ImGui::MenuItem("About"))
			{
				LOG("Showing About details");
				show_about_window = !show_about_window;
			}
			ImGui::EndMenu(); 
		}

		ImGui::EndMainMenuBar(); 
	}
}

void ModuleEditor::ShowAbout()
{
	ImGui::Begin("About", &show_about_window);

	ImGui::Text("Engine Name: %s", TITLE);
	ImGui::Text("Description: %s", "example text"); //TODO
	ImGui::Text("Author: %s", AUTHOR);

	ImGui::Separator();
	//TODO:Show libraries with versions used
	ImGui::Text("Libraries used:");
	ImGui::BulletText("ImGui Version: %s", IMGUI_VERSION);
	ImGui::BulletText("MathGeoLib Version: "); //TODO

	ImGui::BulletText("SDL Version: %d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);

	ImGui::BulletText("OpenGL Version: %s", (const char*)glGetString(GL_VERSION));
	ImGui::BulletText("GLEW Version: %s", (const char*)glewGetString(GLEW_VERSION));

	//TODO:include library used for textures
	ImGui::Separator();

	//TODO:License
	ImGui::Text("License: %s", LICENSE);

	ImGui::End();

}
