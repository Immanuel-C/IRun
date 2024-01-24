#pragma once

#include "Device.h"

namespace IRun {
	namespace Vk {
		typedef uint64_t CommandBuffer;

		enum struct CommandBufferLevel {
			Primary = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			Secondary = VK_COMMAND_BUFFER_LEVEL_SECONDARY, // vkCmdExecuteCommands(buffer)
		};

		class CommandPool {
		public:
			CommandPool() = default;
			CommandPool(const Device& device, int queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);

			void Destroy(const Device& device);

			CommandBuffer CreateBuffer(const Device& device, CommandBufferLevel level);

			void BeginRecordingCommands(const Device& device, CommandBuffer commandBuffer);
			void EndRecordingCommands(CommandBuffer commandBuffer);

			VkCommandBuffer operator[](CommandBuffer commandBuffer) {
				return m_commandBuffers[(uint64_t)commandBuffer];
			}

			inline const VkCommandBuffer Get(CommandBuffer commandBuffer) { return m_commandBuffers[commandBuffer]; }

		private:
			VkCommandPool m_commandPool;
			std::vector<VkCommandBuffer> m_commandBuffers;
		};
	}
}

