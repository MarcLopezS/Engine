#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "Application.h"
#include "SDL.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <algorithm>

ModuleEditor::ModuleEditor() 
{
	show_demo_window = false;
	show_another_window = false;
	show_config_window = false;
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
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

	fps_log.reserve(max_log_size);
	ms_log.reserve(max_log_size);

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

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	if (show_log_window)
		RenderLogWindow();

	if (show_another_window)
	{
		ImGui::Begin("Another Window", nullptr);
		ImGui::Text("This is another window!");  

		if (ImGui::Button("Close"))
		{
			show_another_window = false;  
		}

		ImGui::End();  
	}

	if (show_config_window)
	{
		ImGui::Begin("Configuration",&show_config_window);
		ImGui::Text("Options");

		if (ImGui::CollapsingHeader("Application"))
		{
			UpdatePerformanceLogs();
			ImGui::InputText("Application Name", const_cast<char*>(TITLE), strlen(TITLE) + 1, ImGuiInputTextFlags_ReadOnly);

			ImGui::Text("Performance Metrics:");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);

			DrawPerformanceGraphs(); 
		}

		ImGui::End();
	}

	{ //Demo (remove later)
		static float f = 0.0f;
		static int counter = 0;


		ImGui::Begin("Hello, world!", nullptr);                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_log_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
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
		//make configuration menu
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
			if (ImGui::MenuItem("Gui Demo"))
			{
				LOG("Opening ImGui Demo");
				show_demo_window = !show_demo_window;
			}
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

void ModuleEditor::UpdatePerformanceLogs()
{
	float deltaTime = App->GetDeltaTime();

	float fps = (deltaTime > 0.0f) ? (1.0f / deltaTime) : 0.0f;
	float ms = deltaTime * 1000.0f;

	fps_log.push_back(fps);
	ms_log.push_back(ms);

	if (fps_log.size() > max_log_size)
		fps_log.erase(fps_log.begin());
	if (ms_log.size() > max_log_size)
		ms_log.erase(ms_log.begin());
}

void ModuleEditor::DrawPerformanceGraphs()
{
	if (!fps_log.empty())
	{
		char fps_title[25];
		sprintf_s(fps_title, 25, "Framerate %.1f", fps_log.back());
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, fps_title, 0.0f, 100.0f, ImVec2(310, 100));
	}

	if (!ms_log.empty())
	{
		char ms_title[25];
		sprintf_s(ms_title, 25, "Milliseconds %.1f", ms_log.back());
		ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, ms_title, 0.0f, 40.0f, ImVec2(310, 100));
	}
}
