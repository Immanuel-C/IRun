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

			// Multisampling for anti-aliasing
			vk::PipelineMultisampleStateCreateInfo multiSamplingInfo{};
			multiSamplingInfo.sampleShadingEnable = false;
			multiSamplingInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
			multiSamplingInfo.minSampleShading = 1.0f; 
			multiSamplingInfo.pSampleMask = nullptr; 
			multiSamplingInfo.alphaToCoverageEnable = false; 
			multiSamplingInfo.alphaToOneEnable = false;

			// Depth and stencil testing
			// Not needed right now
			vk::PipelineDepthStencilStateCreateInfo depthStencilInfo{};

			// Describes how colours will be blended
			vk::PipelineColorBlendAttachmentState colourBlendAttachment{};
			// The channels that will be effected
			colourBlendAttachment.colorWriteMask = 
				vk::ColorComponentFlagBits::eR | 
				vk::ColorComponentFlagBits::eG | 
				vk::ColorComponentFlagBits::eB | 
				vk::ColorComponentFlagBits::eA;
			colourBlendAttachment.blendEnable = false;
			colourBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
			colourBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
			colourBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
			colourBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
			colourBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
			colourBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

			vk::PipelineColorBlendStateCreateInfo colourBlending{};
			colourBlending.logicOpEnable = false;
			colourBlending.logicOp = vk::LogicOp::eCopy;
			colourBlending.attachmentCount = 1;
			colourBlending.pAttachments = &colourBlendAttachment;


			// For uniforms
			// Will use later
			vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.setLayoutCount = 0;
			pipelineLayoutInfo.pSetLayouts = nullptr;
			pipelineLayoutInfo.pushConstantRangeCount = 0;
			pipelineLayoutInfo.pPushConstantRanges = nullptr;

			m_pipelineLayout = device.Get().createPipelineLayout(pipelineLayoutInfo);

			I_DEBUG_LOG_INFO("Pipeline Layout: %p", m_pipelineLayout);

			CreateRenderPass(device, swapChain);

			vk::GraphicsPipelineCreateInfo pipelineInfo{};
			pipelineInfo.stageCount = 2;
			// Two shader stages vertex and fragment
			pipelineInfo.pStages = shaderStages.data();
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizerCreateInfo;
			// No depth stencil
			pipelineInfo.pDepthStencilState = nullptr;
			pipelineInfo.pColorBlendState = &colourBlending;
			pipelineInfo.pDynamicState = &dynamicState;
			pipelineInfo.layout = m_pipelineLayout;
			pipelineInfo.renderPass = m_renderPass;
			pipelineInfo.subpass = 0;
			// This pipeline can derive from other pipelines
			// Have to have vk::PipelineCreateFlagBits::eDerivative in the flags for this struct
			pipelineInfo.basePipelineHandle = nullptr;
			pipelineInfo.basePipelineIndex = -1;


			vk::ResultValue<vk::Pipeline> result = device.Get().createGraphicsPipeline(nullptr, pipelineInfo);
			VK_CHECK(result.result, "Failed to create Vulkan graphics pipeline");
			m_graphicsPipeline = result.value;

			I_DEBUG_LOG_INFO("Graphics Pipeline: %p", m_graphicsPipeline);


			vertShaderModule.Destroy(device);
			fragShaderModule.Destroy(device);
		}

		vk::Pipeline& GraphicsPipeline::Get() { return m_graphicsPipeline; }

		vk::RenderPass& GraphicsPipeline::GetRenderPass() { return m_renderPass; }

		void GraphicsPipeline::Destroy(Device& device) {
			I_DEBUG_LOG_INFO("Graphics Pipeline Deleted: %p", (VkPipeline)m_graphicsPipeline);
			device.Get().destroyPipeline(m_graphicsPipeline);
			I_DEBUG_LOG_INFO("Pipeline Layout Deleted: %p", (VkPipelineLayout)m_pipelineLayout);
			device.Get().destroyPipelineLayout(m_pipelineLayout);
			I_DEBUG_LOG_INFO("Render Pass Deleted: %p", (VkRenderPass)m_renderPass);
			device.Get().destroyRenderPass(m_renderPass);
		}

		// The render pass tells Vulkan about the framebuffer attachments that will be used whil rendering.
		// We need to specify how many depth and colour buffers there will be, how many samples to use for each of them
		// and how their contents should be handled throughout the rendering operations
		void GraphicsPipeline::CreateRenderPass(Device& device, SwapChain& swapChain) {
			// In out case we will have 1 colour buffer attachment representing one image from our buffer
			vk::AttachmentDescription colourAttachment{};
			colourAttachment.format = swapChain.GetSwapChainImageFormat();
			// For multisampling
			colourAttachment.samples = vk::SampleCountFlagBits::e1;
			// VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents of the attachment
			// VK_ATTACHMENT_LOAD_OP_CLEAR : Clear the values to a constant at the start
			// VK_ATTACHMENT_LOAD_OP_DONT_CARE : Existing contents are undefined; we don't care about them
			// Clear framebuffer to black for new frame
			colourAttachment.loadOp = vk::AttachmentLoadOp::eClear;
			// VK_ATTACHMENT_STORE_OP_STORE: Rendered contents will be stored in memory and can be read later
			// VK_ATTACHMENT_STORE_OP_DONT_CARE: Contents of the framebuffer will be undefined after the rendering operation
			// We want to see the triangle on the screen so we store it (on to the screen?)
			colourAttachment.storeOp = vk::AttachmentStoreOp::eStore;
			// For stencil (we don't care)
			colourAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
			colourAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
			// Common layouts
			// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color attachment
			// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: Images to be presented in the swap chain
			// VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL : Images to be used as destination for a memory copy operation
			// The layout of the pixles of an image can change based on what you're trying to do with the image.
			// We don't care what the last image layout was. Layout before render pass.
			// The caveat is that the image may not be saved but we don't care about the either.
			colourAttachment.initialLayout = vk::ImageLayout::eUndefined;
			// We are trying to present the image. Layout after render pass 
			colourAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;


			// A single render pass consists of multipls subpasses. Subpasses are subsequent render operations
			// that depend on the contents of framebuffers in previous passes.
			// Useful for post-processing i guess.

			// Each subpass references 
			vk::AttachmentReference colourAttachmentRef{};
			// Index of the AttachmentDescription array. If we have no array the index is 0.
			// The array/pointer is passed to the renderpass create info (vk::RenderPassCreateInfo).
			// In the fragment shader we reference this index as the out colour (layout(location = 0) out vec4 outColour).
			colourAttachmentRef.attachment = 0;
			// Transition the attachment to this layout (eColorAttachmentOptimal)
			// We want this to be a colour buffer
			colourAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

			// These other attachments can be referenced in the subpass
			// pInputAttachments: Attachments that are read from a shader
			// pResolveAttachments : Attachments used for multisampling color attachments
			// pDepthStencilAttachment : Attachment for depthand stencil data
			// pPreserveAttachments : Attachments that are not used by this subpass, but for which the data must be preserved
			vk::SubpassDescription subpass{};
			// Tells Vulkan this a graphics subpass
			subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colourAttachmentRef;

			vk::RenderPassCreateInfo renderPassInfo{};
			renderPassInfo.attachmentCount = 1;
			// This is the array/pointer that is referenced in the vk::AttachmentReference('s)
			// Will be attached to the vk::ImageView at the same index in the framebuffer
			renderPassInfo.pAttachments = &colourAttachment;
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpass;

			m_renderPass = device.Get().createRenderPass(renderPassInfo);
			I_DEBUG_LOG_INFO("Render Pass: %p", m_renderPass);
		}
	}
}