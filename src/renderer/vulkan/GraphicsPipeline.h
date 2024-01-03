#pragma once

#include "Device.h"
#include "Swapchain.h"
#include "tools\File.h"
#include "tools\dxc\HLSLCompiler.h"

#include <string>

#include <vulkan\vulkan.h>

namespace IRun {
	namespace Vk {
		class GraphicsPipeline {
		public:
			GraphicsPipeline(const std::string& vertShaderFilename, const std::string& fragShaderfilename, const Device& device, const Swapchain& swapchain);

			void Destroy(const Device& device);
		private:
			VkPipeline m_graphicsPipeline;
			VkPipelineLayout m_graphicsPipelineLayout;

			VkRenderPass m_renderPass;

			VkShaderModule CreateShaderModules(const uint32_t* spirvByteCode, size_t codeSize, const Device& device);
			void CreateRenderpass(const Device& device, const Swapchain& swapchain);
		};
	}
}

