#include "speaker.h"
#include "util.h"

void Speaker::loadSound(char* fileName)
{
	ALuint buffer = util::loadSound(fileName);

	source = 0;
	alGenSources(1, &source);
	alSourcei(source, AL_BUFFER, (ALint)buffer);
}

void Speaker::play()
{
	alSourcePlay(source);
}

void Speaker::pause()
{
	alSourcePause(source);
}

void Speaker::setPos(glm::vec3 pos)
{
	alSource3f(source, AL_POSITION, pos.x, pos.z, pos.y);
}