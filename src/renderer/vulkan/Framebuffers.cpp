#include "Framebuffers.h"


namespace IRun {
	namespace Vk {

		Framebuffers::Framebuffers(Swapchain& swapchain, RenderPass& renderPass, Device& device) {
			m_framebuffers.resize(swapchain.GetSwapchainImages().size());


			for (int i = 0; i < m_framebuffers.size(); i++) {
				// Only have a colour attachment (for now)
				std::array<VkImageView, 1> attachments{
					swapchain.GetSwapchainImages()[i].view,
				};

				VkFramebufferCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				createInfo.renderPass = renderPass.Get();
				createInfo.attachmentCount = (uint32_t)attachments.size();
				// Index of array matches indices of array VkRenderPassCreateInfo::pAttachments
				createInfo.pAttachments = attachments.data();
				createInfo.width = swapchain.GetChosenSwapchainDetails().first.width;
				createInfo.height = swapchain.GetChosenSwapchainDetails().first.height;
				createInfo.layers = 1;

				VK_CHECK(vkCreateFramebuffer(device.Get().first, &createInfo, nullptr, &m_framebuffers[i]), "Failed to create Vulkan framebuffer! Abort!");
				I_DEBUG_LOG_TRACE("Created Vulkan framebuffer: 0x%p", m_framebuffers[i]);
			}
		}

		void Framebuffers::Destroy(Device& device) {
			for (const VkFramebuffer& framebuffer : m_framebuffers) {
				I_DEBUG_LOG_TRACE("Destroyed Vulkan framebuffer: 0x%p", framebuffer);
				vkDestroyFramebuffer(device.Get().first, framebuffer, nullptr);
			}
				
		}

	}
}
