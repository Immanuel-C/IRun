#pragma once

#include <vulkan/vulkan.hpp>
#include <memory>

#include "Device.h"
#include "Vec2.h"

namespace IRun {
	namespace Vulkan {
		class VertexBuffer {
		public:
			VertexBuffer() = default;
			template<size_t size>
			VertexBuffer(Device& device, PhysicalDevice& physicalDevice, std::array<Vec2, size> buffer) { Create(device, physicalDevice, buffer.data(), buffer.size()); }
			void Create(Device& device, PhysicalDevice& physicalDevice, Vec2* buffer, size_t bufferSize);

			vk::Buffer& Get();
			vk::DeviceMemory GetDeviceMemory();

			uint32_t FindMemoryType(PhysicalDevice& device, uint32_t typeFilter, vk::MemoryPropertyFlags properties);

			void Destroy(Device& device);
		private:
			vk::Buffer m_buffer;
			vk::DeviceMemory m_deviceMemory;
		};
	}
}