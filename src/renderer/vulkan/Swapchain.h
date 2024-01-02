#pragma once

#include "Device.h"
#include "Surface.h"

#include <algorithm>

namespace IRun {
	namespace Vk {
		class Swapchain {
		public:
			Swapchain() = default;
			/// <summary>
			/// Create the swapchain, obtain the swapchain images, and create image views.
			/// </summary>
			/// <param name="surface">Surface to be presented to.</param>
			/// <param name="device">Device that will render the swapchain images.</param>
			Swapchain(bool vSync, IWindow::Window& window, const Surface& surface, const Device& device);
			/// <summary>
			/// Destroys the swapchain, images, and image views.
			/// </summary>
			/// <param name="device"></param>
			void Destroy(const Device& device);
			/// <summary>
			/// Get the native swapchain handle.
			/// </summary>
			/// <returns>native swapchain handle.</returns>
			inline VkSwapchainKHR Get() const { return m_swapchain; }

		private:
			VkSwapchainKHR m_swapchain;

			VkExtent2D m_imageExtent;
			VkSurfaceFormatKHR m_surfaceFormat;

			std::vector<SwapchainImage> m_images;

			VkSurfaceFormatKHR ChooseBestSurfaceFormat(const Device& device);
			VkPresentModeKHR ChooseBestPresentationMode(bool vSync, const Device& device);
			VkExtent2D ChooseSwapchainImageResolution(IWindow::Window& window, const Device& device);

			VkImageView CreateImageView(VkImage image, VkFormat format, const Device& device);
		};
	}
}

