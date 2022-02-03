#pragma once

#include "vkTypes.h"
#include "vkMesh.h"
#include "vma/vk_mem_alloc.h"
#include <deque>
#include <functional>
#include <unordered_map>
#include "glm/glm.hpp"
#include "../input/input.h"
#include "../camera/camera.h"

struct Material
{
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
};

struct RenderObject
{
	Mesh* mesh;
	Material* material;
	glm::mat4 transformMatrix;
};

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

struct FrameData
{
	VkSemaphore presentSemaphore, renderSemaphore;
	VkFence renderFence;

	VkCommandPool commandPool;
	VkCommandBuffer mainCommandBuffer;
};

constexpr unsigned int frameOverlap = 2;

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
	void initScene();

	void uploadMesh(Mesh& mesh);

	bool loadShaderModule(const char* filePath, VkShaderModule* outShaderModule);

public:
	FrameData frames[frameOverlap];

	FrameData& getCurrentFrame();

	glm::vec3 camPos = { 10.f, 10.f, -10.f };

	Input input;
	Camera cam;

	std::vector<RenderObject> renderables;

	std::unordered_map<std::string, Material> materials;
	std::unordered_map<std::string, Mesh> meshes;

	Material* createMaterial(VkPipeline pipeline, VkPipelineLayout layout, const std::string& name);

	Material* getMaterial(const std::string& name);

	Mesh* getMesh(const std::string& name);

	void drawObjects(VkCommandBuffer cmd, RenderObject* first, int count);

	VkImageView depthImageView;
	AllocatedImage depthImage;

	VkFormat depthFormat;

	Mesh monkeyMesh;

	VkPipelineLayout meshPipelineLayout;
	VkPipeline meshPipeline;
	Mesh triangleMesh;

	VmaAllocator allocator;

	DeletionQueue mainDeletionQueue;

	VkPipelineLayout trianglePipelineLayot;
	VkPipeline trianglePipeline;
	VkPipeline redTrianglePipeline;

	VkRenderPass renderPass;
	std::vector<VkFramebuffer> framebuffers;

	VkQueue graphicsQueue;
	uint32_t graphicsQueueFamily;

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