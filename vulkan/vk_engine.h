#pragma once

#include "vk_types.h"

class VulkanEngine
{
	void initVulkan();
	void initSwapchain();

public:
	VkSwapchainKHR swapchain;
	VkFormat swapchainImageFormat;
	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkSurfaceKHR surface;

	bool isInitialized = false;
	int frameNumber = 0;

	VkExtent2D windowExtent = { 1700, 900 };

	struct SDL_Window* window = nullptr;

	void init();

	void cleanup();

	void draw();

	void run();
};