#include "Renderer.h"


namespace IRun {
	namespace Vulkan {


		Renderer::Renderer(Window& window, const std::string& vertFileName, const std::string& fragFileName, bool vSync)
			: m_window{ window },
			m_vSync{ false }

		{
			m_instance = { window };
			m_surface = { m_instance, window };
			m_physicalDevice = { m_instance, m_surface };
			m_device = { m_physicalDevice, m_surface };
			m_swapChain = { window, m_device, m_physicalDevice, m_surface, vSync };
			GraphicsPipelineFeatures features{};
			features.polygonMode = vk::PolygonMode::eLine;
			features.lineWidth = 5.0f;
			m_graphicsPipeline = { vertFileName, fragFileName, m_device, m_swapChain, features};
			m_framebuffer = { m_device, m_swapChain, m_graphicsPipeline };


			QueueFamilyIndices indices = QueueFamilyIndices::FindQueueFamilies(m_physicalDevice.Get(), m_surface.Get());
			// We want to record to a command buffer every frame. So we want to able to reset and rerecord over it.
			m_graphicsCommandPool = { m_device, indices.graphicsFamily.value(), vk::CommandPoolCreateFlagBits::eResetCommandBuffer };
			m_graphicsCommandBuffers = {};
			m_graphicsCommandBuffers.resize(m_MAX_FRAMES_IN_FLIGHT);
			// We want to be able to directly submit to a queue
			for (CommandBuffer& graphicsCommandBuffer : m_graphicsCommandBuffers)
				graphicsCommandBuffer = { m_device, m_graphicsCommandPool, vk::CommandBufferLevel::ePrimary };

			m_imageAvailableSemaphores = {};
			m_imageAvailableSemaphores.resize(m_MAX_FRAMES_IN_FLIGHT);
			m_renderFinshedSemaphores = {};
			m_renderFinshedSemaphores.resize(m_MAX_FRAMES_IN_FLIGHT);
			m_inFlightFences = {};
			m_inFlightFences.resize(m_MAX_FRAMES_IN_FLIGHT);

			for (Semaphore& imageAvailableSemaphore : m_imageAvailableSemaphores)
				imageAvailableSemaphore = { m_device, (vk::SemaphoreCreateFlagBits)0 };
			for (Semaphore& renderFinishedSemaphore : m_renderFinshedSemaphores)
				renderFinishedSemaphore = { m_device, (vk::SemaphoreCreateFlagBits)0 };
			for (Fence& inFlightFences : m_inFlightFences)
				inFlightFences = { m_device, vk::FenceCreateFlagBits::eSignaled };

			m_vertexBuffer = { m_device, m_physicalDevice, m_vertices };

		}

		void Renderer::Destroy() {
			// Have to wait for all asynchronous commands to finish
			m_device.Get().waitIdle();

			for (Semaphore& imageAvailableSemaphore : m_imageAvailableSemaphores)
				imageAvailableSemaphore.Destroy(m_device);
			for (Semaphore& renderFinishedSemaphore : m_renderFinshedSemaphores)
				renderFinishedSemaphore.Destroy(m_device);
			for (Fence& inFlightFences : m_inFlightFences)
				inFlightFences.Destroy(m_device);
			m_graphicsCommandPool.Destroy(m_device);
			m_framebuffer.Destroy(m_device);
			m_graphicsPipeline.Destroy(m_device);
			m_swapChain.Destroy(m_device);
			m_device.Destroy();
			m_surface.Destroy(m_instance);
			m_instance.Destroy();
		}

		void Renderer::Draw() {
			m_inFlightFences[m_currentFrame].Wait(m_device);

			uint32_t imageIndex;
			// Disable timeout with a very long timout
			vk::Result imageAqcuireResult = m_device.Get().acquireNextImageKHR(m_swapChain.Get(), UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame].Get(), nullptr, &imageIndex);

			// Recreate swapChain if out of date
			// If the driver does not support vk::Result::eErrorOutOfDateKHR we have to handle it manually
			if (imageAqcuireResult == vk::Result::eErrorOutOfDateKHR || m_window.WasResized()) {
				RecreateSwapChain();
				// Recreate the semaphore because it gets signaled for some reason event though acquireNextImageKHR fails
				m_imageAvailableSemaphores[m_currentFrame].Destroy(m_device);
				m_imageAvailableSemaphores[m_currentFrame] = { m_device, (vk::SemaphoreCreateFlagBits)0 };
				return;
			}
			else if (imageAqcuireResult != vk::Result::eSuccess && imageAqcuireResult != vk::Result::eSuboptimalKHR)
				I_LOG_FATAL_ERROR("Failed to acquire next swapChain image!");

			// Only reset fence if we are submiting work
			m_inFlightFences[m_currentFrame].Reset(m_device);

			m_graphicsCommandBuffers[m_currentFrame].Reset((vk::CommandBufferResetFlagBits)0);

			m_graphicsCommandBuffers[m_currentFrame].Begin((vk::CommandBufferUsageFlagBits)0, nullptr);


			vk::RenderPassBeginInfo renderPassBeginInfo{};
			renderPassBeginInfo.renderPass = m_graphicsPipeline.GetRenderPass();
			// Attach framebuffer image to attachments in the render pass
			renderPassBeginInfo.framebuffer = m_framebuffer.GetFramebuffers()[imageIndex];
			// The next two parameters define the size of the render area. The render area defines where shader loads and stores will take place. The pixels outside this region will have undefined values. It should match the size of the attachments for best performance.
			renderPassBeginInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
			renderPassBeginInfo.renderArea.extent = m_swapChain.GetSwapChainExtent2D();

			// vk::ClearValue::ClearValue() -> vk::ClearColorValue::ClearColorValue() -> float float32[4]
			vk::ClearValue clearColour{};
			clearColour.color = { 0.0f, 0.0f, 0.0f, 1.0f };
			// When the framebuffer is cleared (vk::AttachmentLoadOp::eClear specified in the colour attachement) the framebuffer will be cleared to this colour.
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearColour;

			// vk::SubpassContents::eInline: The render pass commands will be embedded in the primary command buffer itselfand no secondary command buffers will be executed.
			// vk::SubpassContents::eSecondaryCommandBuffers : The render pass commands will be executed from secondary command buffers.
			m_graphicsCommandBuffers[m_currentFrame].Get().beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
			// Specify what type of pipeline we are using (graphics, compute, raytrace)
			m_graphicsCommandBuffers[m_currentFrame].Get().bindPipeline(vk::PipelineBindPoint::eGraphics, m_graphicsPipeline.Get());


			// Set viewport and scissor because we defined them as dynamic

			vk::Viewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = m_swapChain.GetSwapChainExtent2D().width;
			viewport.height = m_swapChain.GetSwapChainExtent2D().height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			m_graphicsCommandBuffers[m_currentFrame].Get().setViewport(0, viewport);

			vk::Rect2D scissor{};
			scissor.offset = vk::Offset2D{ 0, 0 };
			scissor.extent = m_swapChain.GetSwapChainExtent2D();
			m_graphicsCommandBuffers[m_currentFrame].Get().setScissor(0, scissor);


			std::array<vk::Buffer, 1> vertexBuffers = { m_vertexBuffer.Get() };
			std::array<vk::DeviceSize, 1> vertexBufferOffsets = { 0 };
			m_graphicsCommandBuffers[m_currentFrame].Get().bindVertexBuffers(0, 1, vertexBuffers.data(), vertexBufferOffsets.data());

			m_graphicsCommandBuffers[m_currentFrame].Get().draw(3, 1, 0, 0);

			m_graphicsCommandBuffers[m_currentFrame].Get().endRenderPass();

			m_graphicsCommandBuffers[m_currentFrame].End();

			vk::SubmitInfo submitInfo{};
			// Next three params specify that we want to wait to write colours to our image
			// until the semaphore is signaled
			submitInfo.pWaitSemaphores = &m_imageAvailableSemaphores[m_currentFrame].Get();
			submitInfo.waitSemaphoreCount = 1;
			std::array<vk::PipelineStageFlags, 1> waitStages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
			submitInfo.pWaitDstStageMask = waitStages.data();
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_graphicsCommandBuffers[m_currentFrame].Get();
			// Signal this semaphore when done rendering
			submitInfo.pSignalSemaphores = &m_renderFinshedSemaphores[m_currentFrame].Get();
			submitInfo.signalSemaphoreCount = 1;

			// Fence will get signaled when the command buffer finishes execution
			m_device.GetGraphicsQueue().submit(submitInfo, m_inFlightFences[m_currentFrame].Get());

			vk::PresentInfoKHR presentInfo{};
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = &m_renderFinshedSemaphores[m_currentFrame].Get();
			presentInfo.pSwapchains = &m_swapChain.Get();
			presentInfo.swapchainCount = 1;
			presentInfo.pImageIndices = &imageIndex;

			vk::Result presentResult = m_device.GetPresentQueue().presentKHR(presentInfo);

			// Recreate swapChain if out of date or suboptimal
			// If the driver does not support vk::Result::eErrorOutOfDateKHR we have to handle it manually
			if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR || m_window.WasResized()) {
				RecreateSwapChain();
			}
			else if (presentResult != vk::Result::eSuccess)
				I_LOG_FATAL_ERROR("Failed to present swap chain image!");

			
			// Gets the index of the next frame in flight
			m_currentFrame = (m_currentFrame + 1) % m_MAX_FRAMES_IN_FLIGHT;
		}

		void Renderer::RecreateSwapChain()
		{
			// Check if the window is minimized
			int width = 0, height = 0;
			glfwGetFramebufferSize(m_window.GetNativeHandle(), &width, &height);
			while (width == 0 || height == 0) {
				// Wait till the next event then check if the window is not minimized
				glfwGetFramebufferSize(m_window.GetNativeHandle(), &width, &height);
				glfwWaitEvents();
			}

			m_device.Get().waitIdle();

			// Implement oldSwapChain later
			m_swapChain.Destroy(m_device);
			m_swapChain.Create(m_window, m_device, m_physicalDevice, m_surface, m_vSync, false);
			m_framebuffer.Destroy(m_device);
			m_framebuffer = { m_device, m_swapChain, m_graphicsPipeline };

			m_window.SetResized(false);
		}

	}
}