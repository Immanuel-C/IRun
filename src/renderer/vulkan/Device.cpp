#include "Device.h"

#include "tools\Timer.h"

namespace IRun {
	namespace Vk {

		std::string StringQueueType(QueueType type) {
			std::string strQueueType{};
			switch (type)
			{
			case IRun::Vk::QueueType::Graphics:
				strQueueType = "graphics";
				break;
			case IRun::Vk::QueueType::Compute:
				strQueueType = "compute";
				break;
			case IRun::Vk::QueueType::Transfer:
				strQueueType = "transfer";
				break;
			case IRun::Vk::QueueType::VideoDecode:
				strQueueType = "video decode";
				break;
			case IRun::Vk::QueueType::Presentation:
				strQueueType = "presentation";
				break;
			default:
				break;
			}

			return strQueueType;
		}

		Device::Device(const Instance& instance, const Surface& surface) {
			GetPhysicalDevice(instance, surface);
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
	
		void Device::GetPhysicalDevice(const Instance& instance, const Surface& surface) {
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(instance.Get(), &deviceCount, nullptr);

			I_ASSERT_FATAL_ERROR(deviceCount == 0, "Failed to find any suitable physical devices. Does you system support Vulkan?");

			std::vector<VkPhysicalDevice> devices{};
			devices.resize(deviceCount);

			vkEnumeratePhysicalDevices(instance.Get(), &deviceCount, devices.data());

			for (const VkPhysicalDevice& device : devices) {
				if (CheckDeviceSuitable(device, surface)) {
					m_physicalDevice = device;
					I_DEBUG_LOG_TRACE("Acquired physical device: 0x%p", m_physicalDevice);
					break;
				}
			}

			I_ASSERT_FATAL_ERROR(!m_physicalDevice, "No suitable physical devices available! Abort!");

			VkPhysicalDeviceProperties deviceProps{};
			vkGetPhysicalDeviceProperties(m_physicalDevice, &deviceProps);

			I_LOG_INFO("Device Name: %s, Device Type: %s", deviceProps.deviceName, string_VkPhysicalDeviceType(deviceProps.deviceType));

			m_indices = FindQueueFamilies(m_physicalDevice, surface);
			m_swapchainDetails = FindSwapchainDetails(m_physicalDevice, surface);
		}

		void Device::CreateDevice(const Instance& instance) {

			// Probably a bad way to sort queue families and create queues but it works.
			std::vector<std::pair<QueueType, int>> queueFamiliesSorted{
				{ QueueType::Graphics, m_indices.graphicsFamily },
				{ QueueType::Presentation, m_indices.presentationFamily },
				{ QueueType::Transfer, m_indices.transferFamily },
				{ QueueType::VideoDecode, m_indices.videoDecodeFamily },
				{ QueueType::Compute, m_indices.computeFamily },
			};

			std::sort(queueFamiliesSorted.begin(), queueFamiliesSorted.end(), [=](std::pair<QueueType, int>& a, std::pair<QueueType, int>& b) {
				return a.second < b.second;
				});

			// Organize same queue families into a group
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

			std::vector<std::vector<float>> queuePriorites{};
			queuePriorites.resize(sameQueueFamilies.size());
			
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies{};
			queueFamilies.resize(queueFamilyCount);

			vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, queueFamilies.data());


			// Create queues in same queue family
			int i = 0;
			for (const std::unordered_map<QueueType, int>& sameQueueFamily : sameQueueFamilies) {
				VkDeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = sameQueueFamily.begin()->second;

				if (queueFamilies[sameQueueFamily.begin()->second].queueCount >= sameQueueFamily.size())
				{
					for (const std::pair<QueueType, int> sameQueue : sameQueueFamily) {
						switch (sameQueue.first)
						{
						// The graphics, presentation and transfer queues are critical for operation so the priority is the highest for them
						case QueueType::Graphics:
							queuePriorites[i].push_back(1.0f);
							break;
						case QueueType::Presentation:
							queuePriorites[i].push_back(1.0f);
							break;
						case QueueType::Transfer:
							queuePriorites[i].push_back(1.0f);
							break;
						case QueueType::VideoDecode:
							queuePriorites[i].push_back(0.75f);
							break;
						case QueueType::Compute:
							queuePriorites[i].push_back(0.5f);
							break;
						default:
							break;
						}
					}
					queueCreateInfo.queueCount = (uint32_t)sameQueueFamily.size();
					// Can create multiple queues in the same family and each queue will get a priority from scale 0.0 - 1.0.
					queueCreateInfo.pQueuePriorities = queuePriorites[i].data();
				}
				else {
					queueCreateInfo.queueCount = 1;
					float priority = 1.0f;
					queueCreateInfo.pQueuePriorities = &priority;
				}
				queueCreateInfos.push_back(queueCreateInfo);
				i++;
			}



			VkDeviceCreateInfo deviceCreateInfo{};
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCreateInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
			deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
			deviceCreateInfo.enabledExtensionCount = (uint32_t)m_deviceExtensions.size();
			deviceCreateInfo.ppEnabledExtensionNames = m_deviceExtensions.data();
			// Deprecated in Vulkan 1.1
			deviceCreateInfo.enabledLayerCount = 0;
			deviceCreateInfo.ppEnabledLayerNames = nullptr;

			VkPhysicalDeviceFeatures deviceFeatures{};
			deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

			VK_CHECK(vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device), "Failed to create Vulkan Logical Device!");
			I_DEBUG_LOG_TRACE("Created Vulkan device: 0x%p", m_device);


			// Retrieve the queues
			for (const std::unordered_map<QueueType, int>& sameQueueFamily : sameQueueFamilies) {
				i = 0;
				for (auto itr = sameQueueFamily.begin(); itr != sameQueueFamily.end(); itr++) {
					
					std::string strQueueType = StringQueueType(itr->first);
					VkQueue queue = nullptr;

					if (queueFamilies[itr->second].queueCount >= sameQueueFamily.size()) {
						vkGetDeviceQueue(m_device, itr->second, i, &queue);
						m_queues.insert({ itr->first, queue });
						I_DEBUG_LOG_TRACE("Created Vulkan %s queue: 0x%p (queue family: %i, queue index: %i) from device: 0x%p", strQueueType.c_str(), queue, itr->second, i, m_device);
					}
					else {
						vkGetDeviceQueue(m_device, itr->second, 0, &queue);
						m_queues.insert({ itr->first, queue });
						I_DEBUG_LOG_TRACE("Created Vulkan %s queue: 0x%p (queue family: %i) from device: 0x%p", strQueueType.c_str(), queue, itr->second, m_device);
					}
					i++;
				}

			}

		}


		struct QueueFamilyCount {
			int index;
			uint32_t queueCount;
		};


		static void CompareQueueCounts(QueueType type, std::unordered_map<QueueType, QueueFamilyCount>& supportedQueuesinQueueFamilies, VkQueueFamilyProperties queueFamilyProps, int currentIndex) {
			// Check if QueueType is in map if not create a new key.
			if (supportedQueuesinQueueFamilies.find(type) == supportedQueuesinQueueFamilies.end())
				supportedQueuesinQueueFamilies.insert({ type, { currentIndex, queueFamilyProps.queueCount } });
			else {
				// If the new queue family has a higher queue count than set queue creation of this type to the new queue family
				if (queueFamilyProps.queueCount > supportedQueuesinQueueFamilies[QueueType::Graphics].queueCount) {
					supportedQueuesinQueueFamilies.erase(type);
					supportedQueuesinQueueFamilies.insert({ type, { currentIndex, queueFamilyProps.queueCount } });
				}
			}
		}

		QueueFamilyIndices Device::FindQueueFamilies(VkPhysicalDevice device, const Surface& surface)
		{

			if (device == nullptr) {
				device = m_physicalDevice;
			}

			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies{};
			queueFamilies.resize(queueFamilyCount);

			// The queueFamilies list is in order of queue family so the location of the queue family we are trying to get is equal to the number of times the loop has run.
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

			std::unordered_map<QueueType, QueueFamilyCount> supportedQueuesinQueueFamilies{};

			int i = 0;
			for (const VkQueueFamilyProperties& queueFamilyProps : queueFamilies) {
				VkBool32 presentationQueue = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface.Get(), &presentationQueue);

				// Have to check if a queue family has more than one queue because a queue can have 0 queues for some reason.
				if (queueFamilyProps.queueCount > 0 && queueFamilyProps.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					CompareQueueCounts(QueueType::Graphics, supportedQueuesinQueueFamilies, queueFamilyProps, i);
				if (queueFamilyProps.queueCount > 0 && queueFamilyProps.queueFlags & VK_QUEUE_COMPUTE_BIT)
					CompareQueueCounts(QueueType::Compute, supportedQueuesinQueueFamilies, queueFamilyProps, i);
				if (queueFamilyProps.queueCount > 0 && queueFamilyProps.queueFlags & VK_QUEUE_TRANSFER_BIT)
					CompareQueueCounts(QueueType::Transfer, supportedQueuesinQueueFamilies, queueFamilyProps, i);
				if (queueFamilyProps.queueCount > 0 && queueFamilyProps.queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR)
					CompareQueueCounts(QueueType::VideoDecode, supportedQueuesinQueueFamilies, queueFamilyProps, i);
				if (queueFamilyProps.queueCount > 0 && presentationQueue) 
					CompareQueueCounts(QueueType::Presentation, supportedQueuesinQueueFamilies, queueFamilyProps, i);

				i++;
			}

			QueueFamilyIndices indices{};


			indices.graphicsFamily = supportedQueuesinQueueFamilies[QueueType::Graphics].index;
			indices.computeFamily = supportedQueuesinQueueFamilies[QueueType::Compute].index;
			indices.transferFamily = supportedQueuesinQueueFamilies[QueueType::Transfer].index;
			indices.videoDecodeFamily = supportedQueuesinQueueFamilies[QueueType::VideoDecode].index;
			indices.presentationFamily = supportedQueuesinQueueFamilies[QueueType::Presentation].index;

			return indices;
		}

		bool Device::CheckDeviceSuitable(VkPhysicalDevice device, const Surface& surface) {

			/*
			VkPhysicalDeviceProperties2 deviceProps{};
			vkGetPhysicalDeviceProperties2(device, &deviceProps);

			VkPhysicalDeviceFeatures2 deviceFeatures{};
			vkGetPhysicalDeviceFeatures2(device, &deviceFeatures);
			*/


			QueueFamilyIndices indices = FindQueueFamilies(device, surface);
			SwapchainDetails details = FindSwapchainDetails(device, surface);

			bool extensionsSupported = CheckDeviceExtensionSupport(device);
			
			bool swapchainValid = false;

			if (extensionsSupported) swapchainValid = !details.formats.empty() && !details.presentModes.empty();

			return indices.IsValid() && extensionsSupported && swapchainValid;
		}

		bool Device::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
			uint32_t extensionCount = 0;
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

			if (!extensionCount) return false;

			std::vector<VkExtensionProperties> extensions{};
			extensions.resize(extensionCount);

			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());
			
			int supportedCount = 0;
			for (const VkExtensionProperties& extension : extensions) 
				for (const char* requiredExtension : m_deviceExtensions) 
					if (strcmp(extension.extensionName, requiredExtension) == '\0') {
						supportedCount++;
						if (supportedCount == m_deviceExtensions.size()) return true;
					}

			return false;
		}

		SwapchainDetails Device::FindSwapchainDetails(VkPhysicalDevice device, const Surface& surface) {
			SwapchainDetails swapchainDetails{};

			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface.Get(), &swapchainDetails.capabilities);

			uint32_t formatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.Get(), &formatCount, nullptr);

			if (formatCount) {
				swapchainDetails.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.Get(), &formatCount, swapchainDetails.formats.data());
			}

			uint32_t presentModeCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.Get(), &presentModeCount, nullptr);

			if (presentModeCount) {
				swapchainDetails.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.Get(), &presentModeCount, swapchainDetails.presentModes.data());
			}

			return swapchainDetails;
		}
	}
}