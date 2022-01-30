#include "input.h"

void Input::onFrame(SDL_Event* e)
{
	for (auto pair : callbacks)
	{
		if (e->key.keysym.sym == pair.keyCode)
			pair.callback();
	}
}

void Input::registerKeyPress(SDL_KeyCode key, std::function<void()>&& callback)
{
	Pair pair{};
	pair.keyCode = key;
	pair.callback = callback;

	callbacks.push_back(pair);
}