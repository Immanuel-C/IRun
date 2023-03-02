#pragma once

#include "SwapChain.h"
#include "GraphicsPipeline.h"

namespace IRun {
	namespace Vulkan {
		class Framebuffer {
		public:
			Framebuffer(Device& device, SwapChain& swapChain, GraphicsPipeline& graphicsPipeline);
			std::vector<vk::Framebuffer>& GetFramebuffers();

			void Destroy(Device& device);
		private:
			std::vector<vk::Framebuffer> m_framebuffers;
		};
	}
}

