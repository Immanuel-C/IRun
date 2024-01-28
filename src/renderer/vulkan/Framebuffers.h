#pragma once

#include "Swapchain.h"
#include "Device.h"
#include "RenderPass.h"

namespace IRun {
	namespace Vk {
		class Framebuffers {
		public:
			Framebuffers() = default;
			Framebuffers(Swapchain& swapchain, RenderPass& renderPass, Device& device);

			void Destroy(Device& device);

			const inline std::vector<VkFramebuffer>& Get() const { return m_framebuffers; }

			VkFramebuffer operator[](int index) {
				return m_framebuffers[index];
			}

		private:
			std::vector<VkFramebuffer> m_framebuffers;
		};
	}
}

