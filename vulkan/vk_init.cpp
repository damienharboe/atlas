#include "vk_init.h"
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
}
