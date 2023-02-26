#pragma once

#include "ShaderModule.h"
#include "Device.h"
#include "SwapChain.h"

namespace IRun {
	namespace Vulkan {
		class GraphicsPipeline {
		public:
			GraphicsPipeline(const std::string& vertFileName, const std::string& fragFileName, Device& device, SwapChain& swapChain);

			vk::Pipeline& Get();
			vk::RenderPass& GetRenderPass();

			void Destroy(Device& device);
		private:
			void CreateRenderPass(Device& device, SwapChain& swapChain);

			vk::Pipeline m_graphicsPipeline;
			vk::PipelineLayout m_pipelineLayout;
			vk::RenderPass m_renderPass;
		};
	}
}
