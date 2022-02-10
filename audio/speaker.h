#pragma once
#include <openal/al.h>
#include "glm/glm.hpp"


class Speaker
{
	ALuint source;

public:
	void loadSound(char* fileName);
	
	void play();
	void pause();

	void setPos(glm::vec3 pos);
};