#include "Device.h"


namespace IRun {
	namespace VK {

		std::string StringQueueType(QueueType type) {
			std::string strQueueType{};
			switch (type)
			{
			case IRun::VK::QueueType::Graphics:
				strQueueType = "graphics";
				break;
			case IRun::VK::QueueType::Compute:
				strQueueType = "compute";
				break;
			case IRun::VK::QueueType::Transfer:
				strQueueType = "transfer";
				break;
			case IRun::VK::QueueType::VideoDecode:
				strQueueType = "video decode";
				break;
			case IRun::VK::QueueType::Presentation:
				strQueueType = "presentation";
				break;
			default:
				break;
			}

			return strQueueType;
		}

		Device::Device(const Instance& instance) {
			GetPhysicalDevice(instance);
			CreateDevice(instance);
		}

		// Don't need to destroy the queues because they are created and destroyed with the device.
		void Device::Destroy() {
			vkDestroyDevice(m_device, nullptr);
			I_DEBUG_LOG_TRACE("Destroyed Vulkan Device: 0x%p", m_device);

			for (const std::pair<QueueType, VkQueue>& queue : m_queues) {
				std::string strQueueType = StringQueueType(queue.first);
				I_DEBUG_LOG_TRACE("Destroyed Vulkan %s queue: 0x%p from device: 0x%p", strQueueType.c_str(), queue.second, m_device);
			}
		}
	
		void Device::GetPhysicalDevice(const Instance& instance) {
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(instance.Get(), &deviceCount, nullptr);

			I_ASSERT_FATAL_ERROR(deviceCount == 0, "Failed to find any suitable physical devices. Does you system support Vulkan?");

			std::vector<VkPhysicalDevice> devices{};
			devices.resize(deviceCount);

			vkEnumeratePhysicalDevices(instance.Get(), &deviceCount, devices.data());

			for (const VkPhysicalDevice& device : devices) {
				if (CheckDeviceSuitable(device)) {
					m_physicalDevice = device;
					I_DEBUG_LOG_TRACE("Acquired physical device: 0x%p", m_physicalDevice);
					break;
				}
			}

			m_indices = FindQueueFamilies(m_physicalDevice);
		}

		void Device::CreateDevice(const Instance& instance) {
			std::vector<std::pair<QueueType, int>> queueFamiliesSorted{
				{ QueueType::Graphics, m_indices.graphicsFamily },
				{ QueueType::Transfer, m_indices.transferFamily },
				{ QueueType::VideoDecode, m_indices.videoDecodeFamily },
				{ QueueType::Compute, m_indices.computeFamily },
			};

			std::sort(queueFamiliesSorted.begin(), queueFamiliesSorted.end(), [=](std::pair<QueueType, int>& a, std::pair<QueueType, int>& b) {
				return a.second < b.second;
				});

			std::vector<std::unordered_map<QueueType, int>> sameQueueFamilies{};
			sameQueueFamilies.resize(1);
			int vectorSize = 0, prevQueueFamilyIndex = queueFamiliesSorted[0].second;
			for (const std::pair<QueueType, int> queueFamily : queueFamiliesSorted) {
				if (prevQueueFamilyIndex != queueFamily.second) {
					vectorSize++;
					sameQueueFamilies.resize(vectorSize + 1);
					prevQueueFamilyIndex = queueFamily.second;
				}
				sameQueueFamilies[vectorSize].insert(queueFamily);
			}

			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};


			int i = 0;
			for (const std::unordered_map<QueueType, int>& sameQueueFamily : sameQueueFamilies) {
				VkDeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = sameQueueFamily.begin()._Ptr->_Myval.second;
				queueCreateInfo.queueCount = 1;

				// Can create multiple queues in the same family and each queue will get a priority from scale 0.0 - 1.0.
				float priority = 1.0f;
				queueCreateInfo.pQueuePriorities = &priority;

				queueCreateInfos.push_back(queueCreateInfo);
				i++;
			}

			VkDeviceCreateInfo deviceCreateInfo{};
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
			deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
			// Optional
			deviceCreateInfo.enabledExtensionCount = 0;
			deviceCreateInfo.ppEnabledExtensionNames = nullptr;
			// Deprecated in Vulkan 1.1
			deviceCreateInfo.enabledLayerCount = 0;
			deviceCreateInfo.ppEnabledLayerNames = nullptr;

			VkPhysicalDeviceFeatures deviceFeatures{};
			deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

			VK_CHECK(vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device), "Failed to create Vulkan Logical Device!");
			I_DEBUG_LOG_TRACE("Created Vulkan device: 0x%p", m_device);

			for (const std::unordered_map<QueueType, int>& sameQueueFamily : sameQueueFamilies) {
				for (auto itr = sameQueueFamily.begin(); itr != sameQueueFamily.end(); itr++) {
					
					std::string strQueueType = StringQueueType(itr->first);

					VkQueue queue = nullptr;
					vkGetDeviceQueue(m_device, itr->second, 0, &queue);
					m_queues.insert({ itr->first, queue });
					I_DEBUG_LOG_TRACE("Created Vulkan %s queue: 0x%p (queue family: %i) from device: 0x%p", strQueueType.c_str(), queue, itr->second, m_device);
				}

			}
		}
		QueueFamilyIndices Device::FindQueueFamilies(VkPhysicalDevice device)
		{
			QueueFamilyIndices indices{};

			if (device == nullptr) {
				device = m_physicalDevice;
			}

			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies{};
			queueFamilies.resize(queueFamilyCount);

			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
			// The queueFamilies list is in order of queue family so the location of the queue family we are trying to get is equal to the number of times the loop has run.
			int i = 0;
			for (const VkQueueFamilyProperties& queueFamilyProps : queueFamilies) {
				// Have to check if a queue family has more than one queue because a queue can have 0 queues for some reason.
				if (queueFamilyProps.queueCount > 0 && queueFamilyProps.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					indices.graphicsFamily = i;
				if (queueFamilyProps.queueCount > 0 && queueFamilyProps.queueFlags & VK_QUEUE_COMPUTE_BIT)
					indices.computeFamily = i;
				if (queueFamilyProps.queueCount > 0 && queueFamilyProps.queueFlags & VK_QUEUE_TRANSFER_BIT)
					indices.transferFamily = i;
				if (queueFamilyProps.queueCount > 0 && queueFamilyProps.queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR)
					indices.videoDecodeFamily = i;

				if (indices.IsValid()) break;

				i++;
			}

			return indices;
		}

		bool Device::CheckDeviceSuitable(VkPhysicalDevice device) {

			/*
			VkPhysicalDeviceProperties deviceProps{};
			vkGetPhysicalDeviceProperties(device, &deviceProps);

			VkPhysicalDeviceFeatures deviceFeatures{};
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
			*/

			QueueFamilyIndices indices = FindQueueFamilies(device);

			return indices.IsValid();
		}
	}
}