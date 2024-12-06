#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleOpenGL.h"
#include "SDL/include/SDL.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput()
{
	keyState = new KeyState[MAX_KEYS];
	memset(keyState, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);

}

// Destructor
ModuleInput::~ModuleInput()
{
	RELEASE_ARRAY(keyState);
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
	mouse_motion = { 0, 0 };

	keyboard = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keyboard[i] == 1)
		{
			if (keyState[i] == KEY_IDLE)
				keyState[i] = KEY_DOWN;
			else
				keyState[i] = KEY_REPEAT;
		}
		else
		{
			if (keyState[i] == KEY_REPEAT || keyState[i] == KEY_DOWN)
				keyState[i] = KEY_UP;
			else
				keyState[i] = KEY_IDLE;
		}
	}

	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if (mouse_buttons[i] == KEY_DOWN)
			mouse_buttons[i] = KEY_REPEAT;

		if (mouse_buttons[i] == KEY_UP)
			mouse_buttons[i] = KEY_IDLE;
	}

    return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleInput::Update()
{
    SDL_Event sdlEvent;

    while (SDL_PollEvent(&sdlEvent) != 0)
    {
        switch (sdlEvent.type)
        {
            case SDL_QUIT:
                return UPDATE_STOP;
            case SDL_WINDOWEVENT:
                if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED || sdlEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    App->GetOpenGL()->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
                break;
			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[sdlEvent.button.button - 1] = KEY_DOWN;
				break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[sdlEvent.button.button - 1] = KEY_UP;
				break;

			case SDL_MOUSEMOTION:
				mouse_motion.x = sdlEvent.motion.xrel / SCREEN_SIZE;
				mouse_motion.y = sdlEvent.motion.yrel / SCREEN_SIZE;
				mouse.x = sdlEvent.motion.x / SCREEN_SIZE;
				mouse.y = sdlEvent.motion.y / SCREEN_SIZE;
				break;
        }
    }

    keyboard = SDL_GetKeyboardState(NULL);

    return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

const Point& ModuleInput::GetMousePosition() const
{
	return mouse;
}

const Point& ModuleInput::GetMouseMotion() const
{
	return mouse_motion;
}