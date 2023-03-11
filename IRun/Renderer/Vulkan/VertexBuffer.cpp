#include "VertexBuffer.h"


namespace IRun {
	namespace Vulkan {
		void VertexBuffer::Create(Device& device, PhysicalDevice& physicalDevice, Vec2* buffer, size_t bufferSize) {
			vk::BufferCreateInfo bufferInfo{};
			bufferInfo.size = sizeof(Vec2) * bufferSize;
			// Can specify multiple usages
			bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
			// Just like swap chain images buffers cam be used in multiple queues
			// but we are only using this in the graphics queue
			bufferInfo.sharingMode = vk::SharingMode::eExclusive;

			m_buffer = device.Get().createBuffer(bufferInfo);

			I_DEBUG_LOG_INFO("Buffer: %p", m_buffer);

			vk::MemoryRequirements memRequirments;
			// size: The size of the required amount of memory in bytes, may differ from bufferInfo.size.
			// alignment : The offset in bytes where the buffer begins in the allocated region of memory, depends on bufferInfo.usageand bufferInfo.flags.
			// memoryTypeBits : Bit field of the memory types that are suitable for the buffer.
			memRequirments = device.Get().getBufferMemoryRequirements(m_buffer);

			vk::MemoryAllocateInfo allocInfo{};
			// Aligned size could be actual buffer size
			allocInfo.allocationSize = memRequirments.size;
			allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirments.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

			m_deviceMemory = device.Get().allocateMemory(allocInfo);
			I_DEBUG_LOG_INFO("Device Memory: %p, Buffer: %p", m_deviceMemory, m_buffer);


			// 3rd param for memory pools (offset)
			device.Get().bindBufferMemory(m_buffer, m_deviceMemory, 0);

			// 2 param for memory pools (offset)
			void* data = device.Get().mapMemory(m_deviceMemory, (vk::DeviceSize)0, bufferSize);
			::memcpy(data, buffer, bufferSize);
			device.Get().unmapMemory(m_deviceMemory);
		}

		vk::Buffer& VertexBuffer::Get() { return m_buffer; }

		vk::DeviceMemory VertexBuffer::GetDeviceMemory() { return m_deviceMemory; }

		uint32_t VertexBuffer::FindMemoryType(PhysicalDevice& device, uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
			// Query the available types of memory
			// We dont care about the heaps right now
			vk::PhysicalDeviceMemoryProperties memProperties;
			memProperties = device.Get().getMemoryProperties();

			for (int i = 0; i < memProperties.memoryTypeCount; i++) {
				// Check for memory required memory type is available
				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
					return i;
			}

			I_LOG_FATAL_ERROR("Failed to find suitable physical device memory type for buffer: %p", m_buffer);

			return 0;
		}

		void VertexBuffer::Destroy(Device& device) {
			I_DEBUG_LOG_INFO("Destroyed Buffer: %p", m_buffer);
			I_DEBUG_LOG_INFO("Buffer: %p, Deallocated Device Memory: %p", m_buffer, m_deviceMemory);
			device.Get().destroyBuffer(m_buffer);
			device.Get().freeMemory(m_deviceMemory);
		}
	}
}