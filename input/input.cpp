#include "input.h"

void Input::onFrame(SDL_Event* e)
{
	// Calling callbacks
	for (auto callback : callbacks)
	{
		if (callback.type == KEY_PRESS && e->type == SDL_KEYDOWN && callback.keyCode == e->key.keysym.sym)
			callback.callback();
		else if (callback.type == KEY_RELEASE && e->type == SDL_KEYUP && callback.keyCode == e->key.keysym.sym)
			callback.callback();
	}
}

void Input::registerKeyPress(SDL_Keycode key, std::function<void()>&& callback)
{
	Pair pair{};
	pair.keyCode = key;
	pair.callback = callback;
	pair.type = KEY_PRESS;

	callbacks.push_back(pair);
}

void Input::registerKeyRelease(SDL_Keycode key, std::function<void()>&& callback)
{
	Pair pair{};
	pair.keyCode = key;
	pair.callback = callback;
	pair.type = KEY_RELEASE;

	callbacks.push_back(pair);
}