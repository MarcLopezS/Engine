#pragma once
#include "Module.h"
#include "Globals.h"
#include "SDL/include/SDL_scancode.h"

#define NUM_MOUSE_BUTTONS 5

typedef unsigned __int8 Uint8;

struct Point {
    float x;
    float y;

    Point(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }

    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y);
    }
};

enum EventWindow
{
    WE_QUIT = 0,
    WE_HIDE = 1,
    WE_SHOW = 2,
    WE_COUNT
};

enum KeyState
{
    KEY_IDLE = 0,
    KEY_DOWN,
    KEY_REPEAT,
    KEY_UP
};

class ModuleInput : public Module
{
public:
	
	ModuleInput();
	~ModuleInput();

	bool Init();
    update_status PreUpdate();
	update_status Update();
	bool CleanUp();

    //Check the key states
    KeyState GetKey(int id) const { return keyState[id]; }
    KeyState GetMouseButtonDown(int id) const { return mouse_buttons[id - 1]; }

    // Get mouse / axis position
    const Point& GetMouseMotion() const;
    const Point& GetMousePosition() const;

private:
	const Uint8 *keyboard = NULL;
    KeyState* keyState;
    KeyState	mouse_buttons[NUM_MOUSE_BUTTONS];
    Point mouse_motion;
    Point mouse;

};