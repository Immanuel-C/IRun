#pragma once

#include <vulkan/vulkan.hpp>
#include <map>

#include "Instance.h"
#include "QueueFamily.h"
#include "Surface.h"
#include "SwapChainDetails.h"

namespace IRun {
	namespace Vulkan {
		class PhysicalDevice {
		public:
			PhysicalDevice() = default;
			PhysicalDevice(const Instance& instance, Surface& surface);

			vk::PhysicalDevice& Get();
			const vk::PhysicalDeviceFeatures& GetPhysicalDeviceFeatures();

			operator vk::PhysicalDevice() { return m_physicalDevice; }
			operator vk::PhysicalDevice& () { return m_physicalDevice; }

		private:
			bool CheckDeviceExtensionSupport(vk::PhysicalDevice& device);
			uint32_t RateDeviceSuitability(vk::PhysicalDevice& device, Surface& surface);

			vk::PhysicalDeviceFeatures m_features;

			vk::PhysicalDevice m_physicalDevice;
		};
	}
}

