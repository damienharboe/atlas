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

void VulkanEngine::initCommands()
{
	auto commandPoolInfo = vkinit::commandPoolCreateInfo(graphicsQueueFamily);

	VK_CHECK(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &commandPool));

	auto allocInfo = vkinit::commandBufferAllocInfo(commandPool);

	VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, &mainCommandBuffer));
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

	isInitialized = true;
}

void VulkanEngine::cleanup()
{
	if (isInitialized)
	{
		vkDestroyCommandPool(device, commandPool, nullptr);

		vkDestroySwapchainKHR(device, swapchain, nullptr);

		vkDestroyRenderPass(device, renderPass, nullptr);

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