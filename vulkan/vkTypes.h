#pragma once

#include "vkbootstrap/VkBootstrap.h"
#include "vma/vk_mem_alloc.h"

struct AllocatedBuffer 
{
	VkBuffer buffer;
	VmaAllocation allocation;
};