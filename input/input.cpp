#include "input.h"

void Input::onFrame(SDL_Event* e)
{
	// Updating keystate
	if (e->type == SDL_KEYDOWN)
	{
		auto it = keyState.find(e->key.keysym.sym);

		if (it == keyState.end())
			keyState.insert(std::pair<SDL_Keycode, bool>(e->key.keysym.sym, true));
		else
			it->second = true;
	}
	else if (e->type == SDL_KEYUP)
	{
		auto it = keyState.find(e->key.keysym.sym);

		if (it != keyState.end())
			it->second = false;
	}

	// Calling callbacks
	for (auto callback : callbacks)
	{
		if (callback.type == KEY_PRESS && keyState[callback.keyCode] == true)
			callback.callback();
		
		if (callback.type == KEY_RELEASE && e->type == SDL_KEYUP && callback.keyCode == e->key.keysym.sym)
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