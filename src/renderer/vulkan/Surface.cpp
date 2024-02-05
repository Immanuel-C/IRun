#include "Surface.h"

namespace IRun {
	namespace Vk {
		Surface::Surface(IWindow::Window& window, Instance& instance) {
			VK_CHECK(IWindow::Vk::CreateSurface(window, instance.Get(), m_surface), "Failed to create Vulkan window surface!");
			I_DEBUG_LOG_TRACE("Created Vulkan surface: 0x%p", m_surface);
		}
		void Surface::Destroy(const Instance& instance) {
			I_DEBUG_LOG_TRACE("Destroyed Vulkan surface: 0x%p", m_surface);
			vkDestroySurfaceKHR(instance.Get(), m_surface, nullptr);
		}
	}
}