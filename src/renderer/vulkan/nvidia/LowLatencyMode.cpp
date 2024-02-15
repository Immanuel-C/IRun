#include "LowLatencyMode.h"


namespace IRun {
	namespace Vk {
		namespace Nv {
			static VkResult VkSetLatencySleepModeNV(VkDevice device, VkSwapchainKHR swapchain, VkLatencySleepModeInfoNV* sleepModeInfo) {
				auto fun = (PFN_vkSetLatencySleepModeNV)vkGetDeviceProcAddr(device, "vkSetLatencySleepModeNV");

				if (!fun)
					return VK_ERROR_EXTENSION_NOT_PRESENT;

				return fun(device, swapchain, sleepModeInfo);
			}

			static VkResult VkLatencySleepNV(VkDevice device, VkSwapchainKHR swapchain, VkLatencySleepInfoNV* sleepInfo) {
				auto fun = (PFN_vkLatencySleepNV)vkGetDeviceProcAddr(device, "vkLatencySleepNV");

				if (!fun)
					return VK_ERROR_EXTENSION_NOT_PRESENT;

				return fun(device, swapchain, sleepInfo);
			}
			
			bool CheckIfVendorNv(const VkPhysicalDeviceProperties& props) {
				if (props.vendorID != NVIDIA_VENDORID)
					return false;

				return true;
			}

			void SetLowLatencyMode(VkDevice device, const VkPhysicalDeviceProperties& props, VkSwapchainKHR swapchain, LowLatencyMode mode) {
				if (!CheckIfVendorNv(props)) return;

				// VkLatencySleepModeInfoNV::lowLatencyBoost will increase power draw by gpu.
				VkLatencySleepModeInfoNV sleepModeInfo{};
				sleepModeInfo.sType = VK_STRUCTURE_TYPE_LATENCY_SLEEP_MODE_INFO_NV;

				switch (mode)
				{
				case LowLatencyMode::None:
					sleepModeInfo.lowLatencyMode = VK_FALSE;
					sleepModeInfo.lowLatencyBoost = VK_FALSE;
					break;
				case LowLatencyMode::On:
					sleepModeInfo.lowLatencyMode = VK_TRUE;
					sleepModeInfo.lowLatencyBoost = VK_FALSE;
					break;
				case LowLatencyMode::Boost:
					sleepModeInfo.lowLatencyMode = VK_FALSE;
					sleepModeInfo.lowLatencyBoost = VK_TRUE;
					break;
				case LowLatencyMode::OnBoost:
					sleepModeInfo.lowLatencyMode = VK_TRUE;
					sleepModeInfo.lowLatencyBoost = VK_TRUE;
					break;
				default:
					I_ASSERT_FATAL_ERROR(true, "SetLowLatencyMode(..., LowLatencyMode) failed. Param IRun::Vk::Nv::LowLatencyMode is invalid!");
					break;
				}

				sleepModeInfo.minimumIntervalUs = 0;

				VK_CHECK(VkSetLatencySleepModeNV(device, swapchain, &sleepModeInfo), "Failed to initialize VK_NV_low_latency2 Vulkan extension!");
			}

			void LatencySleep(VkDevice device, const VkPhysicalDeviceProperties& props, VkSwapchainKHR swapchain, VkSemaphore signalSemaphore) {
				if (!CheckIfVendorNv(props)) return;

				VkLatencySleepInfoNV sleepInfo{};
				sleepInfo.sType = VK_STRUCTURE_TYPE_LATENCY_SLEEP_INFO_NV;
				sleepInfo.signalSemaphore = signalSemaphore;
				sleepInfo.value = 0;

				VK_CHECK(VkLatencySleepNV(device, swapchain, &sleepInfo), "Failed to initialize VK_NV_low_latency2 Vulkan extension!");
			}

		}
	}
}