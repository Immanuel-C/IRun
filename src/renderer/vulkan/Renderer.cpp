#include "Renderer.h"

namespace IRun {
	namespace Vk {
		static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

		Renderer::Renderer(IWindow::Window& window, ECS::Helper& helper, bool vSync) :
			m_window{ window },
			m_helper{ helper },
			m_currentFrame{ 0 },
			m_vSync{ vSync },
			m_framebufferResized{ false },
			m_oldFramebufferSize{ window.GetFramebufferSize() }
		{ 
			if (debugMode) timer.Start();
			m_instance = Instance{ window };
			m_surface = Surface{ window, m_instance };
			m_device = Device{ m_instance, m_surface };
			m_swapchain = Swapchain{ vSync, window, m_surface, m_device, nullptr };
			m_renderPass = RenderPass{ m_device, m_swapchain };

			m_pipelineCache = PipelineCache{};
			ErrorCode err = m_pipelineCache.RetrieveCache("shaders/cache/PipelineCache.bin", m_device);

			// For first run or corrupted cache. Create empty cache for data to be stored in.
			if ((int64_t)(err & ErrorCode::IoError) || (int64_t)(err & ErrorCode::Corrupt)) {
				m_pipelineCache.CreateCache(m_device, nullptr, 0);
			}

			m_basePipeline = GraphicsPipeline{ 
				"shaders/Vert.hlsl", 
				"shaders/Frag.hlsl", 
				ShaderLanguage::HLSL, 
				m_device, m_swapchain, 
				m_renderPass, 
				nullptr, 
				m_pipelineCache 
			};
			
			m_framebuffers = Framebuffers{ m_swapchain, m_renderPass, m_device };
			m_graphicsCommandPool = CommandPool{ m_device, m_device.GetQueueFamilies().graphicsFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT };
			m_transferCommandPool = CommandPool{ m_device, m_device.GetQueueFamilies().transferFamily };

			for (int i = 0; i < m_framebuffers.Get().size(); i++)
				m_commandBuffers.emplace_back(m_graphicsCommandPool.CreateBuffer(m_device, IRun::Vk::CommandBufferLevel::Primary));

			m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
			for (Sync<Semaphore>& semaphore : m_imageAvailableSemaphores)
				semaphore = Sync<Semaphore>{ m_device };

			m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
			for (Sync<Semaphore>& semaphore : m_renderFinishedSemaphores)
				semaphore = Sync<Semaphore>{ m_device };

			m_drawFences.resize(MAX_FRAMES_IN_FLIGHT);
			for (Sync<Fence>& fence : m_drawFences)
				fence = Sync<Fence>{ m_device, VK_FENCE_CREATE_SIGNALED_BIT };

			m_renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;


			VkClearValue clearColor{};
			clearColor.color = { { 0.0f, 0.0f, 0.0f } };
			m_renderPassBeginInfo.clearValueCount = 1;
			m_renderPassBeginInfo.pClearValues = &clearColor;

			if (debugMode) {
				double time = timer.Stop();
				I_DEBUG_LOG_INFO("Time to create IRun::Vk::Renderer: %fms", time);
			}
		}

		void Renderer::AddEntity(ECS::Entity entity) {
			if (debugMode) timer.Start();

			m_entities.insert({ entity, entity });

			auto [vertexData, indexData, shaders] = m_helper.get<ECS::VertexData, ECS::IndexData, ECS::Shader>(entity);

			DeviceLocalBuffer<Vertex> vertexDataBuffer{
				m_device,
				m_transferCommandPool,
				vertexData.data.data(),
				vertexData.data.size(),
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			};

			m_vertexDataBuffers.insert({ entity, vertexDataBuffer });
			
			DeviceLocalBuffer<uint32_t> indexDataBuffer {
				m_device,
				m_transferCommandPool,
				indexData.data.data(),
				indexData.data.size(),
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			};

			m_indexDataBuffers.insert({ entity, indexDataBuffer });

			if (m_graphicsPipelines.count(shaders) > 0) { 
				if (debugMode) {
					double time = timer.Stop();
					I_DEBUG_LOG_INFO("Time to add an entity to IRun::Vk::Renderer: %fms", time);
				}
				return; 
			}

			GraphicsPipeline graphicsPipeline { 
				shaders.vertexFilename,
				shaders.fragmentFilename,
				shaders.language,
				m_device, 
				m_swapchain, 
				m_renderPass, 
				&m_basePipeline, 
				m_pipelineCache 
			};

			m_graphicsPipelines.insert({ shaders, graphicsPipeline });

			if (debugMode) {
				double time = timer.Stop();
				I_DEBUG_LOG_INFO("Time to add an entity to IRun::Vk::Renderer: %fms", time);
			}
		}

		void Renderer::RemoveEntity(ECS::Entity entity) {
			if (debugMode) timer.Start();

			auto [shaders] = m_helper.get<ECS::Shader>(entity);

			DeviceLocalBuffer<Vertex>& vertexDataBuffer = m_vertexDataBuffers.at(entity);
			vertexDataBuffer.Destroy(m_device);
			m_vertexDataBuffers.erase(entity);

			DeviceLocalBuffer<uint32_t>& indexDataBuffer = m_indexDataBuffers.at(entity);
			indexDataBuffer.Destroy(m_device);
			m_indexDataBuffers.erase(entity);

			if (m_graphicsPipelines.count(shaders) == 1) {
				GraphicsPipeline& graphicsPipeline = m_graphicsPipelines.at(shaders);
				graphicsPipeline.Destroy(m_device);
				m_graphicsPipelines.erase(shaders);
			}

			m_entities.erase(entity);

			if (debugMode) {
				double time = timer.Stop();
				I_DEBUG_LOG_INFO("Time to remove an entity to IRun::Vk::Renderer: %fms", time);
			}
		}

		void Renderer::ClearColor(Math::Color color) {
			VkClearValue clearColor{};
			clearColor.color = { { ((float)color.r / 255.0f), (float)(color.g / 255.0f), (float)(color.b / 255.0f), 1.0f } };
			m_renderPassBeginInfo.clearValueCount = 1;
			m_renderPassBeginInfo.pClearValues = &clearColor;
		}

		void Renderer::Draw() {
			IWindow::IVector2 framebufferSize = m_window.GetFramebufferSize();

			if (m_oldFramebufferSize.x != framebufferSize.x || m_oldFramebufferSize.y != framebufferSize.y) {
				m_framebufferResized = true;
				m_oldFramebufferSize = framebufferSize;
			}

			for (const std::pair<ECS::Entity, ECS::Entity>& en : m_entities) {
				ECS::Entity entity = en.first;
				auto [shaders] = m_helper.get<ECS::Shader>(entity);

				std::array<Fence, 1> fencesToWaitFor = {
					m_drawFences[m_currentFrame].Get()
				};

				vkWaitForFences(m_device.Get().first, (uint32_t)fencesToWaitFor.size(), fencesToWaitFor.data(), true, UINT64_MAX);

				uint32_t imageIndex;
				VkResult res = vkAcquireNextImageKHR(m_device.Get().first, m_swapchain.Get(), UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame].Get(), nullptr, &imageIndex);

				// Recreate swapchain
				if (res == VK_ERROR_OUT_OF_DATE_KHR || m_framebufferResized) {
					RecreateSwapchain();
					return;
				}
				else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
					I_LOG_FATAL_ERROR("Failed to acquire swapchain image at index: %u", imageIndex);
				}
					

				vkResetFences(m_device.Get().first, (uint32_t)fencesToWaitFor.size(), fencesToWaitFor.data());

				m_renderPassBeginInfo.renderPass = m_renderPass.Get();
				m_renderPassBeginInfo.renderArea.offset = { 0, 0 };
				m_renderPassBeginInfo.renderArea.extent = m_swapchain.GetChosenSwapchainDetails().first;
				m_renderPassBeginInfo.framebuffer = m_framebuffers[imageIndex];

				m_graphicsCommandPool.BeginRecordingCommands(m_device, m_commandBuffers[imageIndex]);

					vkCmdBeginRenderPass(m_graphicsCommandPool[m_commandBuffers[imageIndex]], &m_renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

						vkCmdBindPipeline(m_graphicsCommandPool[m_commandBuffers[imageIndex]], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipelines.at(shaders).Get());

						VkViewport viewport{};
						viewport.x = 0.0f;
						viewport.y = 0.0f;
						viewport.width = (float)m_swapchain.GetChosenSwapchainDetails().first.width;
						viewport.height = (float)m_swapchain.GetChosenSwapchainDetails().first.height;
						viewport.minDepth = 0.0f;
						viewport.maxDepth = 1.0f;

						vkCmdSetViewport(m_graphicsCommandPool[m_commandBuffers[imageIndex]], 0, 1, &viewport);

						VkRect2D scissor{};
						scissor.offset = { 0, 0 };
						scissor.extent = { m_swapchain.GetChosenSwapchainDetails().first.width, m_swapchain.GetChosenSwapchainDetails().first.height };

						vkCmdSetScissor(m_graphicsCommandPool[m_commandBuffers[imageIndex]], 0, 1, &scissor);

						DeviceLocalBuffer<Vertex> vertexDataBuffer = m_vertexDataBuffers.at(entity);
						DeviceLocalBuffer<uint32_t> indexDataBuffer = m_indexDataBuffers.at(entity);

						std::array<VkBuffer, 1> vertexBuffers = {
							vertexDataBuffer.Get().Get(),
						};

						std::array<VkDeviceSize, 1> offsets = {
							0
						};

						vkCmdBindVertexBuffers(m_graphicsCommandPool[m_commandBuffers[imageIndex]], 0, (uint32_t)vertexBuffers.size(), vertexBuffers.data(), offsets.data());

						vkCmdBindIndexBuffer(m_graphicsCommandPool[m_commandBuffers[imageIndex]], indexDataBuffer.Get().Get(), 0, VK_INDEX_TYPE_UINT32);

						vkCmdDrawIndexed(m_graphicsCommandPool[m_commandBuffers[imageIndex]], (uint32_t)indexDataBuffer.Get().GetSize(), 1, 0, 0, 0);

					vkCmdEndRenderPass(m_graphicsCommandPool[m_commandBuffers[imageIndex]]);

				m_graphicsCommandPool.EndRecordingCommands(m_commandBuffers[imageIndex]);

				VkSubmitInfo submitInfo{};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

				std::array<VkSemaphore, 1> submitWaitSemaphores = {
					m_imageAvailableSemaphores[m_currentFrame].Get()
				};

				submitInfo.waitSemaphoreCount = (uint32_t)submitWaitSemaphores.size();
				submitInfo.pWaitSemaphores = submitWaitSemaphores.data();

				VkPipelineStageFlags waitStages[] = {
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				};
				// 1:1 with pWaitSemaphores
				submitInfo.pWaitDstStageMask = waitStages;

				std::array<VkCommandBuffer, 1> submitCommandBuffers = { 
					m_graphicsCommandPool[m_commandBuffers[imageIndex]] 
				};

				submitInfo.commandBufferCount = (uint32_t)submitCommandBuffers.size();
				submitInfo.pCommandBuffers = submitCommandBuffers.data();

				std::array<VkSemaphore, 1> submitSignalSemaphores = { 
					m_renderFinishedSemaphores[m_currentFrame].Get() 
				};

				submitInfo.signalSemaphoreCount = (uint32_t)submitSignalSemaphores.size();
				submitInfo.pSignalSemaphores = submitSignalSemaphores.data();

				VK_CHECK(vkQueueSubmit(m_device.GetQueues().at(IRun::Vk::QueueType::Graphics), 1, &submitInfo, fencesToWaitFor[0]), "Failed to sumbit semaphore and command buffer info to graphics queue!");


				VkPresentInfoKHR presentInfo{};
				presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
				presentInfo.waitSemaphoreCount = (uint32_t)submitSignalSemaphores.size();
				presentInfo.pWaitSemaphores = submitSignalSemaphores.data();

				std::array<VkSwapchainKHR, 1> swapchainsToPresentTo = {
					m_swapchain.Get()
				};

				presentInfo.swapchainCount = (uint32_t)swapchainsToPresentTo.size();
				presentInfo.pSwapchains = swapchainsToPresentTo.data();

				presentInfo.pImageIndices = &imageIndex;

				res = vkQueuePresentKHR(m_device.GetQueues().at(IRun::Vk::QueueType::Presentation), &presentInfo);

				if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
					RecreateSwapchain();
				else
					VK_CHECK(res, "Failed to present Vulkan swapchain image!");


				m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
			}
		}

		void Renderer::Destroy()
		{
			m_pipelineCache.SaveCache("shaders/cache/PipelineCache.bin", m_device);

			vkQueueWaitIdle(m_device.GetQueues().at(QueueType::Graphics));

			for (auto& [entity, vertexBuffer] : m_vertexDataBuffers)
				vertexBuffer.Destroy(m_device);

			for (auto& [entity, indexBuffer] : m_indexDataBuffers)
				indexBuffer.Destroy(m_device);

			for (Sync<Semaphore>& semaphore : m_imageAvailableSemaphores)
				semaphore.Destroy(m_device);

			for (Sync<Semaphore>& semaphore : m_renderFinishedSemaphores)
				semaphore.Destroy(m_device);

			for (Sync<Fence>& fence : m_drawFences)
				fence.Destroy(m_device);

			m_transferCommandPool.Destroy(m_device);
			m_graphicsCommandPool.Destroy(m_device);
			m_framebuffers.Destroy(m_device);

			for (auto& [shader, graphicsPipeline]: m_graphicsPipelines) 
				graphicsPipeline.Destroy(m_device);
			
			m_basePipeline.Destroy(m_device);
			m_pipelineCache.Destroy(m_device);
			m_renderPass.Destroy(m_device);
			m_swapchain.Destroy(m_device, false);
			m_device.Destroy();
			m_surface.Destroy(m_instance);
			m_instance.Destroy();
		}

		void Renderer::RecreateSwapchain() {
			m_framebufferResized = false;
			IWindow::IVector2 size = m_window.GetWindowSize();

			while (size.x == 0 || size.y == 0) {
				// Implement something like glfwWaitEvents() in IWindow.
				std::this_thread::sleep_for(std::chrono::milliseconds{ 15 });

				m_window.Update();
				size = m_window.GetWindowSize();
			}

			vkDeviceWaitIdle(m_device.Get().first);

			m_device.ResetSwapchainDetails(m_surface);

			for (Sync<Semaphore>& semaphore : m_imageAvailableSemaphores)
				semaphore.Destroy(m_device);

			for (Sync<Semaphore>& semaphore : m_renderFinishedSemaphores)
				semaphore.Destroy(m_device);

			for (Sync<Fence>& fence : m_drawFences)
				fence.Destroy(m_device);

			m_oldSwapchain = m_swapchain;
			m_swapchain = Swapchain{ m_vSync, m_window, m_surface, m_device, &m_oldSwapchain };
			m_oldSwapchain.Destroy(m_device, false);
			m_framebuffers.Destroy(m_device);
			m_framebuffers = Framebuffers{ m_swapchain, m_renderPass, m_device };

			m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
			for (Sync<Semaphore>& semaphore : m_imageAvailableSemaphores)
				semaphore = Sync<Semaphore>{ m_device };

			m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
			for (Sync<Semaphore>& semaphore : m_renderFinishedSemaphores)
				semaphore = Sync<Semaphore>{ m_device };

			m_drawFences.resize(MAX_FRAMES_IN_FLIGHT);
			for (Sync<Fence>& fence : m_drawFences)
				fence = Sync<Fence>{ m_device, VK_FENCE_CREATE_SIGNALED_BIT };
		}
	}
}