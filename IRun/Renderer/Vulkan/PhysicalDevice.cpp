#include "PhysicalDevice.h"


namespace IRun {
	namespace Vulkan {
		PhysicalDevice::PhysicalDevice(const Instance& instance, Surface& surface) {
			std::vector<vk::PhysicalDevice> devices = instance.Get().enumeratePhysicalDevices();

			I_ASSERT_FATAL_ERROR(devices.size() == 0, "Failed to find any devices that support Vulkan!");

			std::map<uint32_t, VkPhysicalDevice> candidates;

			for (const vk::PhysicalDevice& device : devices) {
				uint32_t score = RateDeviceSuitability(device, surface);
				candidates.insert(std::make_pair(score, device));
			}

			// Check if score is higher than 0
			I_ASSERT_FATAL_ERROR(candidates.rbegin()->first == 0, "No suitable graphics cards!");
			m_physicalDevice = candidates.rbegin()->second;

			I_DEBUG_LOG_INFO("Physical Device: %p", (VkPhysicalDevice)m_physicalDevice);
		}

		vk::PhysicalDevice& PhysicalDevice::Get() {
			return m_physicalDevice;
		}

		bool PhysicalDevice::CheckDeviceExtensionSupport(const vk::PhysicalDevice& device) {
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

		uint32_t PhysicalDevice::RateDeviceSuitability(const vk::PhysicalDevice& device, Surface& surface) {

			uint32_t score = 0;

			if (device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) score += 2;
			else if (device.getProperties().deviceType == vk::PhysicalDeviceType::eIntegratedGpu) score += 1;
			
			score += device.getProperties().limits.maxImageDimension2D;

			QueueFamilyIndices indices = QueueFamilyIndices::FindQueueFamilies(device, surface);

			if (!indices.IsComplete() || !CheckDeviceExtensionSupport(device)) score = 0;

			return score;
		}
	}
}
