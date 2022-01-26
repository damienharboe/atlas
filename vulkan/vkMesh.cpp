#include "vkMesh.h"

VertexInputDescription Vertex::getVertexDescription()
{
	VertexInputDescription desc;

	VkVertexInputBindingDescription mainBinding{};
	mainBinding.binding = 0;
	mainBinding.stride = sizeof(Vertex);
	mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	desc.bindings.push_back(mainBinding);

	VkVertexInputAttributeDescription  posAttrib{};
	posAttrib.binding = 0;
	posAttrib.location = 1;
	posAttrib.format = VK_FORMAT_R32G32B32_SFLOAT;
	posAttrib.offset = offsetof(Vertex, position);

	VkVertexInputAttributeDescription  normAttrib{};
	normAttrib.binding = 0;
	normAttrib.location = 1;
	normAttrib.format = VK_FORMAT_R32G32B32_SFLOAT;
	normAttrib.offset = offsetof(Vertex, normal);

	VkVertexInputAttributeDescription colorAttrib{};
	normAttrib.binding = 0;
	normAttrib.location = 2;
	normAttrib.format = VK_FORMAT_R32G32B32_SFLOAT;
	normAttrib.offset = offsetof(Vertex, color);

	desc.attributes.push_back(posAttrib);
	desc.attributes.push_back(normAttrib);
	desc.attributes.push_back(colorAttrib);


	return desc;
}