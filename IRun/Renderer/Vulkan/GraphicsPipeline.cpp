#include "GraphicsPipeline.h"


namespace IRun {
	namespace Vulkan {
		GraphicsPipeline::GraphicsPipeline(const std::string& vertFileName, const std::string& fragFileName, Device& device, SwapChain& swapChain) {
			ShaderModule vertShaderModule{ vertFileName, ShaderType::Vertex, device };
			ShaderModule fragShaderModule{ fragFileName, ShaderType::Fragment, device };

			vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
			vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
			vertShaderStageInfo.module = vertShaderModule.Get();
			// Name of the main function
			vertShaderStageInfo.pName = "main";

			vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
			fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
			fragShaderStageInfo.module = fragShaderModule.Get();
			fragShaderStageInfo.pName = "main";


			std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {
				vertShaderStageInfo,
				fragShaderStageInfo
			};

			// For passing in vertex data
			vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
			// Spacing between data and whether the data is per-vertex or per-instance
			vertexInputInfo.vertexBindingDescriptionCount = 0;
			vertexInputInfo.pVertexBindingDescriptions = nullptr;
			// Type of attributes passed to the vertex shader, which binding to load them from and at which offset
			vertexInputInfo.vertexAttributeDescriptionCount = 0;
			vertexInputInfo.pVertexAttributeDescriptions = nullptr;

			vertShaderModule.Destroy(device);
			fragShaderModule.Destroy(device);

			vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
			// VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
			// VK_PRIMITIVE_TOPOLOGY_LINE_LIST : line from every 2 vertices without reuse
			// VK_PRIMITIVE_TOPOLOGY_LINE_STRIP : the end vertex of every line is used as start vertex for the next line
			// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : triangle from every 3 vertices without reuse
			// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP : the second and third vertex of every triangle are used as first two vertices of the next triangle
			// No features required
			inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
			inputAssemblyInfo.primitiveRestartEnable = false;

			std::array<vk::DynamicState, 2> dynamicStates = {
				vk::DynamicState::eViewport,
				vk::DynamicState::eScissor
			};

			// Can change this about the fixed pipeline during runtime 
			vk::PipelineDynamicStateCreateInfo dynamicState{};
			dynamicState.dynamicStateCount = dynamicStates.size();
			dynamicState.pDynamicStates = dynamicStates.data();

			// Viewport describes the region of the framebuffer that the output will be rendered to.
			vk::Viewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)swapChain.GetSwapChainExtent2D().width;
			viewport.height = (float)swapChain.GetSwapChainExtent2D().height;
			// Range of depth values to use for framebuffer. Must be in range of 0.0f - 1.0f
			// Should stick to standard values
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			// Like a filter anything out side of it won't be rendered.
			// Basically the opposite of the viewport.
			// viewport takes priority
			vk::Rect2D scissor{};
			// Offset from 0, 0 (Bottom right of the framebuffer)
			scissor.offset = vk::Offset2D{ 0, 0 };
			scissor.extent = swapChain.GetSwapChainExtent2D();

			vk::PipelineViewportStateCreateInfo viewportState{};
			viewportState.viewportCount = 1;
			viewportState.pViewports = &viewport;
			viewportState.scissorCount = 1;
			viewportState.pScissors = &scissor;

			vk::PipelineRasterizationStateCreateInfo rasterizerCreateInfo{};
			// Fragments beyond near and far planes are clamped to them as opposed to discarding them.
			// Useful in shadow maps.
			rasterizerCreateInfo.depthClampEnable = false;
			// Primitves are discareded... For some reason
			// Might be useful to for benchmarking and trying to improve preformence in the fragment shader
			// If turned off the fragment shader wont run.
			rasterizerCreateInfo.rasterizerDiscardEnable = false;
			// FILL, LINE (a.k.a wireframe), POINT (for each vertex)
			// Similer to the input assemblers topology except less options
			// Feature required for anything other than fill.
			rasterizerCreateInfo.polygonMode = vk::PolygonMode::eFill;
			// WideLine feature required for anything other than 1.0f
			rasterizerCreateInfo.lineWidth = 1.0f;
			// cull the front of the object or the back
			rasterizerCreateInfo.cullMode = vk::CullModeFlagBits::eBack;
			// Tells which way we specified the order of the vertices
			rasterizerCreateInfo.frontFace = vk::FrontFace::eCounterClockwise;
			// The rasterizer can alter the depth values by adding a constant value or biasing them bast on a fragments slope?
			// Do not understand this
			// Used in shadow mapping
			rasterizerCreateInfo.depthBiasEnable = false;
			rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
			rasterizerCreateInfo.depthBiasClamp = 0.0f;
			rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;
		}
	}
}