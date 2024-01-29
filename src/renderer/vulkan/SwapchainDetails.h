#pragma once


#include <vulkan\vulkan.h>
#include <vector>

namespace IRun {
	namespace Vk {
		/// <summary>
		/// Details that the swapchain need to create.
		/// </summary>
		struct SwapchainDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		/// <summary>
		/// A wrapper for VkImage and VkImageView.
		/// </summary>
		struct SwapchainImage {
			VkImage image;
			VkImageView view;
		};
	}
}