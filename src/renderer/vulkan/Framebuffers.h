#pragma once

#include "Swapchain.h"
#include "Device.h"
#include "RenderPass.h"

namespace IRun {
	namespace Vk {
		/// <summary>
		/// A wrapper around VkFramebuffer.
		/// </summary>
		class Framebuffers {
		public:
			Framebuffers() = default;
			/// <summary>
			/// Creates the VkFramebuffer.
			/// </summary>
			/// <param name="swapchain">A valid IRun::Vk::Swapchain.</param>
			/// <param name="renderPass">A valid IRun::Vk::RenderPass.</param>
			/// <param name="device">A valid IRun::Vk::Device.</param>
			Framebuffers(Swapchain& swapchain, RenderPass& renderPass, Device& device);
			/// <summary>
			/// Destroys the VkFramebuffers.
			/// </summary>
			/// <param name="device">A valid IRun::Vk::Device.</param>
			void Destroy(Device& device);
			/// <returns>Handle to the VkFramebuffers</returns>
			const inline std::vector<VkFramebuffer>& Get() const { return m_framebuffers; }

			VkFramebuffer operator[](int index) {
				return m_framebuffers[index];
			}
		private:
			std::vector<VkFramebuffer> m_framebuffers;
		};
	}
}

