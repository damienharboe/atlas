#include "input.h"
#include <iostream>

void Input::onEventLoop(SDL_Event* e)
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
	else if (e->type == MOUSE_MOTION)
	{
		mousePos = glm::vec2(static_cast<float>(e->motion.x), static_cast<float>(e->motion.y));
	}

	for (auto callback : callbacks)
	{
		if (callback.type == KEY_RELEASE && callback.keyCode == e->key.keysym.sym)
			callback.callback();
		else if (e->type == SDL_MOUSEMOTION && callback.type == MOUSE_MOTION)
			callback.callback2(glm::vec2(e->motion.xrel, e->motion.yrel));
	}
}

void Input::onFrame()
{
	// Calling callbacks
	for (auto callback : callbacks)
		if (callback.type == KEY_PRESS && keyState[callback.keyCode] == true)
			callback.callback();
}

InputID Input::registerKeyPress(SDL_Keycode key, std::function<void()>&& callback)
{
	Pair pair{};
	pair.keyCode = key;
	pair.callback = callback;
	pair.type = KEY_PRESS;
	pair.id = generateInputID();

	callbacks.push_back(pair);

	return pair.id;
}

InputID Input::registerKeyRelease(SDL_Keycode key, std::function<void()>&& callback)
{
	Pair pair{};
	pair.keyCode = key;
	pair.callback = callback;
	pair.type = KEY_RELEASE;
	pair.id = generateInputID();

	callbacks.push_back(pair);

	return pair.id;
}

InputID Input::registerMouseMotion(std::function<void(glm::vec2)>&& callback)
{
	Pair pair{};
	pair.callback2 = callback;
	pair.type = MOUSE_MOTION;
	pair.id = generateInputID();

	callbacks.push_back(pair);

	return pair.id;
}

void Input::removeCallback(InputID id)
{
	for (int i = 0; i < callbacks.size(); i++)
	{
		if (callbacks[i].id == id)
			callbacks.erase(callbacks.begin() + i);
	}
}