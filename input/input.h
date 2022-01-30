#pragma once
#include <vector>
#include <functional>
#include "sdl/SDL_keycode.h"
#include "sdl/SDL_events.h"

struct Pair
{
	SDL_KeyCode keyCode;
	std::function<void()> callback;
};

class Input
{
	std::vector<Pair> callbacks;

public:
	void onFrame(SDL_Event* e);

	// Callback is invoked when keypress event is fired.
	void registerKeyPress(SDL_KeyCode key, std::function<void()>&& callback);
	
	// Callback is invoked every frame when key is down
	void registerKeyDown(SDL_KeyCode key, void(*callback)());

	// Callback is invoked when keyrelease event is fired.
	void registerKeyRelease(SDL_KeyCode key, void(*callback)());
};