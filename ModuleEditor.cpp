#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleCamera.h"
#include "Application.h"
#include "SDL.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <algorithm>

ModuleEditor::ModuleEditor() 
{
	show_config_window = false;
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

		if (ImGui::CollapsingHeader("Window"))
		{
			DrawWindowOptions();
		}
		
		App->ShowHardwareInfo();

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
				LOG("Showing Software details");
			}
			ImGui::EndMenu(); 
		}

		ImGui::EndMainMenuBar(); 
	}
}

void ModuleEditor::DrawWindowOptions()
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