#pragma once

#include <vulkan\vulkan.h>

#include "Vertex.h"
#include "Device.h"

namespace IRun {
	namespace Vk {
		template<typename DataType, size_t dataSize>
		class Buffer {
		public:
			Buffer() = default;

			// VkSharingModeExclusive
			// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			Buffer(Device& device, DataType* data, VkBufferUsageFlags flags, VkSharingMode sharingMode, VkMemoryPropertyFlags propertyFlags) {
				VkBufferCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				// size in bytes
				createInfo.size = sizeof(DataType) * dataSize;
				createInfo.usage = flags;
				createInfo.sharingMode = sharingMode;

				VK_CHECK(vkCreateBuffer(device.Get().first, &createInfo, nullptr, &m_buffer), "Failed to create Vulkan buffer!");

				VkMemoryRequirements memoryRequirments{};
				vkGetBufferMemoryRequirements(device.Get().first, m_buffer, &memoryRequirments);

				VkMemoryAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo.allocationSize = memoryRequirments.size;
				allocInfo.memoryTypeIndex = FindMemoryTypeIndex(device, memoryRequirments.memoryTypeBits, propertyFlags);

				I_ASSERT_FATAL_ERROR(allocInfo.memoryTypeIndex == UINT32_MAX, "Failed to find a suitable memory type index for allocation Vulkan device memory!");

				VK_CHECK(vkAllocateMemory(device.Get().first, &allocInfo, nullptr, &m_memory), "Failed to allocate Vulkan device memory");

				// memoryOffset is for memory pools. I should add that.
				vkBindBufferMemory(device.Get().first, m_buffer, m_memory, 0);

				void* mappedData;
				// VkMemoryMapFlags is reserved should always be zero.
				vkMapMemory(device.Get().first, m_memory, 0, createInfo.size, 0, &mappedData);
				memcpy(mappedData, data, (size_t)createInfo.size);
				vkUnmapMemory(device.Get().first, m_memory);
			}

			void Destroy(Device& device) {
				vkFreeMemory(device.Get().first, m_memory, nullptr);
				vkDestroyBuffer(device.Get().first, m_buffer, nullptr);
			}

			size_t GetSize() const { return dataSize; }

			const VkBuffer Get() const { return m_buffer; }
			const VkDeviceMemory GetMemory() const { return m_memory; }

		private:
			VkBuffer m_buffer;
			VkDeviceMemory m_memory;

			uint32_t FindMemoryTypeIndex(Device& device, uint32_t allowedTypes, VkMemoryPropertyFlags propertyFlags) {
				// Props of the physical device memory
				VkPhysicalDeviceMemoryProperties memoryProperties{};
				vkGetPhysicalDeviceMemoryProperties(device.Get().second, &memoryProperties);

				for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
					// Black magic
					// Index of memory type must match bit in allowedTypes
					// Desired property flags must be available in physical memory property flags
					if ((allowedTypes & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags) {
						return i;
					}
				}

				return UINT32_MAX;
			}
		};
	}
}

