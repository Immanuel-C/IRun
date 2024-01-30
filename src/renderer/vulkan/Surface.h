#pragma once

#include <vulkan\vulkan.h>
#include <IWindow.h>
#include <IWindowVK.h>

#include "Instance.h"
#include "Check.h"

namespace IRun {
	namespace Vk {
		class Surface {
		public:
			Surface() = default;
			/// <summary>
			/// Create the surface.
			/// </summary>
			/// <param name="window">Window to be rendered to.</param>
			/// <param name="instance">Vulkan instance this surface is created under.</param>
			Surface(IWindow::Window& window, const Instance& instance);
			/// <summary>
			/// Destroy the surface.
			/// </summary>
			/// <param name="instance"></param>
			void Destroy(const Instance& instance);
			/// <summary>
			/// Get the native Vulkan surface.
			/// </summary>
			/// <returns>native Vulkan surface.</returns>
			VkSurfaceKHR Get() const { return m_surface; }
		private:
			VkSurfaceKHR m_surface = nullptr;
		};
	}
}

