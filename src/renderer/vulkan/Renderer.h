#pragma once

#include <IWindow.h>

#include "Instance.h"
#include "Surface.h"
#include "Device.h"
#include "Swapchain.h"
#include "GraphicsPipeline.h"
#include "RenderPass.h"
#include "Framebuffers.h"
#include "CommandPool.h"
#include "Sync.h"

#include "ecs/Components.h"

namespace IRun {
	namespace Vk {
		class Renderer {
		public:
			Renderer() = default;
			Renderer(IWindow::Window& window, bool vSync);

			void AddEntity(const ECS::Entity& entity);

			void Draw(ECS::Entity enitity);

			void Destroy();
		private:
			Instance m_instance;
			Surface m_surface;
			Device m_device;
			Swapchain m_swapchain;
			RenderPass m_renderPass;
			Framebuffers m_framebuffers;
			CommandPool m_commandPool;
			// GraphicsPipeline m_basePipeline;
			std::vector<CommandBuffer> m_commandBuffers;
			std::vector<GraphicsPipeline> m_graphicsPipelines;
			std::vector<ECS::Entity> m_entities;

			IWindow::Window& m_window;
		};
	}
}
