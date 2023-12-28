#pragma once

#include <IWindow.h>
#include <IWindowVK.h>
#include <ILog.h>
#include <vulkan\vulkan.h>

#include "tools\VkCheck.h"

namespace IRun {
	namespace VK {
		class Instance {
		public:
			Instance() = default;
			Instance(IWindow::Window& window);
			/// <summary>
			/// Destroy's VkInstance and VkDebugUtilsMessenger (if in debug).
			/// </summary>
			void Destroy();
			/// <summary>
			/// Get the native Vulkan instance.
			/// </summary>
			/// <returns>native Vulkan instance.</returns>
			inline VkInstance Get() const { return m_instance; }
		private:
			VkInstance m_instance;
			VkDebugUtilsMessengerEXT m_debugMessenger;

			void CreateInstance(IWindow::Window& window);
			void CreateDebugMessenger();



			const std::vector<const char*> m_validationLayers = {
				"VK_LAYER_KHRONOS_validation"
			};

			#ifdef DEBUG
			bool m_enableValidationLayers = true;
			#elif NDEBUG
			bool m_enableValidationLayers = false;
			#endif

			bool CheckValidationLayerSupport();
		};
	}
}

