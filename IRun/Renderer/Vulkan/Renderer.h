#pragma once

#include "Window.h"
#include "Instance.h"
#include "Surface.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"
#include "Framebuffer.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "SyncObjects.h"

namespace IRun {
	namespace Vulkan {
		class Renderer {
		public:
			Renderer(Window& window, const std::string& vertFileName, const std::string& fragFileName, bool vSync);

			void Destroy();

			void Draw();
		private:
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
		};
	}
}

