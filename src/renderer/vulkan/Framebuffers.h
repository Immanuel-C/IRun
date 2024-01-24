#pragma once

#include "Swapchain.h"
#include "Device.h"

namespace IRun {
	namespace Vk {
		class Framebuffers {
		public:
			Framebuffers() = default;
			Framebuffers(const Swapchain& swapchain, VkRenderPass renderPass, const Device& device);

			void Destroy(const Device& device);

			const inline std::vector<VkFramebuffer>& Get() const { return m_framebuffers; }

			VkFramebuffer operator[](int index) {
				return m_framebuffers[index];
			}

		private:
			std::vector<VkFramebuffer> m_framebuffers;
		};
	}
}

