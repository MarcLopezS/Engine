#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleOpenGL.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"
#include "ModuleRenderExercise.h"
#include "SDL/include/SDL.h"
#include "backends/imgui_impl_sdl2.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput() : _mouse_wheel(0)
{
	_keyState = new KeyState[MAX_KEYS];
	memset(_keyState, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(_mouse_buttons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);

}

// Destructor
ModuleInput::~ModuleInput()
{
	RELEASE_ARRAY(_keyState);
}

// Called before render is available
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

update_status ModuleInput::PreUpdate()
{
	SDL_Event sdlEvent;

	_mouse_wheel = 0;
	_mouse_motion = { 0, 0 };

	while (SDL_PollEvent(&sdlEvent) != 0)
	{
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);

		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			return UPDATE_STOP;
		case SDL_WINDOWEVENT:
			if (sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE) return UPDATE_STOP;
			if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED || sdlEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				App->GetOpenGL()->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
			break;
		case SDL_MOUSEBUTTONDOWN:
			_mouse_buttons[sdlEvent.button.button - 1] = KEY_DOWN;
			break;

		case SDL_MOUSEBUTTONUP:
			_mouse_buttons[sdlEvent.button.button - 1] = KEY_UP;
			break;

		case SDL_MOUSEMOTION:
			_mouse_motion.x = sdlEvent.motion.xrel / SCREEN_SIZE;
			_mouse_motion.y = sdlEvent.motion.yrel / SCREEN_SIZE;
			_mouse.x = sdlEvent.motion.x / SCREEN_SIZE;
			_mouse.y = sdlEvent.motion.y / SCREEN_SIZE;
			break;
		case SDL_MOUSEWHEEL:
			_mouse_wheel = sdlEvent.wheel.y;
			break;
		case SDL_DROPFILE:
		{
			char* droppedFile = sdlEvent.drop.file;

			LOG("File dropped: %s", droppedFile);

			App->GetModuleRenderExcercise()->LoadNewModel(droppedFile);
			SDL_free(droppedFile);
			App->GetModuleCamera()->FocusCamera();
			break;
		}
		}
	}

	_keyboard = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (_keyboard[i] == 1)
		{
			if (_keyState[i] == KEY_IDLE)
				_keyState[i] = KEY_DOWN;
			else
				_keyState[i] = KEY_REPEAT;
		}
		else
		{
			if (_keyState[i] == KEY_REPEAT || _keyState[i] == KEY_DOWN)
				_keyState[i] = KEY_UP;
			else
				_keyState[i] = KEY_IDLE;
		}
	}

	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if (_mouse_buttons[i] == KEY_DOWN)
			_mouse_buttons[i] = KEY_REPEAT;

		if (_mouse_buttons[i] == KEY_UP)
			_mouse_buttons[i] = KEY_IDLE;
	}

    return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleInput::Update()
{
    return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

const char* ModuleInput::KeyStateToString(KeyState state) const
{
	switch (state)
	{
	case KEY_IDLE:    return "IDLE";
	case KEY_DOWN:    return "DOWN";
	case KEY_REPEAT:  return "PRESSED";
	case KEY_UP:      return "UP";
	default:          return "UNKNOWN";
	}
}

void ModuleInput::DrawInputUIWindow()
{
	if (ImGui::CollapsingHeader("Input"))
	{
		ImVec4 originalHeaderColor = ImGui::GetStyle().Colors[ImGuiCol_Header];
		ImVec4 originalHeaderHoveredColor = ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered];

		ImVec4 grayColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		ImVec4 grayHoveredColor = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Header, grayColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, grayHoveredColor);

		if (ImGui::CollapsingHeader("Mouse Button State"))
		{
			ImGui::Text("Mouse Position: (%f, %f)", _mouse.x, _mouse.y);

			for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
			{
				ImGui::Text("Mouse Button %d: %s", i + 1, KeyStateToString(_mouse_buttons[i]));
			}
		}

		const SDL_Scancode keysToShow[] = {
			SDL_SCANCODE_W, SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D,
			SDL_SCANCODE_Q, SDL_SCANCODE_E,
			SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT
		};

		if (ImGui::CollapsingHeader("Keyboard State"))
		{
			for (SDL_Scancode scancode : keysToShow)
			{
				ImGui::Text("Key %s: %s", SDL_GetScancodeName(scancode), KeyStateToString(_keyState[scancode]));
			}
		}

		//Restore original colors
		ImGui::PopStyleColor(2);
	}
	
}
