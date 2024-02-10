#pragma once

#include "Device.h"

namespace IRun {
	namespace Vk {
		/// <summary>
		/// An index of the VkCommandBuffer in the IRun::Vk::CommandPool.
		/// </summary>
		typedef uint64_t CommandBuffer;
		/// <summary>
		/// The level of the command buffer.
		/// </summary>
		enum struct CommandBufferLevel {
			Primary = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			// vkCmdExecuteCommands(buffer)
			Secondary = VK_COMMAND_BUFFER_LEVEL_SECONDARY, 
		};
		/// <summary>
		/// A wrapper for VkCommandPool and VkCommandBuffer(s).
		/// </summary>
		class CommandPool {
		public:
			CommandPool() = default;
			/// <summary>
			/// Creates the command pool.
			/// </summary>
			/// <param name="device">A valid IRun::Vk::Device.</param>
			/// <param name="queueFamilyIndex">The queue family the command buffers in this command pool can be executed by.</param>
			/// <param name="flags">Flags for VkCommandPoolCreateInfo::flags. Must be a valid VkCommandPoolCreateFlags.</param>
			CommandPool(const Device& device, int queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
			/// <summary>
			/// Destroys the VkCommandPool and all VkCommandBuffers. The queue family that this command pool is used for or the device must be idle.
			/// </summary>
			/// <param name="device">A valid IRun::Vk::Device.</param>
			void Destroy(const Device& device);
			/// <summary>
			/// Creates a VkCommandBuffer.
			/// </summary>
			/// <param name="device">A valid IRun::Vk::Device.</param>
			/// <param name="level">The level of this command buffer.</param>
			/// <returns>The index of the command buffer in the CommandPool.</returns>
			CommandBuffer CreateBuffer(const Device& device, CommandBufferLevel level);
			/// <summary>
			/// Begin recording commands to a command buffer.
			/// </summary>
			/// <param name="device">A valid IRun::Vk::Device.</param>
			/// <param name="commandBuffer">The command buffer to record to.</param>
			/// <param name="usageFlags">A VkCommandBufferUsageFlags that will be passed into VkCommandBufferBeginInfo::flags.</param>
			/// <param name="inheritanceInfo">A VkCommandBufferInheritanceInfo that will be passed into VkCommandBufferBeginInfo::pInheritanceInfo</param>
			void BeginRecordingCommands(const Device& device, CommandBuffer commandBuffer, VkCommandBufferUsageFlags usageFlags = 0, std::optional<VkCommandBufferInheritanceInfo> inheritanceInfo = std::nullopt);
			/// <summary>
			/// End recording commands to a command buffer.
			/// </summary>
			/// <param name="commandBuffer">The command buffer to stop recording commands to.</param>
			void EndRecordingCommands(CommandBuffer commandBuffer);
			/// <summary>
			/// Destroy a specific command buffer. 
			/// </summary>
			/// <param name="commandBuffer">Command buffer to be destroyed.</param>
			void DestroyCommandBuffer(Device& device, CommandBuffer commandBuffer);
			inline VkCommandBuffer operator[](CommandBuffer commandBuffer) { return m_commandBuffers.at(commandBuffer); }
			/// <summary>
			/// Get the VkCommandBufferHandle.
			/// </summary>
			/// <param name="commandBuffer">Index of command buffer to retrieve.</param>
			/// <returns>Handle to VkCommandBuffer.</returns>
			inline const VkCommandBuffer Get(CommandBuffer commandBuffer) { return m_commandBuffers.at(commandBuffer); }

			inline const VkCommandPool GetHandle() const { return m_commandPool; }
		private:
			VkCommandPool m_commandPool;
			std::unordered_map<CommandBuffer, VkCommandBuffer> m_commandBuffers;
		};
	}
}

