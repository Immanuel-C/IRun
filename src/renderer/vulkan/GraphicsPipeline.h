#pragma once

#include "Device.h"
#include "Swapchain.h"
#include "RenderPass.h"
#include "PipelineCache.h"
#include "tools\File.h"
#include "tools\dxc\HLSLCompiler.h"
#include "../ShaderLang.h"
#include "../Vertex.h"

#include <string>

#include <vulkan\vulkan.h>

namespace IRun {
	namespace Vk {
		/// <summary>
		/// A wrapper for VkPipeline.
		/// </summary>
		class GraphicsPipeline {
		public:
			GraphicsPipeline() = default;
			/// <summary>
			/// Creates a graphics pipeline.
			/// </summary>
			/// <param name="vertShaderFilename">Path to either a valid hlsl or spirv vertex shader.</param>
			/// <param name="fragShaderfilename">Path to either a valid hlsl or spirv fragment shader.</param>
			/// <param name="lang">Language that the shader is written in. Must be wither hlsl or spirv.</param>
			/// <param name="device">A valid IRun::Vk::Device.</param>
			/// <param name="swapchain">A valid IRun::Vk::Swapchain.</param>
			/// <param name="renderPass">A valid IRun::Vk::RenderPass</param>
			/// <param name="basePipeline">Can be nullptr, the base pipeline that this pipeline is based on.</param>
			/// <param name="pipelineCache">A valid IRun::Vk::PipelineCache.</param>
			GraphicsPipeline(const std::string& vertShaderFilename, const std::string& fragShaderfilename, ShaderLanguage lang, Device& device, Swapchain& swapchain, RenderPass& renderPass, GraphicsPipeline* basePipeline, PipelineCache& pipelineCache);
			/// <returns>Get the VkPipeline handle.</returns>
			inline const VkPipeline Get() const { return m_graphicsPipeline; }
			/// <summary>
			/// Destroy the VkPipeline.
			/// </summary>
			/// <param name="device">A valid IRun::Vk::Device.</param>
			void Destroy(const Device& device);
		private:
			VkPipeline m_graphicsPipeline;
			VkPipelineLayout m_graphicsPipelineLayout;

			VkShaderModule CreateShaderModules(const uint32_t* spirvByteCode, size_t codeSize, const Device& device);
		};
	}
}

