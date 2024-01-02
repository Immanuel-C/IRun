#include "Surface.h"

namespace IRun {
	namespace Vk {
		Surface::Surface(IWindow::Window& window, const Instance& instance) {
			VK_CHECK(IWindow::Vk::CreateSurface(instance.Get(), m_surface, window), "Failed to create Vulkan window surface!");
			I_DEBUG_LOG_TRACE("Created Vulkan surface: 0x%p", m_surface);
		}
		void Surface::Destroy(const Instance& instance) {
			I_DEBUG_LOG_TRACE("Destroyed Vulkan surface: 0x%p", m_surface);
			vkDestroySurfaceKHR(instance.Get(), m_surface, nullptr);
		}
	}
}