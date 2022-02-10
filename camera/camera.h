#pragma once
#include "glm/glm.hpp"
#include "../input/input.h"

class Camera
{
	glm::vec3 camPos;
	glm::vec3 camFront;
	glm::vec3 camUp;

	float yaw;
	float pitch;
	float sensitiviy;
	float camSpeed;

	Input* input;

public:
	void init(Input* input);

	void updatePos(glm::vec3 pos);
	void updatePosRel(glm::vec3 relPos);
	void updateDir(glm::vec2 mousePos);
	void updateDirDelta(glm::vec2 mousePos);

	glm::vec3 getPos();
	float getYaw() { return yaw; };
	float getPitch() { return pitch; };

	glm::mat4 getView();
};