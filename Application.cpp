#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleInput.h"
#include "ModuleRenderExercise.h"
#include "ModuleCamera.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditor.h"
#include "ModuleTexture.h"
#include "imgui.h"
#include "glew-2.1.0\include\GL\glew.h"


using namespace std;

Application::Application() : _deltaTime(0.0f), _lastFrameTime(SDL_GetTicks())
{
	// Order matters: they will Init/start/update in this order
	modules.push_back(std::make_unique<ModuleWindow>());
	_window = static_cast<ModuleWindow*>(modules.back().get());

	modules.push_back(std::make_unique<ModuleOpenGL>());
	_render = static_cast<ModuleOpenGL*>(modules.back().get());

	modules.push_back(std::make_unique<ModuleDebugDraw>());
	_debugDraw = static_cast<ModuleDebugDraw*>(modules.back().get());

	modules.push_back(std::make_unique<ModuleRenderExercise>());
	_renderEx = static_cast<ModuleRenderExercise*>(modules.back().get());

	modules.push_back(std::make_unique<ModuleEditor>());
	_editor = static_cast<ModuleEditor*>(modules.back().get());

	modules.push_back(std::make_unique<ModuleInput>());
	_input = static_cast<ModuleInput*>(modules.back().get());

	modules.push_back(std::make_unique<ModuleCamera>());
	_camera = static_cast<ModuleCamera*>(modules.back().get());

	modules.push_back(std::make_unique<ModuleTexture>());
	_texture = static_cast<ModuleTexture*>(modules.back().get());

	_fps_log.reserve(_max_log_size);
	_ms_log.reserve(_max_log_size);

}

Application::~Application()
{
}

bool Application::Init()
{
	bool ret = true;
	
	for (auto& module : modules)
	{
		if (!module->Init())
		{
			ret = false;
			break;
		}
	}

	return ret;
}

update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	CalculateDeltaTime();

	for (auto& module : modules)
	{
		if (ret == UPDATE_CONTINUE)
			ret = module->PreUpdate();
	}

	for (auto& module : modules)
	{
		if (ret == UPDATE_CONTINUE)
			ret = module->Update();
	}

	for (auto& module : modules)
	{
		if (ret == UPDATE_CONTINUE)
			ret = module->PostUpdate();
	}

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (auto it = modules.rbegin(); it != modules.rend(); ++it)
	{
		if (!(*it)->CleanUp())
		{
			ret = false;
		}
	}

	return ret;
}


void Application::CalculateDeltaTime()
{
	Uint32 currentFrameTime = SDL_GetTicks();
	_deltaTime = (currentFrameTime - _lastFrameTime) / 1000.0f;
	_lastFrameTime = currentFrameTime;
}

void Application::ShowPerformanceInfo() 
{
	if (ImGui::CollapsingHeader("Application"))
	{
		UpdatePerformanceLogs();
		ImGui::InputText("Application Name", const_cast<char*>(TITLE), strlen(TITLE) + 1, ImGuiInputTextFlags_ReadOnly);

		ImGui::Text("Performance Metrics:");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / _editor->io->Framerate, _editor->io->Framerate);

		DrawPerformanceGraphs();
	}
}


void Application::ShowHardwareInfo()
{
	if (ImGui::CollapsingHeader("Hardware"))
	{
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

		float usedMemoryKb = static_cast<float>(totalMemoryKb - availableMemoryKb);
		float reservedMemoryKb = static_cast<float>(totalMemoryKb - (usedMemoryKb + availableMemoryKb));

		ImGui::Text("VRAM Budget: %.1f Mb", totalMemoryKb / 1024.0f);
		ImGui::Text("VRAM Available: %.1f Mb", availableMemoryKb / 1024.0f);
		ImGui::Text("VRAM Usage: %.1f Mb", usedMemoryKb / 1024.0f);
		ImGui::Text("VRAM Reserved: %.1f Mb", reservedMemoryKb/1024.0f);

	}
}

void Application::UpdatePerformanceLogs()
{
	float fps = (_deltaTime > 0.0f) ? (1.0f / _deltaTime) : 0.0f;
	float ms = _deltaTime * 1000.0f;

	_fps_log.push_back(fps);
	_ms_log.push_back(ms);

	if (_fps_log.size() > _max_log_size)
		_fps_log.erase(_fps_log.begin());
	if (_ms_log.size() > _max_log_size)
		_ms_log.erase(_ms_log.begin());
}

void Application::DrawPerformanceGraphs()
{
	char title[25];

	if (!_fps_log.empty())
	{
		sprintf_s(title, 25, "Framerate %.1f", _fps_log.back());
		ImGui::PlotHistogram("##framerate", &_fps_log[0], static_cast<int>(_fps_log.size()), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	}

	if (!_ms_log.empty())
	{
		sprintf_s(title, 25, "Milliseconds %.1f", _ms_log.back());
		ImGui::PlotHistogram("##milliseconds", &_ms_log[0], static_cast<int>(_ms_log.size()), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	}
}
