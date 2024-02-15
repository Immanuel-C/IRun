#include "DescriptorPool.h"

namespace IRun {
	namespace Vk {
		DescriptorPool::DescriptorPool(Device& device, size_t maxSets, size_t descriptorPoolSizeCount, VkDescriptorPoolSize* descriptorPoolSizes) {
			VkDescriptorPoolCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			createInfo.maxSets = (uint32_t)maxSets;
			createInfo.poolSizeCount = (uint32_t)descriptorPoolSizeCount;
			createInfo.pPoolSizes = descriptorPoolSizes;

			VK_CHECK(vkCreateDescriptorPool(device.Get().first, &createInfo, nullptr, &m_descriptorPool), "Failed to create Vulkan descriptor pool!");
			I_DEBUG_LOG_TRACE("Created Vulkan descriptor pool: 0x%p", m_descriptorPool);
		}

		DescriptorSet DescriptorPool::CreateDescriptorSet(const Device& device, size_t layoutBindingSize, VkDescriptorSetLayoutBinding* layoutBindings, VkDescriptorSetLayoutCreateFlags layoutFlags) {
			VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
			layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutCreateInfo.bindingCount = (uint32_t)layoutBindingSize;
			layoutCreateInfo.pBindings = layoutBindings;
			layoutCreateInfo.flags = layoutFlags;

			VkDescriptorSetLayout layout;

			VK_CHECK(vkCreateDescriptorSetLayout(device.Get().first, &layoutCreateInfo, nullptr, &layout), "Failed to create Vulkan descriptor set layout!");
			I_DEBUG_LOG_TRACE("Created Vulkan descriptor set layout: 0x%p", layout);

			// VkDescriptorSetLayout have 1:1 relationship with VkDescriptorSet
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_descriptorPool;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &layout;

			VkDescriptorSet descriptorSet;

			VK_CHECK(vkAllocateDescriptorSets(device.Get().first, &allocInfo, &descriptorSet), "Failed to create Vulkan descriptor set!");
			I_DEBUG_LOG_TRACE("Created Vulkan descriptor set: 0x%p", descriptorSet);

			m_descriptorSets.resize(m_descriptorSets.size() + 1);
			m_descriptorSetLayouts.resize(m_descriptorSetLayouts.size() + 1);

			DescriptorSet descriptorSetIndex = m_descriptorSets.size() - 1;

			m_descriptorSetLayouts[descriptorSetIndex] = layout;
			m_descriptorSets[descriptorSetIndex] = descriptorSet;

			return descriptorSetIndex;
		}

		void DescriptorPool::DestroyDescriptorSet(const Device& device, DescriptorSet descriptorSet) {
			I_DEBUG_ASSERT_FATAL_ERROR(descriptorSet < 0 || descriptorSet > m_descriptorSets.size(), "IRun::Vk::DescriptorPool::DestroyDescriptorSet: invalid descriptor set!");

			I_DEBUG_LOG_TRACE("Destroyed Vulkan descriptor set layout: 0x%p", m_descriptorSetLayouts[descriptorSet]);
			vkDestroyDescriptorSetLayout(device.Get().first, m_descriptorSetLayouts[descriptorSet], nullptr);

			VkDescriptorSet vkDescriptorSet = m_descriptorSets[descriptorSet];
			I_DEBUG_LOG_TRACE("Destroyed Vulkan descriptor set: 0x%p", vkDescriptorSet);
			vkFreeDescriptorSets(device.Get().first, m_descriptorPool, 1, &vkDescriptorSet);
		}

		void DescriptorPool::WriteBufferToDescriptor(const Device& device, DescriptorSet descriptorSet, uint32_t binding, VkDescriptorType descriptorType, VkBuffer buffer, size_t offset, size_t bufferSizeInBytes) {
			I_DEBUG_ASSERT_FATAL_ERROR(descriptorSet < 0 || descriptorSet > m_descriptorSets.size(), "IRun::Vk::DescriptorPool::BindBufferToDescriptor: invalid descriptor set!");

			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = buffer;
			bufferInfo.offset = (VkDeviceSize)offset;
			bufferInfo.range = (VkDeviceSize)bufferSizeInBytes;

			VkWriteDescriptorSet descriptorSetWrite{};
			descriptorSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorSetWrite.dstSet = m_descriptorSets[descriptorSet];
			descriptorSetWrite.dstBinding = binding;
			descriptorSetWrite.dstArrayElement = 0;
			descriptorSetWrite.descriptorType = descriptorType;
			descriptorSetWrite.descriptorCount = 1;
			descriptorSetWrite.pBufferInfo = &bufferInfo;
			descriptorSetWrite.pImageInfo = nullptr;
			descriptorSetWrite.pTexelBufferView = nullptr;

			// https://www.khronos.org/blog/vk-ext-descriptor-buffer
			vkUpdateDescriptorSets(device.Get().first, 1, &descriptorSetWrite, 0, nullptr);
		}

		void DescriptorPool::Destroy(const Device& device) {
			I_DEBUG_LOG_TRACE("Destroyed Vulkan descriptor pool: 0x%p", m_descriptorPool);
			vkDestroyDescriptorPool(device.Get().first, m_descriptorPool, nullptr);
			for (VkDescriptorSetLayout& layout : m_descriptorSetLayouts) {
				I_DEBUG_LOG_TRACE("Destroyed Vulkan descriptor set layout: 0x%p", layout);
				vkDestroyDescriptorSetLayout(device.Get().first, layout, nullptr);
			}
		}
	}
}