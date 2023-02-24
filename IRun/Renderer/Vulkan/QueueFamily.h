#pragma once

#include <optional>
#include <vulkan/vulkan.hpp>

namespace IRun {
	namespace Vulkan {
		// Indices = Index
		struct QueueFamilyIndices {
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool IsComplete() {
				return graphicsFamily.has_value() && presentFamily.has_value();
			}

			static QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice& device, vk::SurfaceKHR& surface) {
				QueueFamilyIndices indices{};

				std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

				int i = 0;
				for (const vk::QueueFamilyProperties& queueFamily : queueFamilies) {
					if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
						indices.graphicsFamily = i;

					if (indices.IsComplete()) break;


					vk::Bool32 presentSupport;
					device.getSurfaceSupportKHR(i, surface, &presentSupport);

					if (presentSupport) indices.presentFamily = i;

					i++;
				}

				return indices;
			}
		};



	}
}