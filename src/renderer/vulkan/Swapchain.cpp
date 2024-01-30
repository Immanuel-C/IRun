#include "Swapchain.h"

namespace IRun {
	namespace Vk {
		Swapchain::Swapchain(bool vSync, IWindow::Window& window, const Surface& surface, const Device& device, Swapchain* oldSwapchain) {
			m_surfaceFormat = ChooseBestSurfaceFormat(device);
			VkPresentModeKHR presentMode = ChooseBestPresentationMode(vSync, device);
			m_imageExtent = ChooseSwapchainImageResolution(window, device);

			uint32_t imageCount = device.GetSwapchainDetails().capabilities.minImageCount + 1;

			if (device.GetSwapchainDetails().capabilities.maxImageCount > 0 && imageCount > device.GetSwapchainDetails().capabilities.maxImageCount) 
				imageCount = device.GetSwapchainDetails().capabilities.maxImageCount;
			

			VkSwapchainCreateInfoKHR swapchainCreateInfo{};
			swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapchainCreateInfo.surface = surface.Get();
			swapchainCreateInfo.imageFormat = m_surfaceFormat.format;
			swapchainCreateInfo.imageColorSpace = m_surfaceFormat.colorSpace;
			swapchainCreateInfo.presentMode = presentMode;
			swapchainCreateInfo.imageExtent = m_imageExtent;
			swapchainCreateInfo.minImageCount = imageCount;
			// Always set to one for our use case
			swapchainCreateInfo.imageArrayLayers = 1;
			swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			swapchainCreateInfo.preTransform = device.GetSwapchainDetails().capabilities.currentTransform;
			// Blending of two windows overlapping.
			// We want no blending.
			swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapchainCreateInfo.clipped = true;

			QueueFamilyIndices queueFamilyIndices = device.GetQueueFamilies();

			std::array<uint32_t, 2> arrQueueFamilyIndices = {
				(uint32_t)queueFamilyIndices.graphicsFamily,
				(uint32_t)queueFamilyIndices.presentationFamily,
			};

			if (queueFamilyIndices.graphicsFamily != queueFamilyIndices.presentationFamily) {
				swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				swapchainCreateInfo.queueFamilyIndexCount = (uint32_t)arrQueueFamilyIndices.size();
				swapchainCreateInfo.pQueueFamilyIndices = arrQueueFamilyIndices.data();
			}
			else {
				swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				swapchainCreateInfo.queueFamilyIndexCount = 0;
				swapchainCreateInfo.pQueueFamilyIndices = nullptr;
			}

			VkSwapchainKHR old = nullptr;

			if (oldSwapchain != nullptr) 
				old = oldSwapchain->Get();

			swapchainCreateInfo.oldSwapchain = old;

			VK_CHECK(vkCreateSwapchainKHR(device.Get().first, &swapchainCreateInfo, nullptr, &m_swapchain), "Failed to create Vulkan swapchain! Abort!");

			I_DEBUG_LOG_TRACE("Created Vulkan swapchain: 0x%p", m_swapchain);

			uint32_t swapchainImageCount = 0;
			vkGetSwapchainImagesKHR(device.Get().first, m_swapchain, &swapchainImageCount, nullptr);

			std::vector<VkImage> images{};
			images.resize(swapchainImageCount);

			vkGetSwapchainImagesKHR(device.Get().first, m_swapchain, &swapchainImageCount, images.data());

			for (VkImage image : images) {
				I_DEBUG_LOG_TRACE("Obtained Vulkan image : 0x%p", image);
				SwapchainImage swapchainImage;
				swapchainImage.image = image;
				swapchainImage.view = CreateImageView(image, m_surfaceFormat.format, device);
				I_DEBUG_LOG_TRACE("Created Vulkan image view: 0x%p (image: 0x%p)", swapchainImage.view, image);
				m_images.push_back(swapchainImage);
			}
		}

		void Swapchain::Destroy(const Device& device, bool isOldSwapchain) {
			for (const SwapchainImage& image : m_images)
				vkDestroyImageView(device.Get().first, image.view, nullptr);
			if (!isOldSwapchain)
				vkDestroySwapchainKHR(device.Get().first, m_swapchain, nullptr); 
		}

		// Best format is subjective, IRun will use:
		// format     : VK_FORMAT_R8G8B8_UNORM || VK_FORMAT_B8G8R8_UNORM
		// colorSpace : VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
		VkSurfaceFormatKHR Swapchain::ChooseBestSurfaceFormat(const Device& device) {
			const std::vector<VkSurfaceFormatKHR>& formats = device.GetSwapchainDetails().formats;

			// if size is one and VK_FORMAT_UNDEFINED all formats are supported
			if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
				return { VK_FORMAT_R8G8B8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

			for (const VkSurfaceFormatKHR& format : formats) {
				if ((format.format == VK_FORMAT_R8G8B8_UNORM || format.format == VK_FORMAT_B8G8R8_UNORM) && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					return format;
			}

			// yolo
			return formats[0];
		}
		VkPresentModeKHR Swapchain::ChooseBestPresentationMode(bool vSync, const Device& device) {
			std::vector<VkPresentModeKHR> presentModes = device.GetSwapchainDetails().presentModes;

			for (const VkPresentModeKHR& presentMode : presentModes)
			{
				if (vSync && presentMode == VK_PRESENT_MODE_FIFO_KHR)
					return VK_PRESENT_MODE_FIFO_KHR;
				else if (!vSync && presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
					return presentMode;
			}

			I_LOG_WARNING("VK_PRESENT_MODE_MAILBOX_KHR or VK_PRESENT_MODE_IMMEDIATE_KHR is not supported fallback to VK_PRESENT_MODE_FIFO_KHR! Try using another Graphics API if you don't want to use vSync");

			// Required by Vulkan spec
			return VK_PRESENT_MODE_FIFO_KHR;
		}

		#undef max
		#undef min

		VkExtent2D Swapchain::ChooseSwapchainImageResolution(IWindow::Window& window, const Device& device)
		{
			VkSurfaceCapabilitiesKHR capabilites = device.GetSwapchainDetails().capabilities;

			// IWindow measures screen sizes in pixels (GetFramebufferSize) and screenCoordinates (GetWindowSize). Vulkan uses pixels only. Usually pixels and screenCoordinates 
			// are the same but if you are using a high DPI display screen coordinates are not the same as pixels. In this case VkSurfaceCapabilitesKHR::currentExtent is set to 
			// max number of a uint32_t, so we have to calculate the extent manually.
			if (capabilites.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
				IWindow::IVector2 framebufferSize = window.GetFramebufferSize();

				framebufferSize.x = std::clamp((uint32_t)framebufferSize.x, capabilites.minImageExtent.width, capabilites.maxImageExtent.width);
				framebufferSize.y = std::clamp((uint32_t)framebufferSize.y, capabilites.minImageExtent.height, capabilites.maxImageExtent.height);

				return { (uint32_t)framebufferSize.x, (uint32_t)framebufferSize.y };
			}
			
			return capabilites.currentExtent;
		}
		VkImageView Swapchain::CreateImageView(VkImage image, VkFormat format, const Device& device)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = image;
			createInfo.format = format;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
			// Subresources allow the view to view only a part of an image
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			// Start mipmap level to view
			createInfo.subresourceRange.baseMipLevel = 0;
			// Number of mipmap levels to view
			createInfo.subresourceRange.levelCount = 1;
			// Start array layer to view
			createInfo.subresourceRange.baseArrayLayer = 0;
			// Number of layers to view
			createInfo.subresourceRange.layerCount = 1;
			
			VkImageView imageView;
			VK_CHECK(vkCreateImageView(device.Get().first, &createInfo, nullptr, &imageView), "Failed to create swapchain image view");

			

			return imageView;
		}

	}
}