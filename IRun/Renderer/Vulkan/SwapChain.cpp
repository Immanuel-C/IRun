#include "SwapChain.h"

namespace IRun {
	namespace Vulkan {


		SwapChain::SwapChain(Window& window, Device& device, PhysicalDevice& physicalDevice, Surface& surface, bool vSync) {
			Create(window, device, physicalDevice, surface, vSync);
		}

		vk::SwapchainKHR& SwapChain::Get() { return m_swapChain; }

		vk::Format SwapChain::GetSwapChainImageFormat() { return m_swapChainImageFormat; }

		vk::Extent2D SwapChain::GetSwapChainExtent2D() { return m_swapChainExtent; }

		std::vector<vk::ImageView>& SwapChain::GetSwapChainImageViews() { return m_swapChainImageViews; }

		void SwapChain::Create(Window& window, Device& device, PhysicalDevice& physicalDevice, Surface& surface, bool vSync, bool recreate) {
			if (recreate)
				m_oldSwapChain = m_swapChain;


			SwapChainSupportDetails swapChainSupport{};
			SwapChainSupportDetails::QuerySwapChainSupport(physicalDevice, surface, swapChainSupport);

			vk::SurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
			vk::PresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes, vSync);
			vk::Extent2D extent = ChooseSwapExtent(swapChainSupport.capabilites, window);

			uint32_t imageCount = swapChainSupport.capabilites.minImageCount + 1;

			if (swapChainSupport.capabilites.maxImageCount > 0 && imageCount > swapChainSupport.capabilites.maxImageCount)
				imageCount = swapChainSupport.capabilites.maxImageCount;

			vk::SwapchainCreateInfoKHR swapChainCreateInfo{};
			swapChainCreateInfo.surface = surface.Get();
			swapChainCreateInfo.minImageCount = imageCount;
			swapChainCreateInfo.imageFormat = surfaceFormat.format;
			swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
			swapChainCreateInfo.imageExtent = extent;
			// The imageArrayLayers specifies the amount of layers each image consists of.
			// Only used in stereoscopic 3D apps
			// I Think its used for VR apps
			swapChainCreateInfo.imageArrayLayers = 1;
			// We are going to render onto the image directly
			// If we were using post processing we would transfer this to a file (vk::ImageUsageFlagBits::eeTransferDst)
			// then do the post processing
			swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

			QueueFamilyIndices indices = QueueFamilyIndices::FindQueueFamilies(physicalDevice, surface);
			std::array<uint32_t, 2> queueFamilyIndices = { indices.graphicsFamily.value(), indices.presentFamily.value() };


			// We need to handle how the swapchain images will be handles across multiple queues
			// If the graphicsFamily and presentFamily is the same then we can use vk::SharingMode::eExclusive
			// Which is the most efficent. But if the queues are no the same then we use vk::SharingMode::eConcurrent to tell
			// Vulkan that we want to be able the move the images from queue family to another
			if (indices.graphicsFamily != indices.presentFamily) {
				swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
				// Have to tell Vulkan the queue families we are sharing with
				swapChainCreateInfo.queueFamilyIndexCount = queueFamilyIndices.size();
				swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
			}
			else {
				swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
				swapChainCreateInfo.queueFamilyIndexCount = 0;
				swapChainCreateInfo.pQueueFamilyIndices = nullptr;
			}

			// Can make changes to the images like rotation
			swapChainCreateInfo.preTransform = swapChainSupport.capabilites.currentTransform;
			// Specifies if the alpha channel should be used for blending
			swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
			swapChainCreateInfo.presentMode = presentMode;
			// If another window is over the main window then we dont draw those pixels
			// only turn off if we need the data
			swapChainCreateInfo.clipped = true;
			// For swapchain recreation
			// We can use this to not stop drawing when recreating the swapChain
			if (recreate)
				swapChainCreateInfo.oldSwapchain = m_oldSwapChain;

			m_swapChainImageFormat = surfaceFormat.format;
			m_swapChainExtent = extent;

			m_swapChain = device.Get().createSwapchainKHR(swapChainCreateInfo, nullptr);

			if (recreate) {
				for (int i = 0; i < m_swapChainImageViews.size(); i++) {
					I_DEBUG_LOG_INFO("SwapChainKHR: %p, Image View #%i Deleted: %p", (VkSwapchainKHR)m_swapChain, i, (VkImageView)m_swapChainImageViews[i]);
					device.Get().destroyImageView(m_swapChainImageViews[i]);
				}
				device.Get().waitIdle();
				device.Get().destroySwapchainKHR(m_oldSwapChain);
			}

			I_DEBUG_LOG_INFO("SwapChainKHR: %p", (VkSwapchainKHR)m_swapChain);

			m_swapChainImages = device.Get().getSwapchainImagesKHR(m_swapChain);

			CreateImageViews(device);
		}

		

		void SwapChain::Destroy(Device& device) {
			for (int i = 0; i < m_swapChainImageViews.size(); i++) {
				I_DEBUG_LOG_INFO("SwapChainKHR: %p, Image View #%i Deleted: %p", (VkSwapchainKHR)m_swapChain, i, (VkImageView)m_swapChainImageViews[i]);
				device.Get().destroyImageView(m_swapChainImageViews[i]);
			}

			I_DEBUG_LOG_INFO("SwapChainKHR Deleted: %p", (VkSwapchainKHR)m_swapChain);
			device.Get().destroySwapchainKHR(m_swapChain);
		}

		// An image view is litteraly a view into the image. It describes how to access the image and which part of the image 
		// to access
		void SwapChain::CreateImageViews(Device& device) {
			m_swapChainImageViews = {};
			m_swapChainImageViews.resize(m_swapChainImages.size());

			for (int i = 0; i < m_swapChainImageViews.size(); i++) {
				vk::ImageViewCreateInfo imageViewCreateInfo{};
				imageViewCreateInfo.image = m_swapChainImages[i];
				// How the data should be interpreted
				imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
				imageViewCreateInfo.format = m_swapChainImageFormat;
				// the components field allows you to swizzle the colour channels around.
				// So I could change the A field to actually map to the B field
				// TEST
				imageViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
				imageViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
				imageViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
				imageViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
				// The subresourceRange field describes what the image's purpose is and which part of the image should be accessed. 
				// Our images will be used as color targets without any mipmapping levels or multiple layers.
				imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
				// Mipmaps
				imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
				// 3-D stereographic apps
				imageViewCreateInfo.subresourceRange.levelCount = 1;
				imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
				imageViewCreateInfo.subresourceRange.layerCount = 1;

				m_swapChainImageViews[i] = device.Get().createImageView(imageViewCreateInfo);
				I_DEBUG_LOG_INFO("SwapChainKHR: %p, Image View #%i: %p", (VkSwapchainKHR)m_swapChain, i, (VkImageView)m_swapChainImageViews[i]);
			}
		}

		vk::SurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
			// SRGB - more accurate perceived color
			for (const vk::SurfaceFormatKHR& availableFormat : availableFormats)
				if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear)
					return availableFormat;

			// If we can't find any preffered format and colour spaces we just get the first one
			return availableFormats[0];
		}

		vk::PresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes, bool vSync) {

			if (vSync) return vk::PresentModeKHR::eFifo;

			std::map<vk::PresentModeKHR, bool> presentModeAvailable;
			presentModeAvailable.insert(std::make_pair(vk::PresentModeKHR::eImmediate, false));
			presentModeAvailable.insert(std::make_pair(vk::PresentModeKHR::eMailbox, false));

			for (const vk::PresentModeKHR& presentMode : availablePresentModes) {
				// Tearing but has the least amount of latency
				if (presentMode == vk::PresentModeKHR::eImmediate) presentModeAvailable.find(presentMode)->second = true;
				// Fastest present mode in terms of latency and no tearing
				else if (presentMode == vk::PresentModeKHR::eMailbox) presentModeAvailable.find(presentMode)->second = true;
			}

			if (!vSync) {
				if (presentModeAvailable.find(vk::PresentModeKHR::eMailbox)->second == true) return vk::PresentModeKHR::eMailbox;
				else if (presentModeAvailable.find(vk::PresentModeKHR::eImmediate)->second == true) return vk::PresentModeKHR::eImmediate;
			}

			// Only present mode that is required to be available
			return vk::PresentModeKHR::eFifo;
		}

		vk::Extent2D SwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilites, const Window& window) {
			// Vulkan already has the window extent. Im not too sure though.
			#ifdef max
				#undef max
			#endif // max

			if (capabilites.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
				return capabilites.currentExtent;
			}
			// Vulkan can't get the windows extent?
			else {
				int width, height;
				glfwGetFramebufferSize(window.GetNativeHandle(), &width, &height);

				vk::Extent2D actualExtent = { (uint32_t)width, (uint32_t)height };

				// Clamp the framebuffer size to the min and max image extent
				actualExtent.width = std::clamp(actualExtent.width, capabilites.minImageExtent.width, capabilites.maxImageExtent.width);
				actualExtent.height = std::clamp(actualExtent.height, capabilites.minImageExtent.height, capabilites.maxImageExtent.height);

				return actualExtent;
			}

		}
	}
}