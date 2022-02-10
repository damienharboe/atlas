#include "vulkan/vkEngine.h"
#include "audio/audio.h"
#include "audio/speaker.h"

int main(int argc, char* arv[])
{
	VulkanEngine engine;

	engine.init();

	engine.run();

	engine.cleanup();

	return 0;
}