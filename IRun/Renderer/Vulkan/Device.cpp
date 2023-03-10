#include "Device.h"


namespace IRun {
	namespace Vulkan {
		Device::Device(PhysicalDevice& physicalDevice, Surface& surface) {
			QueueFamilyIndices indices = QueueFamilyIndices::FindQueueFamilies(physicalDevice, surface);


			std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
			std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

			float queuePriority = 1.0f;
			for (uint32_t queueFamily : uniqueQueueFamilies) {
				vk::DeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.queueFamilyIndex = queueFamily;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;
				queueCreateInfos.push_back(queueCreateInfo);
			}

			vk::DeviceCreateInfo deviceCreateInfo{};
			deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
			deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();

			std::array<const char*, 1> requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
			deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
			deviceCreateInfo.enabledExtensionCount = requiredExtensions.size();

			// Enable all available features
			m_enabledFeatures = physicalDevice.GetPhysicalDeviceFeatures();
			deviceCreateInfo.pEnabledFeatures = &m_enabledFeatures;


			VK_CHECK(physicalDevice.Get().createDevice(&deviceCreateInfo, nullptr, &m_device), "Failed to create Vulkan logical device!");

			m_device.getQueue(indices.graphicsFamily.value(), 0, &m_graphicsQueue);
			m_device.getQueue(indices.presentFamily.value(), 0, &m_presentQueue);

			I_DEBUG_LOG_INFO("Device: %p", (VkDevice)m_device);
			I_DEBUG_LOG_INFO("Graphics Queue: %p", (VkQueue)m_graphicsQueue);
			I_DEBUG_LOG_INFO("Present Queue: %p", (VkQueue)m_presentQueue);
		}

		void Device::Destroy() {
			I_DEBUG_LOG_INFO("Device Deleted: %p", (VkDevice)m_device);
			I_DEBUG_LOG_INFO("Graphics Queue Deleted: %p", (VkQueue)m_graphicsQueue);
			I_DEBUG_LOG_INFO("Present Queue Deleted: %p", (VkQueue)m_presentQueue);
			m_device.destroy();
		}

		const vk::PhysicalDeviceFeatures& Device::GetEnabledPhysicalDeviceFeatures() { return m_enabledFeatures; }

		vk::Device& Device::Get() { return m_device; }

		vk::Queue& Device::GetGraphicsQueue() { return m_graphicsQueue; }

		vk::Queue& Device::GetPresentQueue() { return m_presentQueue; }

	}
}