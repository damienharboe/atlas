#include "audio.h"
#include <cstddef>
#include <iostream>
#include "libsndfile/sndfile.h"
#include "openal/alext.h"
#include <thread>
#include "util.h"

void Audio::init()
{
	device = alcOpenDevice(NULL);
	
	if (device)
	{
		context = alcCreateContext(device, NULL);
		alcMakeContextCurrent(context);
	}

	if (alIsExtensionPresent("EAX2.0") == AL_FALSE)
		std::cout << "EAX2.0 not available" << std::endl;
}

void Audio::cleanup()
{
	alcMakeContextCurrent(context);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

glm::vec3 Audio::getListenerPos()
{
	return listenerPos;
}

void Audio::setListenerPos(glm::vec3 listenerPos)
{
	this->listenerPos = listenerPos;
}

float Audio::getGlobalGain()
{
	float gain;
	alGetListenerf(AL_GAIN, &gain);
	return gain;
}

void Audio::setGlobalGain(float gain)
{
	float vol = gain;
	if (vol < 0.f)
		vol = 0;
	else if (vol > 5.f)
		vol = 5.f;

	alListenerf(AL_GAIN, vol);
}