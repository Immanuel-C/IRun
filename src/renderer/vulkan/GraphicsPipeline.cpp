#include "GraphicsPipeline.h"

namespace IRun {
	namespace Vk {
		GraphicsPipeline::GraphicsPipeline(const std::string& vertShaderFilename, const std::string& fragShaderFilename, const Device& device, const Swapchain& swapchain) {
			CreateRenderpass(device, swapchain);

			std::array<CComPtr<IDxcBlob>, 2> shaderBin = Tools::DXC::CompileHLSLtoSPRIV(vertShaderFilename, fragShaderFilename);

			VkShaderModule vertShaderModule = CreateShaderModules((const uint32_t*)shaderBin[0]->GetBufferPointer(), shaderBin[0]->GetBufferSize(), device);
			VkShaderModule fragShaderModule = CreateShaderModules((const uint32_t*)shaderBin[1]->GetBufferPointer(), shaderBin[1]->GetBufferSize(), device);

			VkPipelineShaderStageCreateInfo vertShaderCreateInfo{};
			vertShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderCreateInfo.module = vertShaderModule;
			vertShaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderCreateInfo.pName = "main";

			VkPipelineShaderStageCreateInfo fragShaderCreateInfo{};
			fragShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderCreateInfo.module = fragShaderModule;
			fragShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderCreateInfo.pName = "main";

			std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{
				vertShaderCreateInfo, fragShaderCreateInfo
			};

			// TODO
			VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
			vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
			// Basically does the same thing as glVertexAttribPointer. Handles data spacing/stride info.
			vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;
			vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
			// Basically does the same thing as glVertexAttribPointer. Format and where to bind it to.
			vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;

			VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
			inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			// GL_TRIANGLES
			inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			// Only used for strip topology, not used.
			inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

			// Transform image
			VkViewport viewport{};
			// Top left corner
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			// Bottom right corner
			viewport.width = (float)swapchain.GetChosenSwapchainDetails().first.width;
			viewport.height = (float)swapchain.GetChosenSwapchainDetails().first.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			// Crop image
			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = { swapchain.GetChosenSwapchainDetails().first.width, swapchain.GetChosenSwapchainDetails().first.height };

			VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
			viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportStateCreateInfo.viewportCount = 1;
			viewportStateCreateInfo.pViewports = &viewport;
			viewportStateCreateInfo.scissorCount = 1;
			viewportStateCreateInfo.pScissors = &scissor;

			std::array<VkDynamicState, 2> dynamicStates{
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
			};

			VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
			dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicStateCreateInfo.dynamicStateCount = (uint32_t)dynamicStates.size();
			dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

			VkPipelineRasterizationStateCreateInfo rasterizerStateCreateInfo{};
			rasterizerStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			// don't clip off things past the far plane and clamp it to plane. Required device feature
			rasterizerStateCreateInfo.depthBiasClamp = VK_FALSE;
			// Don't use. Not used in graphics. 
			rasterizerStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
			// Fill the whole shape in. Required device feature
			rasterizerStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
			// Requires device feature
			rasterizerStateCreateInfo.lineWidth = 1.0f;
			rasterizerStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizerStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
			// Whether to add depth bias to fragments for shadow acne. Requires device feature
			rasterizerStateCreateInfo.depthBiasEnable = VK_FALSE;

			// Disabled for now.
			VkPipelineMultisampleStateCreateInfo multisamplingStateCreateInfo{};
			multisamplingStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisamplingStateCreateInfo.sampleShadingEnable = VK_FALSE;
			multisamplingStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

			VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
			colorBlendAttachmentState.blendEnable = VK_TRUE;
			// which colours to blend
			colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			// Blending uses equation: (srcColorBlendFactor * new colour) colorBlendOp (dstColorBlendFactor * old colour)
			colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
			// Dont blend alphas
			colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;


			VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
			colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
			colorBlendStateCreateInfo.attachmentCount = 1;
			colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
			// RGBA
			// Used for VK_BLEND_FACTOR_X_CONSTANT
			colorBlendStateCreateInfo.blendConstants;

			// TODO: Pipline layout
			VkPipelineLayoutCreateInfo piplineLayoutCreateInfo{};
			piplineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			piplineLayoutCreateInfo.setLayoutCount = 0;
			piplineLayoutCreateInfo.pSetLayouts = nullptr;
			piplineLayoutCreateInfo.pushConstantRangeCount = 0;
			piplineLayoutCreateInfo.pPushConstantRanges = nullptr;

			VK_CHECK(vkCreatePipelineLayout(device.Get().first, &piplineLayoutCreateInfo, nullptr, &m_graphicsPipelineLayout), "Failed to create Vulkan graphics pipline layout!");

			I_DEBUG_LOG_TRACE("Created Vulkan pipeline layout: 0x%p", m_graphicsPipelineLayout);

			// TODO: set up depth stencil testing
			// VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};

			VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
			graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			// Shader stages
			graphicsPipelineCreateInfo.stageCount = (uint32_t)shaderStages.size();
			graphicsPipelineCreateInfo.pStages = shaderStages.data();
			graphicsPipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
			graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
			graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
			graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
			graphicsPipelineCreateInfo.pRasterizationState = &rasterizerStateCreateInfo;
			graphicsPipelineCreateInfo.pMultisampleState = &multisamplingStateCreateInfo;
			graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
			graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
			graphicsPipelineCreateInfo.layout = m_graphicsPipelineLayout;
			graphicsPipelineCreateInfo.renderPass = m_renderPass;
			// Index defined in VkRenderPassCreateInfo::pSubpasses
			graphicsPipelineCreateInfo.subpass = 0;

			// Base this pipline off another one
			graphicsPipelineCreateInfo.basePipelineHandle = nullptr;
			// Base this pipeline off of another pipeline in an array of pipeline create infos (not used since we only have one pipline)
			graphicsPipelineCreateInfo.basePipelineIndex = -1;

			VK_CHECK(vkCreateGraphicsPipelines(device.Get().first, nullptr, 1, &graphicsPipelineCreateInfo, nullptr, &m_graphicsPipeline), "Failed to create graphics pipeline!");
			I_DEBUG_LOG_TRACE("Created Vulkan graphics pipeline: 0x%p", m_graphicsPipeline);


			I_DEBUG_LOG_TRACE("Destroyed Vulkan shader module: 0x%p", vertShaderModule);
			I_DEBUG_LOG_TRACE("Destroyed Vulkan shader module: 0x%p", fragShaderModule);
			vkDestroyShaderModule(device.Get().first, vertShaderModule, nullptr);
			vkDestroyShaderModule(device.Get().first, fragShaderModule, nullptr);
			shaderBin[0].Release();
			shaderBin[1].Release();
		}

		void GraphicsPipeline::Destroy(const Device& device) {
			I_DEBUG_LOG_TRACE("Destroyed Vulkan graphics pipeline: 0x%p", m_graphicsPipeline);
			vkDestroyPipeline(device.Get().first, m_graphicsPipeline, nullptr);
			I_DEBUG_LOG_TRACE("Destroyed Vulkan pipeline layout: 0x%p", m_graphicsPipelineLayout);
			vkDestroyPipelineLayout(device.Get().first, m_graphicsPipelineLayout, nullptr);
			I_DEBUG_LOG_TRACE("Destroyed Vulkan render pass: 0x%p", m_renderPass);
			vkDestroyRenderPass(device.Get().first, m_renderPass, nullptr);
		}

		VkShaderModule GraphicsPipeline::CreateShaderModules(const uint32_t* spirvByteCode, size_t codeSize, const Device& device)
		{
			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = codeSize;
			createInfo.pCode = spirvByteCode;

			VkShaderModule shaderModule;
			VK_CHECK(vkCreateShaderModule(device.Get().first, &createInfo, nullptr, &shaderModule), "Failed to create Vulkan shader module! Abort!");

			I_DEBUG_LOG_TRACE("Created Vulkan shader module: 0x%p", shaderModule);

			return shaderModule;
		}


		void GraphicsPipeline::CreateRenderpass(const Device& device, const Swapchain& swapchain) {
			// Colour attachment of render pass
			VkAttachmentDescription colourAttachment{};
			colourAttachment.format = swapchain.GetChosenSwapchainDetails().second.format;
			// Same as VkPipelineMultisampleStateCreateInfo::rasterizationSamples
			colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			// Operation when it first loads in the attachment. VK_ATTACHMENT_LOAD_OP_CLEAR = glClear(GL_COLOR_BUFFER_BIT)
			// Clear the attachment so we can right the new frame to it
			colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			// There will be also a intermediate attachment during the subpass that is defined in VkAttachmentReference::layout.
			// Operation after we are done rendering. Store the data so we can draw it to the screen.
			colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			// Depth stuff
			colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			// Framebuffer data will be stored as an image. Images can also be given different layouts to give best preformence during certain operations.
			// We don't know what layout it is before render pass starts
			colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			// Layout after render pass (to change to)
			colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			// Attachment reference uses an an attachment index in the attachment list passed to renderPassCreateInfo
			VkAttachmentReference colourAttachmentReference{};
			// attachment index in VkRenderPassCreateInfo::pAttachments
			colourAttachmentReference.attachment = 0;
			// Intermediate attachment
			colourAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpass{};
			// Only use in a graphis pipeline.
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colourAttachmentReference;

			// Need to determine when layout transitions occur using subpass dependencies
			std::array<VkSubpassDependency, 2> subpassDepedencies{};

			// Conversion from VK_IMAGE_LAYOUT_UNDEFINED -> VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			// What subpass we are coming from. We are coming from no subpass/an external one.
			subpassDepedencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			// The conversion has to happen after the end of the external subpass.
			subpassDepedencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			// The conversion has to also happen after we read the data to put in an image.
			subpassDepedencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			// Index from VkRenderPassCreateInfo::pSubpasses
			subpassDepedencies[0].dstSubpass = 0;
			// The conversion has to happen before outputing the colour to an image.
			subpassDepedencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			// The conversion has to also happen before we write the colour attachment to the image.
			subpassDepedencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			// Conversion from VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL -> VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			subpassDepedencies[1].srcSubpass = 0;
			subpassDepedencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDepedencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			 
			subpassDepedencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			subpassDepedencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDepedencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;


			VkRenderPassCreateInfo renderPassCreateInfo{};
			renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassCreateInfo.attachmentCount = 1;
			renderPassCreateInfo.pAttachments = &colourAttachment;
			renderPassCreateInfo.subpassCount = 1;
			renderPassCreateInfo.pSubpasses = &subpass;
			renderPassCreateInfo.dependencyCount = (uint32_t)subpassDepedencies.size();
			renderPassCreateInfo.pDependencies = subpassDepedencies.data();

			VK_CHECK(vkCreateRenderPass(device.Get().first, &renderPassCreateInfo, nullptr, &m_renderPass), "Failed to create Vulkan render pass!");
			I_DEBUG_LOG_TRACE("Create Vulkan render pass: 0x%p", m_renderPass);
		}
	}
}