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
			Surface() = default;
			Surface(Instance& instance, Window& window);
			
			void Destroy(Instance& instance);

			vk::SurfaceKHR& Get();

			operator vk::SurfaceKHR() { return m_surface; }
			operator vk::SurfaceKHR&() { return m_surface; }

		private:
			vk::SurfaceKHR m_surface;
		};
	}
}