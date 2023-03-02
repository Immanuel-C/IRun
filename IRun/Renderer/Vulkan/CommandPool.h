#pragma once

#include "Device.h"

namespace IRun {
	namespace Vulkan {
		class CommandPool {
		public:
			CommandPool() = default;
			CommandPool(Device& device, uint32_t queueFamilyIndex, vk::CommandPoolCreateFlagBits flags);

			void Destroy(Device& device);

			vk::CommandPool& Get();
		private:
			vk::CommandPool m_commandPool;
		};
	}
}

