#include "CommandPool.h"


namespace IRun {
	namespace Vk {

		CommandPool::CommandPool(const Device& device, int queueFamilyIndex, VkCommandPoolCreateFlags flags) {
			VkCommandPoolCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			createInfo.queueFamilyIndex = queueFamilyIndex;
			createInfo.flags = flags;

			VK_CHECK(vkCreateCommandPool(device.Get().first, &createInfo, nullptr, &m_commandPool), "Failed to create Vulkan command pool");
			I_DEBUG_LOG_TRACE("Created Vulkan command pool: 0x%p", m_commandPool);
		}

		void CommandPool::Destroy(const Device& device) {
			I_DEBUG_LOG_TRACE("Destroyed Vulkan command pool: 0x%p", m_commandPool);
			vkDestroyCommandPool(device.Get().first, m_commandPool, nullptr);
			for (VkCommandBuffer buffer : m_commandBuffers) {
				I_DEBUG_LOG_TRACE("Destroyed Vulkan command buffer: 0x%p", buffer);
			}
		}

		CommandBuffer CommandPool::CreateBuffer(const Device& device, CommandBufferLevel level) {
			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = m_commandPool;
			allocInfo.level = (VkCommandBufferLevel)level;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;

			VK_CHECK(vkAllocateCommandBuffers(device.Get().first, &allocInfo, &commandBuffer), "Failed to create Vulkan command buffer");

			m_commandBuffers.push_back(commandBuffer);

			I_DEBUG_LOG_TRACE("Create Vulkan command buffer: 0x%p", m_commandBuffers[m_commandBuffers.size() - 1]);

			return m_commandBuffers.size() - 1;
		}

		void CommandPool::BeginRecordingCommands(const Device& device, CommandBuffer commandBuffer) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			VK_CHECK(vkBeginCommandBuffer(m_commandBuffers[commandBuffer], &beginInfo), "Failed to begin Vulkan command buffer");
		}
		void CommandPool::EndRecordingCommands(CommandBuffer commandBuffer) {
			VK_CHECK(vkEndCommandBuffer(m_commandBuffers[commandBuffer]), "Failed to end Vulkan command buffer");
		}
	}
}