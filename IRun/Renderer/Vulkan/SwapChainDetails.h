#pragma once

#include <vulkan/vulkan.hpp>
#include "PhysicalDevice.h"
#include "Surface.h"


namespace IRun {
	namespace Vulkan {
		struct SwapChainSupportDetails {
			vk::SurfaceCapabilitiesKHR capabilites;
			std::vector<vk::SurfaceFormatKHR> formats;
			std::vector<vk::PresentModeKHR> presentModes;

			static void QuerySwapChainSupport(vk::PhysicalDevice& device, Surface& surface, SwapChainSupportDetails& details) {
				VK_CHECK(device.getSurfaceCapabilitiesKHR(surface.Get(), &details.capabilites), "Failed to get Vulkan surface capabilities");

				details.formats = device.getSurfaceFormatsKHR(surface.Get());
				details.presentModes = device.getSurfacePresentModesKHR(surface.Get());
			}
		};
	}
}