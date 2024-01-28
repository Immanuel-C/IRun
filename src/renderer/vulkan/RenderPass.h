#pragma once

#include "Device.h"
#include "Swapchain.h"

namespace IRun {
	namespace Vk {
		class RenderPass {
		public:
			RenderPass() = default;
			RenderPass(Device& device, Swapchain& swapchain);

			void Destroy(Device& device);

			const inline VkRenderPass Get() const { return m_renderPass; }

		private:
			VkRenderPass m_renderPass;
		};
	}
}

