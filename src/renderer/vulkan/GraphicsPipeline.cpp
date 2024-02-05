#include "GraphicsPipeline.h"

#include "tools/Timer.h"

namespace IRun {
	namespace Vk {
		GraphicsPipeline::GraphicsPipeline(const std::string& vertShaderFilename, const std::string& fragShaderFilename, ShaderLanguage lang, Device& device, Swapchain& swapchain, RenderPass& renderPass, GraphicsPipeline* basePipeline, PipelineCache& pipelineCache) {

			VkShaderModule vertShaderModule{};
			VkShaderModule fragShaderModule{};

			
			switch (lang)
			{
			case IRun::ShaderLanguage::HLSL: {
				std::array<std::vector<char>, 2> shaderBin = Tools::DXC::CompileHLSLtoSPRIV(vertShaderFilename, fragShaderFilename);
				vertShaderModule = CreateShaderModules((const uint32_t*)shaderBin[0].data(), shaderBin[0].size(), device);
				fragShaderModule = CreateShaderModules((const uint32_t*)shaderBin[1].data(), shaderBin[1].size(), device);
				break;
			}
			case IRun::ShaderLanguage::Spirv: {
				std::string vertShaderCode = Tools::ReadFile(vertShaderFilename, Tools::IoFlags::Binary);
				std::string fragShaderCode = Tools::ReadFile(fragShaderFilename, Tools::IoFlags::Binary);
				vertShaderModule = CreateShaderModules((uint32_t*)vertShaderCode.c_str(), vertShaderCode.size(), device);
				fragShaderModule = CreateShaderModules((uint32_t*)fragShaderCode.c_str(), vertShaderCode.size(), device);
				break;
			}
			default:
				I_DEBUG_LOG_FATAL_ERROR("GraphicsPipeline::GraphicsPipeline(const std::string&, const std::string&, ShaderLanguage, Device&, Swapchain&, RenderPass&, GraphicsPipeline*, PipelineCache&): param ShaderLangauge is not a valid language type.");
				break;
			}

			
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

			VkVertexInputBindingDescription bindingDescription{};
			// Can bind multiple streams of data, this defines which one.
			bindingDescription.binding = 0;
			// Size of each vertex object.
			bindingDescription.stride = sizeof(Vertex); 
			// For instancing. How to move between data after each vertex.
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

			// position attribute
			// Binds this attribute to VkVertexInputBindingDescription::binding = 0
			attributeDescriptions[0].binding = 0;
			// location in shader.
			attributeDescriptions[0].location = 0;
			// Size and type of data 32 bit float
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			// Location of attribute in each stride
			attributeDescriptions[0].offset = offsetof(Vertex, position);

			// uv attribute
			// Binds this attribute to VkVertexInputBindingDescription::binding = 0
			attributeDescriptions[1].binding = 0;
			// location in shader.
			attributeDescriptions[1].location = 1;
			// Size and type of data 32 bit float
			attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
			// Location of attribute in each stride
			attributeDescriptions[1].offset = offsetof(Vertex, uv);


			VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
			vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
			// Basically does the same thing as glVertexAttribPointer. Handles data spacing/stride info.
			vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputCreateInfo.vertexAttributeDescriptionCount = (uint32_t)attributeDescriptions.size();
			// Basically does the same thing as glVertexAttribPointer. Format and where to bind it to.
			vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

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
			graphicsPipelineCreateInfo.renderPass = renderPass.Get();
			// Index defined in VkRenderPassCreateInfo::pSubpasses
			graphicsPipelineCreateInfo.subpass = 0;

			VkPipeline base = nullptr;

			if (basePipeline != nullptr)
				base = basePipeline->Get();

			// Base this pipline off another one
			graphicsPipelineCreateInfo.basePipelineHandle = base;
			// Base this pipeline off of another pipeline in an array of pipeline create infos (not used since we only have one pipline)
			graphicsPipelineCreateInfo.basePipelineIndex = 0;

			VK_CHECK(vkCreateGraphicsPipelines(device.Get().first, pipelineCache.Get().second, 1, &graphicsPipelineCreateInfo, nullptr, &m_graphicsPipeline), "Failed to create graphics pipeline!");
			I_DEBUG_LOG_TRACE("Created Vulkan graphics pipeline: 0x%p", m_graphicsPipeline);


			I_DEBUG_LOG_TRACE("Destroyed Vulkan shader module: 0x%p", vertShaderModule);
			I_DEBUG_LOG_TRACE("Destroyed Vulkan shader module: 0x%p", fragShaderModule);
			vkDestroyShaderModule(device.Get().first, vertShaderModule, nullptr);
			vkDestroyShaderModule(device.Get().first, fragShaderModule, nullptr);
		}

		void GraphicsPipeline::Destroy(const Device& device) {
			I_DEBUG_LOG_TRACE("Destroyed Vulkan graphics pipeline: 0x%p", m_graphicsPipeline);
			vkDestroyPipeline(device.Get().first, m_graphicsPipeline, nullptr);
			I_DEBUG_LOG_TRACE("Destroyed Vulkan pipeline layout: 0x%p", m_graphicsPipelineLayout);
			vkDestroyPipelineLayout(device.Get().first, m_graphicsPipelineLayout, nullptr);
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

	}
}