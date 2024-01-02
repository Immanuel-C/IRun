#pragma once


#include <vulkan\vulkan.h>
#include <vulkan\vulkan_win32.h>
#include <vector>

struct SwapchainDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct SwapchainImage {
	VkImage image;
	VkImageView view;
};