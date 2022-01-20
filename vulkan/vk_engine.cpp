#include "vk_engine.h"

#include "sdl/SDL.h"
#include "sdl/SDL_vulkan.h"

#include "vk_types.h"
#include "vk_init.h"

#include "vkbootstrap/VkBootstrap.h"

#include <iostream>

#define VK_CHECK(x)													\
	do																\
	{																\
		VkResult res = 0;											\
		if (err)													\
		{															\
			std::cout << "Vulkan error: " << err << std::endl;		\
			abort();												\
		}															\
	} while (0);													\


void VulkanEngine::initVulkan()
{
	vkb::InstanceBuilder builder;

	auto ret = builder.set_app_name("Atlas")
		.request_validation_layers(true)
		.require_api_version(1, 1, 0)
		.use_default_debug_messenger()
		.build();

	vkb::Instance instance = ret.value();

	this->instance = instance.instance;
	debugMessenger = instance.debug_messenger;

	SDL_Vulkan_CreateSurface(window, this->instance, &surface);

	vkb::PhysicalDeviceSelector selector(instance);
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version(1, 1)
		.set_surface(surface)
		.select()
		.value();

	vkb::DeviceBuilder deviceBuilder(physicalDevice);

	vkb::Device device = deviceBuilder.build().value();

	this->device = device;
	this->physicalDevice = physicalDevice;


}

void VulkanEngine::initSwapchain()
{
	vkb::SwapchainBuilder swapchainBuilder(physicalDevice, device, surface);

	vkb::Swapchain swapchain = swapchainBuilder
		.use_default_format_selection()
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(windowExtent.width, windowExtent.height)
		.build()
		.value();

	this->swapchain = swapchain.swapchain;
	swapchainImages = swapchain.get_images().value();
	swapchainImageViews = swapchain.get_image_views().value();
	swapchainImageFormat = swapchain.image_format;
}

void VulkanEngine::init()
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

	window = SDL_CreateWindow(
		"Atlas",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		windowExtent.width,
		windowExtent.height,
		windowFlags
	);

	isInitialized = true;

	initVulkan();
	initSwapchain();
}

void VulkanEngine::cleanup()
{
	if (isInitialized)
	{
		vkDestroySwapchainKHR(device, swapchain, nullptr);

		for (auto imageView : swapchainImageViews)
			vkDestroyImageView(device, imageView, nullptr);

		vkDestroyDevice(device, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkb::destroy_debug_utils_messenger(instance, debugMessenger);
		vkDestroyInstance(instance, nullptr);
		SDL_DestroyWindow(window);
	}
}

void VulkanEngine::draw()
{

}

void VulkanEngine::run()
{
	SDL_Event e;
	bool quit = false;

	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT) quit = true;
		}

		draw();
	}
}