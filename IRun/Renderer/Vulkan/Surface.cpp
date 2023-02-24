#include "Surface.h"


namespace IRun {
	namespace Vulkan {
		Surface::Surface(Instance& instance, Window& window) {

			VkSurfaceKHR temp = nullptr;
			VK_CHECK((vk::Result)glfwCreateWindowSurface(instance.Get(), window.GetNativeHandle(), nullptr, &temp), "Failed to create Vulkan surface (GLFW)");
			m_surface = temp;

			I_DEBUG_LOG_INFO("Vulkan Surface: %p", (VkSurfaceKHR)m_surface);
		}

		vk::SurfaceKHR& Surface::Get() {
			return m_surface;
		}
	}
}