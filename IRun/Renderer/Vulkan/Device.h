#pragma once

#include "PhysicalDevice.h"
#include <set>

namespace IRun {
	namespace Vulkan {
		class Device {
		public:
			Device(PhysicalDevice& physicalDevice, Surface& surface);

			void Destroy();

			vk::Device& Get();
			vk::Queue& GetGraphicsQueue();
			vk::Queue& GetPresentQueue();

			operator vk::Device() {
				return m_device;
			}

		private:
			vk::Device m_device;
			vk::Queue m_graphicsQueue;
			vk::Queue m_presentQueue;
		};
	}
}