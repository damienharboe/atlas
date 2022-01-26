#include "vkEngine.h"

#include "sdl/SDL.h"
#include "sdl/SDL_vulkan.h"

#include "vkTypes.h"
#include "vkInit.h"
#include "pipelineBuilder.h"

#include "vkbootstrap/VkBootstrap.h"
#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#include <iostream>
#include <fstream>
#include "glm/gtc/matrix_transform.hpp"


#define VK_CHECK(x)													\
	do																\
	{																\
		VkResult err = x;											\
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

	graphicsQueue = device.get_queue(vkb::QueueType::graphics).value();
	graphicsQueueFamily = device.get_queue_index(vkb::QueueType::graphics).value();

	VmaAllocatorCreateInfo allocatorInfo{};
	allocatorInfo.physicalDevice = physicalDevice;
	allocatorInfo.device = device;
	allocatorInfo.instance = instance;
	vmaCreateAllocator(&allocatorInfo, &allocator);

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

	mainDeletionQueue.pushFunction([=]() {
		vkDestroySwapchainKHR(device, this->swapchain, nullptr);
	});
}

void VulkanEngine::initCommands()
{
	auto commandPoolInfo = vkinit::commandPoolCreateInfo(graphicsQueueFamily);

	VK_CHECK(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &commandPool));

	auto allocInfo = vkinit::commandBufferAllocInfo(commandPool);

	VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, &mainCommandBuffer));

	mainDeletionQueue.pushFunction([=]() {
		vkDestroyCommandPool(device, commandPool, nullptr);
	});
}

void VulkanEngine::initRenderpass()
{
	VkAttachmentDescription attachmentDesc{};
	attachmentDesc.format = swapchainImageFormat;
	attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference attachmentRef{};
	attachmentRef.attachment = 0;
	attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &attachmentRef;

	VkRenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &attachmentDesc;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;

	VK_CHECK(vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass));

	mainDeletionQueue.pushFunction([=]() {
		vkDestroyRenderPass(device, renderPass, nullptr);
	});
}

void VulkanEngine::initFramebuffers()
{
	VkFramebufferCreateInfo framebufferCreateInfo{};
	framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCreateInfo.pNext = nullptr;
	framebufferCreateInfo.renderPass = renderPass;
	framebufferCreateInfo.attachmentCount = 1;
	framebufferCreateInfo.width = windowExtent.width;
	framebufferCreateInfo.height = windowExtent.height;
	framebufferCreateInfo.layers = 1;

	const size_t swapchainImageCount = swapchainImages.size();
	framebuffers = std::vector<VkFramebuffer>(swapchainImageCount);

	for (int i = 0; i < swapchainImageCount; i++)
	{
		framebufferCreateInfo.pAttachments = &swapchainImageViews[i];
		VK_CHECK(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffers[i]));

		mainDeletionQueue.pushFunction([=]() {
			vkDestroyFramebuffer(device, framebuffers[i], nullptr);
			vkDestroyImageView(device, swapchainImageViews[i], nullptr);
		});
	}
}

void VulkanEngine::initSyncStructures()
{
	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VK_CHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &renderFence));

	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;

	VK_CHECK(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &presentSemaphore));
	VK_CHECK(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderSemaphore));

	mainDeletionQueue.pushFunction([=]() {
		vkDestroyFence(device, renderFence, nullptr);

		vkDestroySemaphore(device, presentSemaphore, nullptr);
		vkDestroySemaphore(device, renderSemaphore, nullptr);
	});
}

void VulkanEngine::initPipelines()
{
	VkShaderModule triangleFragShader;
	if (!loadShaderModule("E:\\Code\\atlas\\x64\\Debug\\shaders\\frag.spv", &triangleFragShader))
		std::cout << "Error when building frag shader module" << std::endl;
	else
		std::cout << "Frag shader module loaded" << std::endl;

	VkShaderModule triangleVertexShader;
	if(!loadShaderModule("E:\\Code\\atlas\\x64\\Debug\\shaders\\vert.spv", &triangleVertexShader))
		std::cout << "Error when building vertex shader module" << std::endl;
	else
		std::cout << "Vertex shader module loaded" << std::endl;

	VkShaderModule rtriangleFragShader;
	if (!loadShaderModule("E:\\Code\\atlas\\x64\\Debug\\shaders\\rfrag.spv", &rtriangleFragShader))
		std::cout << "Error when building frag shader module" << std::endl;
	else
		std::cout << "Frag shader module loaded" << std::endl;

	VkShaderModule rtriangleVertexShader;
	if (!loadShaderModule("E:\\Code\\atlas\\x64\\Debug\\shaders\\rvert.spv", &rtriangleVertexShader))
		std::cout << "Error when building vertex shader module" << std::endl;
	else
		std::cout << "Vertex shader module loaded" << std::endl;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = vkinit::pipelineLayoutCreateInfo();
	
	VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &trianglePipelineLayot));

	PipelineBuilder pipelineBuilder;

	pipelineBuilder.shaderStages.push_back(vkinit::pipelineShaderCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, triangleVertexShader));
	pipelineBuilder.shaderStages.push_back(vkinit::pipelineShaderCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader));

	pipelineBuilder.vertexInputInfo = vkinit::vertexInputStateCreateInfo();
	pipelineBuilder.inputAssembly = vkinit::inputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	pipelineBuilder.viewport.x = 0.f;
	pipelineBuilder.viewport.y = 0.f;
	pipelineBuilder.viewport.width = (float)windowExtent.width;
	pipelineBuilder.viewport.height = (float)windowExtent.height;
	pipelineBuilder.viewport.minDepth = 0.f;
	pipelineBuilder.viewport.maxDepth = 1.f;
	pipelineBuilder.scissor.offset = { 0, 0 };
	pipelineBuilder.scissor.extent = windowExtent;
	
	pipelineBuilder.rasterizer = vkinit::razterizationStateCreateInfo(VK_POLYGON_MODE_FILL);
	pipelineBuilder.multisampling = vkinit::multisampleStateCreateInfo();
	pipelineBuilder.colorBlendAttachment = vkinit::colorBlendAttachmentState();
	pipelineBuilder.pipelineLayout = trianglePipelineLayot;

	trianglePipeline = pipelineBuilder.buildPipeline(device, renderPass);
	
	pipelineBuilder.shaderStages.clear();

	pipelineBuilder.shaderStages.push_back(vkinit::pipelineShaderCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, rtriangleVertexShader));
	pipelineBuilder.shaderStages.push_back(vkinit::pipelineShaderCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, rtriangleFragShader));
	redTrianglePipeline = pipelineBuilder.buildPipeline(device, renderPass);

	VertexInputDescription vertexDescription = Vertex::getVertexDescription();
	pipelineBuilder.vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();
	pipelineBuilder.vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription.attributes.size();

	pipelineBuilder.vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();
	pipelineBuilder.vertexInputInfo.vertexBindingDescriptionCount = vertexDescription.bindings.size();

	pipelineBuilder.shaderStages.clear();

	VkShaderModule meshVertShader;
	if (!loadShaderModule("E:\\Code\\atlas\\x64\\Debug\\shaders\\triMesh.spv", &meshVertShader))
		std::cout << "Error when building triMesh shader module" << std::endl;
	else
		std::cout << "triMesh shader module loaded" << std::endl;

	pipelineBuilder.shaderStages.push_back(vkinit::pipelineShaderCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, meshVertShader));
	pipelineBuilder.shaderStages.push_back(vkinit::pipelineShaderCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, rtriangleFragShader));

	VkPipelineLayoutCreateInfo meshPipelineLayoutInfo = vkinit::pipelineLayoutCreateInfo();

	VkPushConstantRange pushConstant;
	pushConstant.offset = 0;
	pushConstant.size = sizeof(MeshPushConstants);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	meshPipelineLayoutInfo.pPushConstantRanges = &pushConstant;
	meshPipelineLayoutInfo.pushConstantRangeCount = 1;

	VK_CHECK(vkCreatePipelineLayout(device, &meshPipelineLayoutInfo, nullptr, &meshPipelineLayout));

	pipelineBuilder.pipelineLayout = meshPipelineLayout;

	meshPipeline = pipelineBuilder.buildPipeline(device, renderPass);

	vkDestroyShaderModule(device, meshVertShader, nullptr);
	vkDestroyShaderModule(device, rtriangleVertexShader, nullptr);
	vkDestroyShaderModule(device, rtriangleFragShader, nullptr);
	vkDestroyShaderModule(device, triangleVertexShader, nullptr);
	vkDestroyShaderModule(device, triangleFragShader, nullptr);

	mainDeletionQueue.pushFunction([=]() {
		vkDestroyPipeline(device, redTrianglePipeline, nullptr);
		vkDestroyPipeline(device, trianglePipeline, nullptr);
		vkDestroyPipeline(device, meshPipeline, nullptr);

		vkDestroyPipelineLayout(device, trianglePipelineLayot, nullptr);
		vkDestroyPipelineLayout(device, meshPipelineLayout, nullptr);
	});
}

void VulkanEngine::loadMeshes()
{
	triangleMesh.vertices.resize(3);

	triangleMesh.vertices[0].position = { 1.f, 1.f, 0.0f };
	triangleMesh.vertices[1].position = { -1.f, 1.f, 0.0f };
	triangleMesh.vertices[2].position = { 0.f,-1.f, 0.0f };

	triangleMesh.vertices[0].color = { 0.f, 1.f, 0.0f };
	triangleMesh.vertices[1].color = { 0.f, 1.f, 0.0f };
	triangleMesh.vertices[2].color = { 0.f, 1.f, 0.0f };

	uploadMesh(triangleMesh);
}

void VulkanEngine::uploadMesh(Mesh& mesh)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = nullptr;
	bufferInfo.size = mesh.vertices.size() * sizeof(Vertex);
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

	VK_CHECK(vmaCreateBuffer(allocator, &bufferInfo, &vmaallocInfo,
		&mesh.vertexBuffer.buffer,
		&mesh.vertexBuffer.allocation,
		nullptr));

	//add the destruction of triangle mesh buffer to the deletion queue
	mainDeletionQueue.pushFunction([=]() {
		vmaDestroyBuffer(allocator, mesh.vertexBuffer.buffer, mesh.vertexBuffer.allocation);
	});

	//copy vertex data
	void* data;
	vmaMapMemory(allocator, mesh.vertexBuffer.allocation, &data);

	memcpy(data, mesh.vertices.data(), mesh.vertices.size() * sizeof(Vertex));

	vmaUnmapMemory(allocator, mesh.vertexBuffer.allocation);
}

bool VulkanEngine::loadShaderModule(const char* filePath, VkShaderModule* outShaderModule)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
		return false;

	size_t fileSize = (size_t)file.tellg();
	std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
	file.seekg(0);
	file.read((char*)buffer.data(), fileSize);
	file.close();

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;

	createInfo.codeSize = buffer.size() * sizeof(uint32_t);
	createInfo.pCode = buffer.data();

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		return false;

	*outShaderModule = shaderModule;
	return true;
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

	initVulkan();
	initSwapchain();
	initCommands();
	initRenderpass();
	initFramebuffers();
	initSyncStructures();
	initPipelines();
	loadMeshes();

	isInitialized = true;
}

void VulkanEngine::cleanup()
{
	if (isInitialized)
	{
		vkWaitForFences(device, 1, &renderFence, true, UINT64_MAX);

		mainDeletionQueue.flush();

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyDevice(device, nullptr);
		vkb::destroy_debug_utils_messenger(instance, debugMessenger);
		vkDestroyInstance(instance, nullptr);
		SDL_DestroyWindow(window);
	}
}

void VulkanEngine::draw()
{
	VK_CHECK(vkWaitForFences(device, 1, &renderFence, VK_TRUE, UINT64_MAX));
	VK_CHECK(vkResetFences(device, 1, &renderFence));

	uint32_t swapchainImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, presentSemaphore, nullptr, &swapchainImageIndex));

	VK_CHECK(vkResetCommandBuffer(mainCommandBuffer, 0));

	VkCommandBuffer cmd = mainCommandBuffer;

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = nullptr;
	beginInfo.pInheritanceInfo = nullptr;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

	VkClearValue color;
	color.color = { { 0.f, 0.f, 0.f, 1.f } };

	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;

	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent = windowExtent;
	renderPassBeginInfo.framebuffer = framebuffers[swapchainImageIndex];

	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &color;

	vkCmdBeginRenderPass(cmd, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, meshPipeline);

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(cmd, 0, 1, &triangleMesh.vertexBuffer.buffer, &offset);

	glm::vec3 camPos = { 0.f, 0.f, -3.f };
	glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
	glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.f);
	projection[1][1] *= -1;

	glm::mat4 model = glm::rotate(glm::mat4{ 1.f }, glm::radians(frameNumber * 0.4f), glm::vec3(0, 1, 0));

	glm::mat4 meshMatrix = projection * view * model;

	MeshPushConstants constants;
	constants.renderMatrix = meshMatrix;

	vkCmdPushConstants(cmd, meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);

	vkCmdDraw(cmd, triangleMesh.vertices.size(), 1, 0, 0);

	vkCmdEndRenderPass(cmd);

	VK_CHECK(vkEndCommandBuffer(cmd));

	VkSubmitInfo submit{};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.pNext = nullptr;

	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	submit.pWaitDstStageMask = &waitStage;
	
	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &presentSemaphore;

	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &renderSemaphore;

	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmd;

	VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &submit, renderFence));

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;

	presentInfo.pSwapchains = &swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	VK_CHECK(vkQueuePresentKHR(graphicsQueue, &presentInfo));

	frameNumber++;
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
			else if (e.key.keysym.sym == SDLK_SPACE)
			{
				selectedShader++;
				if (selectedShader > 1)
				{
					selectedShader = 0;
				}
			}
		}

		draw();
	}
}