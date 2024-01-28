#pragma once

#include "Device.h"
#include "Swapchain.h"
#include "RenderPass.h"
#include "PipelineCache.h"
#include "tools\File.h"
#include "tools\dxc\HLSLCompiler.h"
#include "Vertex.h"

#include <string>

#include <vulkan\vulkan.h>

namespace IRun {
	namespace Vk {
		class GraphicsPipeline {
		public:
			GraphicsPipeline(const std::string& vertShaderFilename, const std::string& fragShaderfilename, Device& device, Swapchain& swapchain, RenderPass& renderPass, GraphicsPipeline* basePipeline, PipelineCache& pipelineCache);

			inline const VkPipeline Get() const { return m_graphicsPipeline; }

			void Destroy(const Device& device);
		private:
			VkPipeline m_graphicsPipeline;
			VkPipelineLayout m_graphicsPipelineLayout;

			VkShaderModule CreateShaderModules(const uint32_t* spirvByteCode, size_t codeSize, const Device& device);
		};
	}
}

