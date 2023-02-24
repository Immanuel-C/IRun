#pragma once

#include <vulkan/vulkan.hpp>
#include "Instance.h"
#include "Window.h"
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

namespace IRun {
	namespace Vulkan {
		class Surface {
		public:
			Surface(Instance& instance, Window& window);

			vk::SurfaceKHR& Get();

			operator vk::SurfaceKHR() { return m_surface; }
			operator vk::SurfaceKHR&() { return m_surface; }

		private:
			vk::SurfaceKHR m_surface;
		};
	}
}