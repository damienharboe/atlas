#pragma once

#include "vkbootstrap/VkBootstrap.h"

struct AllocatedBuffer 
{
	VkBuffer buffer;
	VmaAllocation allocation;
};