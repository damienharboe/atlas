#pragma once
#include <vector>
#include <functional>
#include <unordered_map>
#include "sdl/SDL_keycode.h"
#include "sdl/SDL_events.h"

enum registerType
{
	KEY_PRESS,
	KEY_RELEASE,
};

struct Pair
{
	SDL_Keycode keyCode;
	std::function<void()> callback;
	registerType type;
};

class Input
{
	std::vector<Pair> callbacks;
	std::unordered_map<SDL_Keycode, bool> keyState;

public:
	// Must be called every frame
	void onFrame(SDL_Event* e);

	// Callback is invoked when keypress event is fired.
	void registerKeyPress(SDL_Keycode key, std::function<void()>&& callback);
	
	// Callback is invoked when keyrelease event is fired.
	void registerKeyRelease(SDL_Keycode key, std::function<void()>&& callback);
};