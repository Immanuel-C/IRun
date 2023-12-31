#pragma once

#include <ILog.h>
#include <vulkan\vulkan.h>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <set>

#include "Instance.h"
#include "QueueFamilyIndices.h"
#include "Check.h"

namespace IRun {
	namespace VK {
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
			Device(const Instance& instance);
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
			// Delete copy constructor because VkDevice could be deleted by another copy.
			Device(const Device&) = delete;
			// Delete copy constructor because VkDevice could be deleted by another copy.
			const Device& operator=(const Device&) = delete;
		private:
			VkPhysicalDevice m_physicalDevice;
			VkDevice m_device;

			std::unordered_map<QueueType, VkQueue> m_queues;

			QueueFamilyIndices m_indices;

			void GetPhysicalDevice(const Instance& instance);
			void CreateDevice(const Instance& instance);

			QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);


			bool CheckDeviceSuitable(VkPhysicalDevice device);
		};
	}
}
