#pragma once
#include "openal/al.h"
#include "openal/alc.h"
#include <unordered_map>
#include "glm/glm.hpp"

class Audio
{
	ALCdevice* device;
	ALCcontext* context;

	glm::vec3 listenerPos;

public:
	void init();
	void cleanup();

	glm::vec3 getListenerPos();
	void setListenerPos(glm::vec3 listenerPos);

	float getGlobalGain();
	void setGlobalGain(float gain);
};