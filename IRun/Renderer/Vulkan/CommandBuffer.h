#pragma once

#include "CommandPool.h"

namespace IRun {
	namespace Vulkan {
		// No destroy function. IRun::Vulkan::CommandPool handles that.
		class CommandBuffer {
		public:
			CommandBuffer() = default;
			CommandBuffer(Device& device, CommandPool& pool, vk::CommandBufferLevel level);

			void Reset(vk::CommandBufferResetFlagBits flags);

			void Begin(vk::CommandBufferUsageFlagBits flags, vk::CommandBufferInheritanceInfo* inheritanceInfo);
			void End();

			vk::CommandBuffer& Get();
		private: 
			vk::CommandBuffer m_commandBuffer;
		};
	}
}

