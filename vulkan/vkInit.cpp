#include "vkInit.h"
#pragma once

VkCommandPoolCreateInfo vkinit::commandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
{
	VkCommandPoolCreateInfo commandPoolInfo{};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.pNext = nullptr;

	commandPoolInfo.queueFamilyIndex = queueFamilyIndex;
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	return commandPoolInfo;
}

VkCommandBufferAllocateInfo vkinit::commandBufferAllocInfo(VkCommandPool pool, uint32_t count, VkCommandBufferLevel level)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;

	allocInfo.commandPool = pool;
	allocInfo.commandBufferCount = 1;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	return allocInfo;
}

VkPipelineShaderStageCreateInfo vkinit::pipelineShaderCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shaderModule)
{
	VkPipelineShaderStageCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.pNext = nullptr;

	createInfo.stage = stage;
	createInfo.module = shaderModule;
	createInfo.pName = "main";

	return createInfo;
}

VkPipelineVertexInputStateCreateInfo vkinit::vertexInputStateCreateInfo()
{
	VkPipelineVertexInputStateCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	createInfo.pNext = nullptr;

	createInfo.vertexBindingDescriptionCount = 0;
	createInfo.vertexAttributeDescriptionCount = 0;

	return createInfo;
}

VkPipelineInputAssemblyStateCreateInfo vkinit::inputAssemblyStateCreateInfo(VkPrimitiveTopology topology)
{
	VkPipelineInputAssemblyStateCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	createInfo.pNext = nullptr;

	createInfo.topology = topology;
	createInfo.primitiveRestartEnable = VK_FALSE;

	return createInfo;
}

VkPipelineRasterizationStateCreateInfo vkinit::razterizationStateCreateInfo(VkPolygonMode polygonMode)
{
	VkPipelineRasterizationStateCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	createInfo.pNext = nullptr;

	createInfo.depthBiasEnable = VK_FALSE;
	createInfo.rasterizerDiscardEnable = VK_FALSE;

	createInfo.polygonMode = polygonMode;
	createInfo.lineWidth = 1.f;
	createInfo.cullMode = VK_CULL_MODE_NONE;
	createInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

	createInfo.depthBiasEnable = VK_FALSE;
	createInfo.depthBiasConstantFactor = 0.f;
	createInfo.depthBiasClamp = 0.f;
	createInfo.depthBiasSlopeFactor = 0.f;

	return createInfo;
}

VkPipelineMultisampleStateCreateInfo vkinit::multisampleStateCreateInfo()
{
	VkPipelineMultisampleStateCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	createInfo.pNext = nullptr;

	createInfo.flags = 0;
	createInfo.sampleShadingEnable = VK_FALSE;
	createInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.minSampleShading = 1.f;
	createInfo.pSampleMask = nullptr;
	createInfo.alphaToCoverageEnable = VK_FALSE;
	createInfo.alphaToOneEnable = VK_FALSE;

	return createInfo;
}

VkPipelineColorBlendAttachmentState vkinit::colorBlendAttachmentState()
{
	VkPipelineColorBlendAttachmentState attachmentState{};
	attachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	attachmentState.blendEnable = VK_FALSE;

	return attachmentState;
}

VkPipelineLayoutCreateInfo vkinit::pipelineLayoutCreateInfo()
{
	VkPipelineLayoutCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	createInfo.pNext = nullptr;

	createInfo.flags = 0;
	createInfo.setLayoutCount = 0;
	createInfo.pSetLayouts = nullptr;
	createInfo.pPushConstantRanges = nullptr;

	return createInfo;
}

VkFramebufferCreateInfo vkinit::framebufferCreateInfo(VkRenderPass renderPass, VkExtent2D extent)
{
	VkFramebufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.pNext = nullptr;

	createInfo.renderPass = renderPass;
	createInfo.attachmentCount = 1;
	createInfo.width = extent.width;
	createInfo.height = extent.height;
	createInfo.layers = 1;

	return createInfo;
}

VkFenceCreateInfo vkinit::fenceCreateInfo(VkFenceCreateFlags flags)
{
	VkFenceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = flags;
	return createInfo;
}

VkSemaphoreCreateInfo vkinit::semaphoreCreateInfo(VkSemaphoreCreateFlags flags)
{
	VkSemaphoreCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = flags;

	return createInfo;
}

VkImageCreateInfo vkinit::imageCreateInfo(VkFormat format, VkImageUsageFlags flags, VkExtent3D extent)
{
	VkImageCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.pNext = nullptr;

	createInfo.imageType = VK_IMAGE_TYPE_2D;

	createInfo.format = format;
	createInfo.extent = extent;

	createInfo.mipLevels = 1;
	createInfo.arrayLayers = 1;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	createInfo.usage = flags;

	return createInfo;
}

VkImageViewCreateInfo vkinit::imageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags flags)
{
	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.pNext = nullptr;

	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.image = image;
	createInfo.format = format;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;
	createInfo.subresourceRange.aspectMask = flags;

	return createInfo;
}

VkPipelineDepthStencilStateCreateInfo vkinit::depthStencilCreateInfo(bool depthTest, bool depthWrite, VkCompareOp compareOp)
{
	VkPipelineDepthStencilStateCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	createInfo.pNext = nullptr;

	createInfo.depthTestEnable = depthTest ? VK_TRUE : VK_FALSE;
	createInfo.depthWriteEnable = depthWrite ? VK_TRUE : VK_FALSE;
	createInfo.depthCompareOp = depthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
	createInfo.depthBoundsTestEnable = VK_FALSE;
	createInfo.minDepthBounds = 0.f;
	createInfo.maxDepthBounds = 1.f;
	createInfo.stencilTestEnable = VK_FALSE;

	return createInfo;
}

VkRenderPassBeginInfo vkinit::renderPassBeginInfo(VkRenderPass renderPass, VkExtent2D extent, VkFramebuffer framebuffer)
{
	VkRenderPassBeginInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	createInfo.pNext = nullptr;
	
	createInfo.renderPass = renderPass;
	createInfo.renderArea.offset.x = 0;
	createInfo.renderArea.offset.y = 0;
	createInfo.renderArea.extent = extent;
	createInfo.framebuffer = framebuffer;

	createInfo.clearValueCount = 1;
	createInfo.pClearValues = nullptr;

	return createInfo;
}
