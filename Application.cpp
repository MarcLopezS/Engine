#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleInput.h"
#include "ModuleRenderExercise.h"
#include "ModuleCamera.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditor.h"
#include "imgui.h"
#include "glew-2.1.0\include\GL\glew.h"


using namespace std;

Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules.push_back(window = new ModuleWindow());
	modules.push_back(render = new ModuleOpenGL());
	modules.push_back(debugDraw = new ModuleDebugDraw());
	modules.push_back(renderEx = new ModuleRenderExercise());
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(input = new ModuleInput());
	modules.push_back(camera = new ModuleCamera());

	deltaTime = 0.0f;
	lastFrameTime = SDL_GetTicks();

	fps_log.reserve(max_log_size);
	ms_log.reserve(max_log_size);

}

Application::~Application()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }
}

bool Application::Init()
{
	bool ret = true;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	return ret;
}

update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	CalculateDeltaTime();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for(list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}


void Application::CalculateDeltaTime()
{
	Uint32 currentFrameTime = SDL_GetTicks();
	deltaTime = (currentFrameTime - lastFrameTime) / 1000.0f;
	lastFrameTime = currentFrameTime;
}

void Application::ShowPerformanceInfo() 
{
	if (ImGui::CollapsingHeader("Application"))
	{
		UpdatePerformanceLogs();
		ImGui::InputText("Application Name", const_cast<char*>(TITLE), strlen(TITLE) + 1, ImGuiInputTextFlags_ReadOnly);

		ImGui::Text("Performance Metrics:");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / editor->io->Framerate, editor->io->Framerate);

		DrawPerformanceGraphs();
	}
}


void Application::ShowHardwareInfo()
{
	if (ImGui::CollapsingHeader("Hardware"))
	{

		ImGui::Text("SDL Version: %d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);

		ImGui::Separator();
		
		//CPU & RAM
		int cpuCount = SDL_GetCPUCount();
		int cacheLineSize = SDL_GetCPUCacheLineSize();
		ImGui::Text("CPUs: %d (Cache Line Size: %db)", cpuCount, cacheLineSize);

		int ram = SDL_GetSystemRAM();
		ImGui::Text("System RAM: %dMb", ram);

		std::string caps = "Caps: ";
		if (SDL_HasRDTSC()) caps += "RDTSC, ";
		if (SDL_HasMMX()) caps += "MMX, ";
		if (SDL_HasSSE()) caps += "SSE, ";
		if (SDL_HasSSE2()) caps += "SSE2, ";
		if (SDL_HasSSE3()) caps += "SSE3, ";
		if (SDL_HasSSE41()) caps += "SSE41, ";
		if (SDL_HasSSE42()) caps += "SSE42, ";
		if (SDL_HasAVX()) caps += "AVX, ";
		if (SDL_HasAVX2()) caps += "AVX2, ";

		if (!caps.empty() && caps.back() == ' ')
		{
			caps.pop_back(); 
			caps.pop_back(); 
		}

		ImGui::PushTextWrapPos(ImGui::GetWindowWidth() - 20.0f);
		ImGui::Text("%s", caps.c_str());
		ImGui::PopTextWrapPos();

		ImGui::Separator();

		//GPU
		ImGui::Text("GPU: %s", glGetString(GL_RENDERER));
		ImGui::Text("Brand: %s", glGetString(GL_VENDOR));

		//VRAM
		GLint totalMemoryKb = 0;
		GLint availableMemoryKb = 0;

		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalMemoryKb);
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &availableMemoryKb);

		float usedMemoryKb = (totalMemoryKb - availableMemoryKb);
		float reservedMemoryKb = totalMemoryKb - (usedMemoryKb + availableMemoryKb);

		ImGui::Text("VRAM Budget: %.1f Mb", totalMemoryKb / 1024.0f);
		ImGui::Text("VRAM Available: %.1f Mb", availableMemoryKb / 1024.0f);
		ImGui::Text("VRAM Usage: %.1f Mb", usedMemoryKb / 1024.0f);
		ImGui::Text("VRAM Reserved: %.1f Mb", reservedMemoryKb/1024.0f);

	}
}

void Application::UpdatePerformanceLogs()
{
	float fps = (deltaTime > 0.0f) ? (1.0f / deltaTime) : 0.0f;
	float ms = deltaTime * 1000.0f;

	fps_log.push_back(fps);
	ms_log.push_back(ms);

	if (fps_log.size() > max_log_size)
		fps_log.erase(fps_log.begin());
	if (ms_log.size() > max_log_size)
		ms_log.erase(ms_log.begin());
}

void Application::DrawPerformanceGraphs()
{
	char title[25];

	if (!fps_log.empty())
	{
		sprintf_s(title, 25, "Framerate %.1f", fps_log.back());
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	}

	if (!ms_log.empty())
	{
		sprintf_s(title, 25, "Milliseconds %.1f", ms_log.back());
		ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	}
}
