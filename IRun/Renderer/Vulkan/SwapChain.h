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

			vk::SwapchainKHR& Get();

			vk::Format GetSwapChainImageFormat();
			vk::Extent2D GetSwapChainExtent2D();

			std::vector<vk::ImageView>& GetSwapChainImageViews();

			void Create(Window& window, Device& device, PhysicalDevice& physicalDevice, Surface& surface, bool vSync, bool recreate = false);

			void Destroy(Device& device);
		private:

			vk::SwapchainKHR m_swapChain;
			vk::SwapchainKHR m_oldSwapChain;

			std::vector <vk::Image> m_swapChainImages;
			std::vector <vk::ImageView> m_swapChainImageViews;
			vk::Format m_swapChainImageFormat;
			vk::Extent2D m_swapChainExtent;

			void CreateImageViews(Device& device);

			vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
			vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes, bool vSync);
			vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilites, const Window& window);
		};
	}
}