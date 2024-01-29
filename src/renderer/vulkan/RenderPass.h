#pragma once

#include "Device.h"
#include "Swapchain.h"

namespace IRun {
	namespace Vk {
		class RenderPass {
		public:
			RenderPass() = default;
			/// <summary>
			/// Create the VkRenderPass.
			/// </summary>
			/// <param name="device">A valid IRun::Vk::Device.</param>
			/// <param name="swapchain">A valid IRun::Vk::Swapchain</param>
			RenderPass(Device& device, Swapchain& swapchain);
			/// <summary>
			/// Destroys the VkRenderPass;
			/// </summary>
			/// <param name="device">A valid IRun::Vk::Device.</param>
			void Destroy(Device& device);
			/// <returns>Handle to the VkRenderPass</returns>
			const inline VkRenderPass Get() const { return m_renderPass; }

		private:
			VkRenderPass m_renderPass;
		};
	}
}

