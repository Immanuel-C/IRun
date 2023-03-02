#pragma once

#include "Device.h"

namespace IRun {
	namespace Vulkan {
		class Semaphore {
		public:
			Semaphore() = default;
			Semaphore(Device& device, vk::SemaphoreCreateFlagBits flags);
			void Destroy(Device& device);
			vk::Semaphore& Get();
		private:
			vk::Semaphore m_semaphore;
		};

		class Fence {
		public:
			Fence() = default;
			Fence(Device& device, vk::FenceCreateFlagBits flags);
			void Destroy(Device& device);

			void Wait(Device& device);
			void Reset(Device& device);

			vk::Fence& Get();
		private:
			vk::Fence m_fence;
		};
	}
}

