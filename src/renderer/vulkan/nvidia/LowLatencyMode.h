#pragma once

#include "../VendorIdentification.h"
#include "../Check.h"

namespace IRun {
	namespace Vk {
		namespace Nv {
			enum struct LowLatencyMode {
				None,
				On,
				Boost,
				OnBoost,
			};

			bool CheckIfVendorNv(const VkPhysicalDeviceProperties& props);

			void SetLowLatencyMode(VkDevice device, const VkPhysicalDeviceProperties& props, VkSwapchainKHR swapchain, LowLatencyMode mode);
			void LatencySleep(VkDevice device, const VkPhysicalDeviceProperties& props, VkSwapchainKHR swapchain, VkSemaphore signalSemaphore);
		}
	}
}

