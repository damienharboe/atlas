#pragma once

#include "vkbootstrap/VkBootstrap.h"
#include "vma/vk_mem_alloc.h"

struct AllocatedImage
{
	VkImage image;
	VmaAllocation allocation;
};

struct AllocatedBuffer 
{
	VkBuffer buffer;
	VmaAllocation allocation;
};