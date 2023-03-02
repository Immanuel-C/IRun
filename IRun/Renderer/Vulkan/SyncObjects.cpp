#include "SyncObjects.h"



namespace IRun {
	namespace Vulkan {
		Semaphore::Semaphore(Device& device, vk::SemaphoreCreateFlagBits flags) {
			vk::SemaphoreCreateInfo semaphoreInfo{};
			semaphoreInfo.flags = flags;
			m_semaphore = device.Get().createSemaphore(semaphoreInfo);
			I_DEBUG_LOG_INFO("Semaphore: %p", (VkSemaphore)m_semaphore);
		}

		void Semaphore::Destroy(Device& device) {
			I_DEBUG_LOG_INFO("Semaphore Destroyed: %p", (VkSemaphore)m_semaphore);
			device.Get().destroySemaphore(m_semaphore);
		}

		vk::Semaphore& Semaphore::Get() {
			return m_semaphore;
		}

		Fence::Fence(Device& device, vk::FenceCreateFlagBits flags) {
			vk::FenceCreateInfo fenceInfo{};
			fenceInfo.flags = flags;
			m_fence = device.Get().createFence(fenceInfo);
			I_DEBUG_LOG_INFO("Fence: %p", (VkFence)m_fence);

		}

		void Fence::Destroy(Device& device) {
			I_DEBUG_LOG_INFO("Fence Destroyed: %p", (VkFence)m_fence);
			device.Get().destroyFence(m_fence);
		}

		void Fence::Wait(Device& device) {
			// The waitAll could equal true or false doesn't matter since we have 1 fence.
			VK_CHECK(device.Get().waitForFences(m_fence, true, UINT64_MAX), "Failed to wait for Vulkan fence");
		}

		void Fence::Reset(Device& device) {
			// Have to reset the fence because it is signal.
			device.Get().resetFences(m_fence);
		}

		vk::Fence& IRun::Vulkan::Fence::Get() {
			return m_fence;
		}
	}
}