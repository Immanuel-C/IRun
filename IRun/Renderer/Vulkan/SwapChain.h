#pragma once

#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

#include "Window.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "Surface.h"

namespace IRun {
	namespace Vulkan {
		class SwapChain
		{
		public:
			SwapChain(Window& window, Device& device, PhysicalDevice& physicalDevice, Surface& surface, bool vSync);

		private:

			vk::SwapchainKHR m_swapChain;

			std::vector <vk::Image> m_swapChainImages;
			vk::Format m_swapChainImageFormat;
			vk::Extent2D m_swapChainExtent;

			vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
			vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes, bool vSync);
			vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilites, const Window& window);
		};
	}
}