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
#include "DeviceLocalBuffer.h"

#include "ecs/Components.h"

#include "tools/Timer.h"

#include <unordered_map>
#include <thread>

namespace IRun {
	namespace Vk {
		/// <summary>
		/// Create renderer using the Vulkan graphics API.
		/// </summary>
		class Renderer {
		public:
			Renderer() = default;
			/// <summary>
			/// Init renderer.
			/// </summary>
			/// <param name="window">A valid IWindow::Window.</param>
			/// <param name="helper">A valid IRun::ECS::Helper.</param>
			/// <param name="vSync">If set to true the framerate of the application will be locked to the monitors refresh rate. Fixes screen tearing but may cause input lag.</param>
			Renderer(IWindow::Window& window, ECS::Helper& helper, bool vSync);
			/// <summary>
			/// Add an entity that is to be rendered.
			/// </summary>
			/// <param name="entity">
			/// An IRun::ECS::Entity that is created by the IRun::ECS::Helper passed into the constructor.
			/// This entity must have components IRun::ECS::VertexData and IRun::ECS::Shader.
			/// </param>
			void AddEntity(ECS::Entity entity);
			/// <summary>
			/// Remove entity from render list.
			/// </summary>
			/// <param name="entity">Entity to be removed.</param>
			void RemoveEntity(ECS::Entity entity);
			/// <summary>
			/// What colour to clear the background of the window to.
			/// </summary>
			/// <param name="color">Must be a valid IRun::Math::Color.</param>
			void ClearColor(Math::Color color);

			void VSync(bool vSync);

			/// <summary>
			/// render all entities.
			/// </summary>
			void Draw();
			/// <summary>
			/// Destroy the renderer. You must destroy the window and IRun::ECS::Helper after destroying the renderer.
			/// </summary>
			void Destroy();
		private:
			IWindow::Window* m_window;
			ECS::Helper* m_helper;

			Instance m_instance;
			Surface m_surface;
			Device m_device;
			Swapchain m_oldSwapchain;
			Swapchain m_swapchain;
			RenderPass m_renderPass;
			PipelineCache m_pipelineCache;
			Framebuffers m_framebuffers;
			CommandPool m_graphicsCommandPool;
			CommandPool m_transferCommandPool;
			GraphicsPipeline m_basePipeline;

			std::vector<Sync<Semaphore>> m_imageAvailableSemaphores{};
			std::vector<Sync<Semaphore>> m_renderFinishedSemaphores{};
			std::vector<Sync<Fence>> m_drawFences{};

			std::vector<CommandBuffer> m_commandBuffers;

			std::unordered_map<ECS::Shader, GraphicsPipeline, ECS::Shader::HashFn> m_graphicsPipelines;
			std::unordered_map<ECS::Entity, DeviceLocalBuffer<Vertex>> m_vertexDataBuffers;
			std::unordered_map<ECS::Entity, DeviceLocalBuffer<uint32_t>> m_indexDataBuffers;

			VkRenderPassBeginInfo m_renderPassBeginInfo{};

			uint32_t m_currentFrame;

			std::unordered_map<ECS::Entity, ECS::Entity> m_entities;

			Tools::Timer<Tools::Milliseconds> timer{};

			bool m_vSync;

			void RecreateSwapchain();

			bool m_framebufferResized;
			IWindow::Vector2<int32_t> m_oldFramebufferSize;

#ifdef DEBUG
			bool debugMode = true;
#else
			bool debugMode = false;
#endif
		};
	}
}
