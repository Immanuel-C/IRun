#pragma once

#include "Device.h"

namespace IRun {
	namespace Vk {
		typedef VkSemaphore Semaphore;
		typedef VkFence Fence;

		

		template<typename SyncObj>
		class Sync {
		public:
			Sync() = default;
			Sync(Device& device, VkFlags flags = 0) {
				if (std::is_same<SyncObj, Semaphore>().value) CreateSemaphore(device, flags);
				else if (std::is_same<SyncObj, Fence>().value)  CreateFence(device, flags);
				else I_LOG_FATAL_ERROR("typename SyncObj from class Sync must be VkSemaphore or VkFence!");
				
			}

			void Destroy(Device& device) {
				if (std::is_same<SyncObj, Semaphore>().value) { 
					vkDestroySemaphore(device.Get().first, (VkSemaphore)m_syncHandle, nullptr); 
					I_DEBUG_LOG_TRACE("Destroyed Vulkan semaphore: 0x%p", m_syncHandle);
				}
				else if (std::is_same<SyncObj, Fence>().value) { 
					vkDestroyFence(device.Get().first, (VkFence)m_syncHandle, nullptr);
					I_DEBUG_LOG_TRACE("Destroyed Vulkan fence: 0x%p", m_syncHandle);

				};
			}

			inline SyncObj Get() const { return m_syncHandle; }
		private:
			SyncObj m_syncHandle = nullptr;

			void CreateFence(Device& device, VkFlags flags) {
				VkFenceCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				createInfo.flags = flags;

				VK_CHECK(vkCreateFence(device.Get().first, &createInfo, nullptr, (VkFence*)&m_syncHandle), "Failed to create vulkan fence!")
				I_DEBUG_LOG_TRACE("Created Vulkan fence: 0x%p", m_syncHandle);
			}

			void CreateSemaphore(Device& device, VkFlags flags) {
				if (flags != 0)
					I_LOG_WARNING("IRun::Vk::Sync<IRun::Vk::Semaphore>(Device&, VkFlags) parameter VkFlags must always be zero! Vulkan spec states that VkSemaphoreCreateInfo::flags must always be zero. It is reserved.");

				VkSemaphoreCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;


				VK_CHECK(vkCreateSemaphore(device.Get().first, &createInfo, nullptr, (VkSemaphore*)&m_syncHandle), "Failed to create semaphore!");
				I_DEBUG_LOG_TRACE("Created Vulkan semaphore: 0x%p", m_syncHandle);
			}
		};
	}
}

