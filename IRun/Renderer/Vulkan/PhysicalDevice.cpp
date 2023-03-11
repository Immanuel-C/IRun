#include "PhysicalDevice.h"


namespace IRun {
	namespace Vulkan {
		PhysicalDevice::PhysicalDevice(const Instance& instance, Surface& surface) {
			std::vector<vk::PhysicalDevice> devices = instance.Get().enumeratePhysicalDevices();

			I_ASSERT_FATAL_ERROR(devices.size() == 0, "Failed to find any devices that support Vulkan!");

			std::map<uint32_t, VkPhysicalDevice> candidates;

			for (vk::PhysicalDevice& device : devices) {
				uint32_t score = RateDeviceSuitability(device, surface);
				candidates.insert(std::make_pair(score, device));
			}

			// Check if score is higher than 0
			I_ASSERT_FATAL_ERROR(candidates.rbegin()->first == 0, "No suitable graphics cards!");
			m_physicalDevice = candidates.rbegin()->second;

			m_features = m_physicalDevice.getFeatures();

			I_DEBUG_LOG_INFO("Physical Device: %p", (VkPhysicalDevice)m_physicalDevice);
		}

		vk::PhysicalDevice& PhysicalDevice::Get() {
			return m_physicalDevice;
		}

		const vk::PhysicalDeviceFeatures& PhysicalDevice::GetPhysicalDeviceFeatures() { return m_features; }

		bool PhysicalDevice::CheckDeviceExtensionSupport(vk::PhysicalDevice& device) {
			std::array<const char*, 1> requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME};
			std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

			uint32_t supportedExtensionCount = 0;
			for (const vk::ExtensionProperties& availableExtension : availableExtensions) {
				for (const char* requiredExtension : requiredExtensions) {
					if (strcmp(requiredExtension, availableExtension.extensionName) == 0) supportedExtensionCount++;
				}

				if (supportedExtensionCount == requiredExtensions.size()) break;
			}

			if (supportedExtensionCount != requiredExtensions.size()) return false;

			return true;
		}

		uint32_t PhysicalDevice::RateDeviceSuitability(vk::PhysicalDevice& device, Surface& surface) {

			uint32_t score = 0;

			if (device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) score += 2;
			else if (device.getProperties().deviceType == vk::PhysicalDeviceType::eIntegratedGpu) score += 1;
			
			score += device.getProperties().limits.maxImageDimension2D;

			QueueFamilyIndices indices = QueueFamilyIndices::FindQueueFamilies(device, surface.Get());


			if (!indices.IsComplete() || !CheckDeviceExtensionSupport(device)) return 0;

			SwapChainSupportDetails swapchainSupport{};
			SwapChainSupportDetails::QuerySwapChainSupport(device, surface, swapchainSupport);
			bool swapChainAdequate = !swapchainSupport.formats.empty() || !swapchainSupport.presentModes.empty();

			if (!swapChainAdequate) return 0;

			return score;
		}
	}
}
