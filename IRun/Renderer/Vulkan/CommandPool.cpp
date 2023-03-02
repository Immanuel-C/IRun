#include "CommandPool.h"

namespace IRun {
	namespace Vulkan {
		CommandPool::CommandPool(Device& device, uint32_t queueFamilyIndex, vk::CommandPoolCreateFlagBits flags) {
			vk::CommandPoolCreateInfo poolInfo{};
			// vk::CommandPoolCreateFlagBits::eTransiet: Hint that command buffers are rerecorded with new commands very often(may change memory allocation behavior)
			// vk::CommandPoolCreateFlagBits::eResetCommandBuffer : Allow command buffers to be rerecorded individually, without this flag they all have to be reset together
			poolInfo.flags = flags;
			// Queue family we will be sending commands to.
			poolInfo.queueFamilyIndex = queueFamilyIndex;

			m_commandPool = device.Get().createCommandPool(poolInfo);
			I_DEBUG_LOG_INFO("Command Pool: %p", (VkCommandPool)m_commandPool);
		}

		void CommandPool::Destroy(Device& device) {
			// Destroys all command buffers allocated inside pool
			I_DEBUG_LOG_INFO("Command Pool Destroyed: %p", (VkCommandPool)m_commandPool);
			device.Get().destroyCommandPool(m_commandPool);
		}

		vk::CommandPool& CommandPool::Get() { return m_commandPool; }
	}
}