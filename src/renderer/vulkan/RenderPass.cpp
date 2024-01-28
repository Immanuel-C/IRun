#include "RenderPass.h"

namespace IRun {
	namespace Vk {
		RenderPass::RenderPass(Device& device, Swapchain& swapchain) {
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

		void RenderPass::Destroy(Device& device) {
			I_DEBUG_LOG_TRACE("Destroyed Vulkan render pass: 0x%p", m_renderPass);
			vkDestroyRenderPass(device.Get().first, m_renderPass, nullptr);
		}
	}
}
