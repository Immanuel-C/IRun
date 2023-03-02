#include "CommandBuffer.h"

namespace IRun {
	namespace Vulkan {
		CommandBuffer::CommandBuffer(Device& device, CommandPool& pool, vk::CommandBufferLevel level)  {
			vk::CommandBufferAllocateInfo allocInfo{};
			allocInfo.commandPool = pool.Get();
			// vk::CommandBufferLevel::ePrimary: Can be submitted to a queue for execution, but cannot be called from other command buffers.
			// vk::CommandBufferLevel::eSecondary: Cannot be submitted directly, but can be called from primary command buffers.
			allocInfo.level = level;
			allocInfo.commandBufferCount = 1;

			VK_CHECK(device.Get().allocateCommandBuffers(&allocInfo, &m_commandBuffer), "Failed to allocate a Vulkan command buffer!");
			I_DEBUG_LOG_INFO("Command Buffer: %p, Command Pool: %p", (VkCommandBuffer)m_commandBuffer, (VkCommandPool)pool.Get());
		}

		void CommandBuffer::Reset(vk::CommandBufferResetFlagBits flags) {
			m_commandBuffer.reset(flags);
		}

		void CommandBuffer::Begin(vk::CommandBufferUsageFlagBits flags, vk::CommandBufferInheritanceInfo* inheritanceInfo) {
			vk::CommandBufferBeginInfo beginInfo{};
			// VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer will be rerecorded right after executing it once.
			// VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary command buffer that will be entirely within a single render pass.
			// VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT : The command buffer can be resubmitted while it is also already pending execution.
			beginInfo.flags = flags;
			// The pInheritanceInfo parameter is only relevant for secondary command buffers.It specifies which state to inherit from the calling primary command buffers.
			beginInfo.pInheritanceInfo = inheritanceInfo;

			m_commandBuffer.begin(beginInfo);
		}

		void CommandBuffer::End() {
			m_commandBuffer.end();
		}

		vk::CommandBuffer& CommandBuffer::Get() { return m_commandBuffer; }
	}
}