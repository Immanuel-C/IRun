#pragma once

#include <ILog.h>
#include <vulkan\vulkan.h>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <set>

#include "Instance.h"
#include "Surface.h"
#include "QueueFamilyIndices.h"
#include "SwapchainDetails.h"
#include "Check.h"
#include "PNext.h"

namespace IRun {
	namespace Vk {
		enum struct QueueType {
			Graphics,
			Compute,
			Transfer,
			VideoDecode,
			Presentation,
		};

		class Device {
		public:
			Device() = default;
			/// <summary>
			/// Obtain the best physical device (APU, GPU, TPU, etc.) for our program, then create an interface between our physical device and IRun. Must be destroyed before the instance.
			/// </summary>
			/// <param name="instance"></param>
			Device(const Instance& instance, const Surface& surface);
			/// <summary>
			/// Destroy the device.
			/// </summary>
			void Destroy();
			/// <summary>
			/// Get the VkPhysicalDevice handle and the VkDevice handle.
			/// </summary>
			/// <returns>native Vulkan physical device and device handle.</returns>
			inline const std::pair<VkDevice, VkPhysicalDevice>& Get() const { return { m_device, m_physicalDevice}; }
			/// <summary>
			/// Get the native handle to all the queues.
			/// </summary>
			/// <returns></returns>
			inline const std::unordered_map<QueueType, VkQueue>& GetQueues() const { return m_queues; }
			/// <summary>
			/// Get the location of the Vulkan queue families.
			/// </summary>
			/// <returns>location of the Vulkan queue families</returns>
			const inline QueueFamilyIndices& GetQueueFamilies() const { return m_indices; }
			/// <summary>
			/// Get the swapchain details.
			/// </summary>
			/// <returns>swapchain details</returns>
			const inline SwapchainDetails& GetSwapchainDetails() const { return m_swapchainDetails; }

			const inline void ResetSwapchainDetails(Surface& surface) { m_swapchainDetails = FindSwapchainDetails(m_physicalDevice, surface); }

			/// <summary>
			/// Get the physical device properties.
			/// </summary>
			const inline VkPhysicalDeviceProperties& GetDeviceProperties() const { return m_deviceProperties; }
		private:
			VkPhysicalDevice m_physicalDevice = nullptr;
			VkDevice m_device;

			VkPhysicalDeviceProperties m_deviceProperties;

			std::unordered_map<QueueType, VkQueue> m_queues;

			std::array<const char*, 1> m_deviceExtensions = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};

			QueueFamilyIndices m_indices;
			SwapchainDetails m_swapchainDetails;

			void GetPhysicalDevice(const Instance& instance, const Surface& surface);
			void CreateDevice(const Instance& instance);

			QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, const Surface& surface);
			SwapchainDetails FindSwapchainDetails(VkPhysicalDevice device, const Surface& surface);


			bool CheckDeviceSuitable(VkPhysicalDevice device, const Surface& surface);
			bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		};
	}
}
