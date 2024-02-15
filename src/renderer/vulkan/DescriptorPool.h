#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"

namespace IRun {
	namespace Vk {
		typedef size_t DescriptorSet;

		class DescriptorPool {
		public:
			DescriptorPool() = default;

			DescriptorPool(Device& device, size_t maxSets, size_t descriptorPoolSizeCount, VkDescriptorPoolSize* descriptorPoolSizes);

			DescriptorSet CreateDescriptorSet(const Device& device, size_t layoutBindingSize, VkDescriptorSetLayoutBinding* layoutBindings, VkDescriptorSetLayoutCreateFlags layoutFlags = 0);

			void DestroyDescriptorSet(const Device& device, DescriptorSet descriptorSet);

			void WriteBufferToDescriptor(const Device& device, DescriptorSet descriptorSet, uint32_t binding, VkDescriptorType descriptorType, VkBuffer buffer, size_t offset, size_t bufferSizeInBytes);

			const inline VkDescriptorPool& Get() const { return m_descriptorPool; }

			const inline std::vector<VkDescriptorSet>& GetDescriptorSets() const { return m_descriptorSets; }

			const inline std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayouts() const { return m_descriptorSetLayouts; }

			const inline VkDescriptorSet& GetDescriptorSet(DescriptorSet descriptorSet) { 
				I_ASSERT_FATAL_ERROR(descriptorSet < 0 || descriptorSet > m_descriptorSets.size(), "IRun::Vk::DescriptorPool::GetDescriptorSet failed. Param descriptorSet must be a valid descriptor set!");
				return m_descriptorSets[descriptorSet]; 
			}

			const inline VkDescriptorSetLayout& GetDescriptorSetLayout(DescriptorSet descriptorSet) { 
				I_ASSERT_FATAL_ERROR(descriptorSet < 0 || descriptorSet > m_descriptorSetLayouts.size(), "IRun::Vk::DescriptorPool::GetDescriptorSetLayout failed. Param descriptorSet must be a valid descriptor set!");
				return m_descriptorSetLayouts[descriptorSet]; 
			}

			void Destroy(const Device& device);
		private:
			VkDescriptorPool m_descriptorPool;
			std::vector<VkDescriptorSet> m_descriptorSets;
			std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
		};
	}
}

