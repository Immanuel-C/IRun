#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace IRun {
	namespace Vulkan {
		struct Vec2 {
			glm::vec2 position;
			glm::vec3 colour;

			// Per vertex data
			static vk::VertexInputBindingDescription GetBindingDescription() {
				vk::VertexInputBindingDescription description{};
				// Matches vk::VertexInputAttributeDescription::binding
				description.binding = 0;
				// Specifies the size of the vecs so Vulkan knows how to read the list of vectors
				description.stride = sizeof(Vec2);
				// Specifies if we should use each data entry for each vertex or instance
				// data entry = each vec?
				description.inputRate = vk::VertexInputRate::eVertex;

				return description;
			}

			// Per attrbute data (e.g. position, colour, etc.)
			static std::array<vk::VertexInputAttributeDescription, 2> GetAttributeDescriptions() {
				// One for position and colour
				std::array<vk::VertexInputAttributeDescription, 2> descriptions;
				// Matches vk::VertexInputBindingDescription::binding
				descriptions[0].binding = 0;
				// Location in shader layout(location = x)
				descriptions[0].location = 0;
				// For telling Vulkan the size of the attribute
				// Vec2 float
				descriptions[0].format = vk::Format::eR32G32Sfloat;
				// offsetof(Vec2, position) = 0 bytes
				// sizeof(position) = 8 bytes 
				// vk::Format::eR32G32Sfloat = 8 bytes
				descriptions[0].offset = offsetof(Vec2, position);

				descriptions[1].binding = 0;
				descriptions[1].location = 1;
				descriptions[1].format = vk::Format::eR32G32B32Sfloat;
				// offsetof(Vec2, colour) = 8 bytes
				// sizeof(Vec2::colour) = 12 bytes
				// vk::Format::eR32G32B32Sfloat = 12 bytes
				descriptions[1].offset = offsetof(Vec2, colour);

				return descriptions;
			}
		};
	}
}