#pragma once

#include "vkTypes.h"
#include "vkMesh.h"
#include "vma/vk_mem_alloc.h"
#include <deque>
#include <functional>
#include "glm/glm.hpp"

struct MeshPushConstants
{
	glm::vec4 data;
	glm::mat4 renderMatrix;
};

struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;
	
	void pushFunction(std::function<void()>&& function)
	{
		deletors.push_back(function);
	}

	void flush()
	{
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
			(*it)();

		deletors.clear();
	}
};

class VulkanEngine
{
	void initVulkan();
	void initSwapchain();
	void initCommands();
	void initRenderpass();
	void initFramebuffers();
	void initSyncStructures();
	void initPipelines();
	void loadMeshes();

	void uploadMesh(Mesh& mesh);

	bool loadShaderModule(const char* filePath, VkShaderModule* outShaderModule);

public:
	VkImageView depthImageView;
	AllocatedImage depthImage;

	VkFormat depthFormat;

	Mesh monkeyMesh;

	VkPipelineLayout meshPipelineLayout;
	VkPipeline meshPipeline;
	Mesh triangleMesh;

	VmaAllocator allocator;

	DeletionQueue mainDeletionQueue;

	int selectedShader = 0;

	VkPipelineLayout trianglePipelineLayot;
	VkPipeline trianglePipeline;
	VkPipeline redTrianglePipeline;

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