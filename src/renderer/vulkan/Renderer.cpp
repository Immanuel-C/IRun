#include "Renderer.h"

namespace IRun {
	namespace Vk {
		Renderer::Renderer(IWindow::Window& window, bool vSync) :
			m_window{ window }, 
			m_instance { window }, 
			m_surface{ window, m_instance },
			m_device{ m_instance, m_surface },
			m_swapchain{ vSync, m_window, m_surface, m_device },
			m_renderPass{ m_device, m_swapchain },
			m_framebuffers{ m_swapchain, m_renderPass, m_device }
		{
		}

		void Renderer::Destroy()
		{
			m_swapchain.Destroy(m_device);
			m_device.Destroy();
			m_surface.Destroy(m_instance);
			m_instance.Destroy();
		}
	}
}