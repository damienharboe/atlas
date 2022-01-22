#include "vulkan/vkEngine.h"

int main(int argc, char* arv[])
{
	VulkanEngine engine;

	engine.init();

	engine.run();

	engine.cleanup();

	return 0;
}