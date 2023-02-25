#pragma once

#include <glfw3.h>
#include <vulkan/vulkan.hpp>

#include "Window.h"
#include "VkCheck.h"

namespace IRun {
	namespace Vulkan {
		class Instance
		{
		public:
			Instance(Window& window);

			void Destroy();

			vk::Instance Get() const;

			operator vk::Instance() {
				return m_instance;
			}

		private:
			vk::Instance m_instance;
		};
	}
}
