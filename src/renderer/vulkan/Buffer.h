#pragma once

#include <vulkan\vulkan.h>

#include "../Vertex.h"
#include "Device.h"
#include "tools/Flags.h"

namespace IRun {
	namespace Vk {
		/// <summary>
		/// IRun specific flags for creating buffers on the Gpu.
		/// </summary>
		enum struct BufferFlags {
			None = 0x0,
			/// <summary>
			/// Do not map the host visible data to the gpu.
			/// </summary>
			NoMap = 0x1,
			Max
		};
		CREATE_FLAGS_FROM_ENUM_STRUCT(BufferFlags, BufferFlags::Max);

		/// <summary>
		/// Wrapper for a VkBuffer. This data is stored on the Gpu.
		/// </summary>
		/// <typeparam name="DataType">Type of data to be stored.</typeparam>
		template<typename DataType>
		class Buffer {
		public:
			Buffer() = default;

			/// <summary>
			/// Init buffer.
			/// </summary>
			/// <param name="device">A valid IRun::Vk::Device.</param>
			/// <param name="data">Data that is to be stored.</param>
			/// <param name="dataSize">Size of data to be stored.</param>
			/// <param name="usageFlags">Usage of the buffer. Must be a valid VkBufferUsageFlags.</param>
			/// <param name="sharingMode">Allow sharing between queue families. Must be a valid VkSharingMode.</param>
			/// <param name="propertyFlags">Properties of the buffers. Must be a valid VkMemoryPropertyFlags.</param>
			Buffer(Device& device, DataType* data, size_t dataSize, VkBufferUsageFlags usageFlags, VkSharingMode sharingMode, VkMemoryPropertyFlags propertyFlags, BufferFlags flags = BufferFlags::None) :
				m_size{ dataSize },
				m_hostCoherent{ false }
			{
				VkBufferCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				// size in bytes
				createInfo.size = sizeof(DataType) * dataSize;
				createInfo.usage = usageFlags;
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

				if (!(int64_t)(flags & BufferFlags::NoMap)) {
					void* mappedData;
					// VkMemoryMapFlags is reserved should always be zero.
					vkMapMemory(device.Get().first, m_memory, 0, createInfo.size, 0, &mappedData);
					memcpy(mappedData, data, (size_t)createInfo.size);

					if (!(propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
						m_hostCoherent = true;

						VkMappedMemoryRange memoryRange{};
						memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
						memoryRange.offset = 0;
						memoryRange.size = VK_WHOLE_SIZE;
						memoryRange.memory = m_memory;

						VK_CHECK(vkFlushMappedMemoryRanges(device.Get().first, 1, &memoryRange), "Failed to flush mapped memory range!");
						VK_CHECK(vkInvalidateMappedMemoryRanges(device.Get().first, 1, &memoryRange), "Failed to flush mapped memory range!");
					}


					vkUnmapMemory(device.Get().first, m_memory);
				}
			}

			inline void SetBufferData(const Device& device, DataType* data) {
				void* mappedData;
				// VkMemoryMapFlags is reserved should always be zero.
				vkMapMemory(device.Get().first, m_memory, 0, (uint32_t)sizeof(DataType) * m_size, 0, &mappedData);
				memcpy(mappedData, data, (size_t)sizeof(DataType) * m_size);

				if (m_hostCoherent) {
					VkMappedMemoryRange memoryRange{};
					memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
					memoryRange.offset = 0;
					memoryRange.size = VK_WHOLE_SIZE;
					memoryRange.memory = m_memory;

					VK_CHECK(vkFlushMappedMemoryRanges(device.Get().first, 1, &memoryRange), "Failed to flush mapped memory range!");
					VK_CHECK(vkInvalidateMappedMemoryRanges(device.Get().first, 1, &memoryRange), "Failed to flush mapped memory range!");
				}

				vkUnmapMemory(device.Get().first, m_memory);
			}

			/// <summary>
			/// Destroy the VkBuffer and free the VkDeviceMemory.
			/// </summary>
			/// <param name="device">A valid IRun::Vk::Device.</param>
			inline void Destroy(Device& device) {
				vkFreeMemory(device.Get().first, m_memory, nullptr);
				vkDestroyBuffer(device.Get().first, m_buffer, nullptr);
			}
			/// <returns>Size of the buffer.</returns>
			const inline size_t GetSize() const { return m_size; }
			/// <returns>Handle to the VkBuffer</returns>
			const inline VkBuffer Get() const { return m_buffer; }
			/// <returns>Handle to the VkDeviceMemory</returns>
			const inline VkDeviceMemory GetMemory() const { return m_memory; }

		private:
			VkBuffer m_buffer;
			VkDeviceMemory m_memory;
			size_t m_size;

			bool m_hostCoherent;

			inline uint32_t FindMemoryTypeIndex(Device& device, uint32_t allowedTypes, VkMemoryPropertyFlags propertyFlags) {
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

