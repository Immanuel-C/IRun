#pragma once

#include "Instance.h"
#include "Window.h"
#include <glfw3.h>
#include <IWindowVK.h>

namespace IRun {
	namespace Vulkan {
		class Surface {
		public:
			Surface(Instance& instance, Window& window);

			const vk::SurfaceKHR& Get();

			operator vk::SurfaceKHR() { return m_surface; }
		private:
			vk::SurfaceKHR m_surface;
		};
	}
}