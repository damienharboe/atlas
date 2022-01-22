#pragma once

#include "vkTypes.h"

class VulkanEngine
{
	void initVulkan();
	void initSwapchain();
	void initCommands();
	void initRenderpass();
	void initFramebuffers();
	void initSyncStructures();
	void initPipelines();

	bool loadShaderModule(const char* filePath, VkShaderModule* outShaderModule);

public:
	VkPipelineLayout trianglePipelineLayot;
	VkPipeline trianglePipeline;

	VkSemaphore presentSemaphore, renderSemaphore;
	VkFence renderFence;

	VkRenderPass renderPass;
	std::vector<VkFramebuffer> framebuffers;

	VkQueue graphicsQueue;
	uint32_t graphicsQueueFamily;

	VkCommandPool commandPool;
	VkCommandBuffer mainCommandBuffer;

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