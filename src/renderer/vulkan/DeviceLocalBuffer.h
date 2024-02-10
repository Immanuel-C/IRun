#pragma once

#include "Device.h"
#include "Buffer.h"
#include "CommandPool.h"

namespace IRun {
	namespace Vk {
		template<typename DataType>
		class DeviceLocalBuffer
		{
		public:
			DeviceLocalBuffer() = default;
			/// <summary>
			/// 
			/// </summary>
			/// <param name="device"></param>
			/// <param name="data"></param>
			/// <param name="dataSize"></param>
			/// <param name="usageFlags"></param>
			/// <param name="sharingMode"></param>
			DeviceLocalBuffer(Device& device, CommandPool& transferCommandPool, DataType* data, size_t dataSize, VkBufferUsageFlags usageFlags) 
			{
				// TODO: Check if transfer queue family == graphics queue family.
				Buffer<DataType> stagingBuffer = Buffer<DataType>{ device, data, dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

				m_deviceLocalBuffer = Buffer<DataType>{ device, data, dataSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usageFlags, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, BufferFlags::NoMap };

				CommandBuffer transferCommandBuffer = transferCommandPool.CreateBuffer(device, CommandBufferLevel::Primary);

				transferCommandPool.BeginRecordingCommands(device, transferCommandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

					{
						VkBufferMemoryBarrier barrier{};
						barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
						barrier.srcAccessMask = VK_ACCESS_NONE;
						barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
						barrier.buffer = m_deviceLocalBuffer.Get();
						barrier.size = m_deviceLocalBuffer.GetSize() * sizeof(DataType);

						vkCmdPipelineBarrier(transferCommandPool[transferCommandBuffer], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 1, &barrier, 0, nullptr);
					}

					VkBufferCopy bufferCopyRegion{};
					bufferCopyRegion.srcOffset = 0;
					bufferCopyRegion.dstOffset = 0;
					bufferCopyRegion.size = dataSize * sizeof(DataType);

					vkCmdCopyBuffer(transferCommandPool[transferCommandBuffer], stagingBuffer.Get(), m_deviceLocalBuffer.Get(), 1, &bufferCopyRegion);

					{
						VkBufferMemoryBarrier barrier{};
						barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
						barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
						barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
						barrier.buffer = m_deviceLocalBuffer.Get();
						barrier.size = m_deviceLocalBuffer.GetSize() * sizeof(DataType);

						vkCmdPipelineBarrier(transferCommandPool[transferCommandBuffer], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 0, nullptr, 1, &barrier, 0, nullptr);
					}


				transferCommandPool.EndRecordingCommands(transferCommandBuffer);

				VkSubmitInfo submitInfo{};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

				std::array<VkCommandBuffer, 1> submitCommandBuffer{
					transferCommandPool[transferCommandBuffer]
				};

				submitInfo.commandBufferCount = (uint32_t)submitCommandBuffer.size();
				submitInfo.pCommandBuffers = submitCommandBuffer.data();

				vkQueueSubmit(device.GetQueues().at(QueueType::Transfer), 1, &submitInfo, nullptr);
				vkQueueWaitIdle(device.GetQueues().at(QueueType::Transfer));

				transferCommandPool.DestroyCommandBuffer(device, transferCommandBuffer);

				stagingBuffer.Destroy(device);
			}

			void Destroy(Device& device) {
				m_deviceLocalBuffer.Destroy(device);
			}

			const Buffer<DataType>& Get() const { return m_deviceLocalBuffer; };
		private:
			Buffer<DataType> m_deviceLocalBuffer;
		};
	}
}