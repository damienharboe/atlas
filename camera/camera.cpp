#include "camera.h"
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

void Camera::init(Input* inputptr)
{
	camPos = glm::vec3(0.f, 0.f, 0.f);
	camFront = glm::vec3(0.f, 0.f, -1.f);
	camUp = glm::vec3(0.f, 1.f, 0.f);
	sensitiviy = 0.1f;
	camSpeed = 0.25f;

	input = inputptr;

	input->registerMouseMotion([=](glm::vec2 delta) {
		updateDir(delta);
	});

	input->registerKeyPress(SDLK_w, [=] {
		camPos += camSpeed * camFront;
	});

	input->registerKeyPress(SDLK_s, [=] {
		camPos -= camSpeed * camFront;
	});

	input->registerKeyPress(SDLK_a, [=] {
		camPos -= glm::normalize(glm::cross(camFront, camUp)) * camSpeed;
	});

	input->registerKeyPress(SDLK_d, [=] {
		camPos += glm::normalize(glm::cross(camFront, camUp)) * camSpeed;
	});
}

void Camera::updatePos(glm::vec3 pos)
{
	camPos = pos;
}

void Camera::updateDir(glm::vec2 mousePos)
{
	yaw += mousePos.x * sensitiviy;
	pitch -= mousePos.y * sensitiviy;

	if (pitch > 89.f)
		pitch = 89.f;
	else if (pitch < -89.f)
		pitch = -89.f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	camFront = glm::normalize(front);
}

glm::vec3 Camera::getPos()
{
	return camPos;
}

void Camera::updatePosRel(glm::vec3 relPos)
{
	camPos += relPos;
}

glm::mat4 Camera::getView()
{
	return glm::lookAt(camPos, camPos + camFront, camUp);
}