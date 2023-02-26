#include "Surface.h"


namespace IRun {
	namespace Vulkan {
		Surface::Surface(Instance& instance, Window& window) {

			VkSurfaceKHR temp = nullptr;
			VK_CHECK((vk::Result)glfwCreateWindowSurface(instance.Get(), window.GetNativeHandle(), nullptr, &temp), "Failed to create Vulkan surface (GLFW)");
			m_surface = temp;

			I_DEBUG_LOG_INFO("SurfaceKHR: %p", (VkSurfaceKHR)m_surface);
		}

		void Surface::Destroy(Instance& instance) {
			I_DEBUG_LOG_INFO("SurfaceKHR Deleted: %p", (VkSurfaceKHR)m_surface);
			instance.Get().destroySurfaceKHR(m_surface);
		}



		vk::SurfaceKHR& Surface::Get() {
			return m_surface;
		}
	}
}