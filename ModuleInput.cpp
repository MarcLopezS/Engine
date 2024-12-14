#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleOpenGL.h"
#include "ModuleEditor.h"
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

//TODO: Check if this process of reading inputs is done correctly
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