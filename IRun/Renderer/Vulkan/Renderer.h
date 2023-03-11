#pragma once

#include "Window.h"
#include "Instance.h"
#include "Surface.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "Framebuffer.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "SyncObjects.h"
#include "VertexBuffer.h"

namespace IRun {
	namespace Vulkan {
		class Renderer {
		public:
			Renderer(Window& window, const std::string& vertFileName, const std::string& fragFileName, bool vSync);

			void Destroy();

			void Draw();
		private:
			Window& m_window;
			Instance m_instance;
			Surface m_surface;
			PhysicalDevice m_physicalDevice;
			Device m_device;
			SwapChain m_swapChain;
			GraphicsPipeline m_graphicsPipeline;
			Framebuffer m_framebuffer;
			CommandPool m_graphicsCommandPool;
			std::vector<CommandBuffer> m_graphicsCommandBuffers;

			std::vector<Semaphore> m_imageAvailableSemaphores;
			std::vector<Semaphore> m_renderFinshedSemaphores;
			std::vector<Fence> m_inFlightFences;

			// I chose two becasuse we don't want the CPU to get too far ahead of the GPU
			// 3 frames in flight could cause latency frames.
			const uint32_t m_MAX_FRAMES_IN_FLIGHT = 2;
			uint32_t m_currentFrame = 0;

			bool m_vSync = false;

			const std::array<Vec2, 3> m_vertices = {
				Vec2{glm::vec2{0.0f, -0.5f}, glm::vec3{1.0f, 0.0f, 0.0f}},
				Vec2{glm::vec2{0.5f,  0.5f}, glm::vec3{0.0f, 1.0f, 0.0f}},
				Vec2{glm::vec2{-0.5f, 0.5f}, glm::vec3{0.0f, 0.0f, 1.0f}}
			};

			VertexBuffer m_vertexBuffer;

			void RecreateSwapChain();
		};
	}
}

