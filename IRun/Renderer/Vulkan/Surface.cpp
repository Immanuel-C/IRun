#include "Surface.h"


namespace IRun {
	namespace Vulkan {
		Surface::Surface(Instance& instance, Window& window) {
			if (window.GetAPI()  == WindowAPI::IWindow) {
				VkSurfaceKHR temp = nullptr;
				VK_CHECK((vk::Result)IWindow::Vk::CreateSurface(instance.Get(), temp, window.GetIWindowAPIHandle()), "Failed to create Vulkan surface (IWindow)");
				m_surface = temp;
			}
			else {
				VkSurfaceKHR temp = nullptr;
				VK_CHECK((vk::Result)glfwCreateWindowSurface(instance.Get(), window.GetGLFWAPIHandle(), nullptr, &temp), "Failed to create Vulkan surface (GLFW)");
				m_surface = temp;
			}

			I_DEBUG_LOG_INFO("Vulkan Surface: %p", (VkSurfaceKHR)m_surface);
		}

		const vk::SurfaceKHR& Surface::Get() {
			return m_surface;
		}
	}
}