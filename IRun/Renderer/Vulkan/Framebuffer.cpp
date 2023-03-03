#include "Framebuffer.h"

namespace IRun {
	namespace Vulkan {
		Framebuffer::Framebuffer(Device& device, SwapChain& swapChain, GraphicsPipeline& graphicsPipeline) : m_framebuffers{} {
			// The image that we have to use for the attachment depends on which image the swap chain returns when we retrieve one for presentation.
			// That means that we have to create a framebuffer for all of the images in the swap chain and use the one that corresponds to the retrieved image at drawing time.
			m_framebuffers.resize(swapChain.GetSwapChainImageViews().size());

			for (int i = 0; i < swapChain.GetSwapChainImageViews().size(); i++) {
				std::array<vk::ImageView, 1> attachments = {
					swapChain.GetSwapChainImageViews()[i]
				};

				vk::FramebufferCreateInfo framebufferInfo{};
				framebufferInfo.renderPass = graphicsPipeline.GetRenderPass();
				framebufferInfo.attachmentCount = attachments.size();
				// The attachments we referenced in the render pass will be bound to the image view at the same index
				framebufferInfo.pAttachments = attachments.data();
				framebufferInfo.width = swapChain.GetSwapChainExtent2D().width;
				framebufferInfo.height = swapChain.GetSwapChainExtent2D().height;
				framebufferInfo.layers = 1;

				m_framebuffers[i] = device.Get().createFramebuffer(framebufferInfo);
				I_DEBUG_LOG_INFO("Framebuffer #%i: %p", i, (VkFramebuffer)m_framebuffers[i]);
			}
		}


		void Framebuffer::Destroy(Device& device) {
			for (int i = 0; i < m_framebuffers.size(); i++) {
				I_DEBUG_LOG_INFO("Framebuffer #%i Deleted: %p", i, (VkFramebuffer)m_framebuffers[i]);
				device.Get().destroyFramebuffer(m_framebuffers[i]);
			}
		}

		std::vector<vk::Framebuffer>& Framebuffer::GetFramebuffers() {
			return m_framebuffers;
		}
	}
}