#pragma once

#include "ShaderModule.h"
#include "Device.h"
#include "SwapChain.h"
#include "Vec2.h"

namespace IRun {
	namespace Vulkan {
		// The class will check if the feature is available before use.
		struct GraphicsPipelineFeatures {
			// The vk::PipelineRasterizationStateCreateInfo::polygonMode is set to this. Default mode is vk::PolygonMode::eFill.
			vk::PolygonMode polygonMode = vk::PolygonMode::eFill;
			// the vk::PipelineRasterizationStateCreateInfo::lineWidth is set to this. Default width is 1.0f.
			float lineWidth = 1.0f;
		};

		class GraphicsPipeline {
		public:
			GraphicsPipeline() = default;
			GraphicsPipeline(const std::string& vertFileName, const std::string& fragFileName, Device& device, SwapChain& swapChain, GraphicsPipelineFeatures features = {});

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
