#pragma once
#include <vector>
#include <functional>
#include <unordered_map>
#include "sdl/SDL_keycode.h"
#include "sdl/SDL_events.h"
#include "glm/glm.hpp"

typedef int InputID;

enum RegisterType
{
	KEY_PRESS,
	KEY_RELEASE,
	MOUSE_MOTION
};

struct Pair
{
	SDL_Keycode keyCode;
	std::function<void()> callback;
	std::function<void(glm::vec2)> callback2;
	RegisterType type;
	InputID id;
};

class Input
{
	std::vector<Pair> callbacks;
	std::unordered_map<SDL_Keycode, bool> keyState;
	InputID currentID = 0;
	glm::vec2 mousePos;

	InputID generateInputID() 
	{
		currentID++;
		return currentID;
	}

public:
	void onEventLoop(SDL_Event* e);

	// Must be called every frame
	void onFrame();

	// Callback is invoked when keypress event is fired.
	InputID registerKeyPress(SDL_Keycode key, std::function<void()>&& callback);
	
	// Callback is invoked when keyrelease event is fired.
	InputID registerKeyRelease(SDL_Keycode key, std::function<void()>&& callback);

	InputID registerMouseMotion(std::function<void(glm::vec2)>&& callback);

	glm::vec2 getMousePos() { return mousePos; };

	// Removes a specific callback
	void removeCallback(InputID id);
};