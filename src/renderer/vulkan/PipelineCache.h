#pragma once

#include "Device.h"

#include <functional>
#include <string>

namespace IRun {
	namespace Vk {
		// See https://medium.com/@zeuxcg/creating-a-robust-pipeline-cache-with-vulkan-961d09416cda
		// See https://github.com/LunarG/VulkanSamples/blob/master/API-Samples/pipeline_cache/pipeline_cache.cpp
		struct PipelineCacheHeader {
			// Our header to make sure its our file
			uint32_t magic;
			// Equal to *pDataSize returned by vkGetPipelineCacheData
			size_t dataSize;
			// A hash of pipeline cache data with the header
			size_t dataHash;

			// Equal to VkPhysicalDeviceProperties::vendorID. Retrieve from IRun::Vk::Device::GetDeviceProperties.
			uint32_t vendorID;
			// Equal to VkPhysicalDeviceProperties::deviceID. Retrieve from IRun::Vk::Device::GetDeviceProperties.
			uint32_t deviceID;
			// Equal to VkPhysicalDeviceProperties::driverVersion. Retrieve from IRun::Vk::Device::GetDeviceProperties.
			uint32_t driverVersion;

			// Equal to VkPhysicalDeviceProperties::pipelineCacheUUID. Retrieve from IRun::Vk::Device::GetDeviceProperties.
			uint8_t uuid[VK_UUID_SIZE];

			size_t Hash(uint8_t* data) {
				return std::hash<std::string>()(ToString(data));
			}

		private:
			std::string ToString(uint8_t* data) const {
				std::string string = std::string{ uuid, uuid + VK_UUID_SIZE };

				return std::to_string(magic) +
					std::to_string(dataSize) +
					std::to_string(vendorID) +
					std::to_string(deviceID) +
					std::to_string(driverVersion) + 
					std::string{ uuid, uuid + VK_UUID_SIZE } + 
					std::string{ data, data + dataSize };
			}
		};

		class PipelineCache {
		public:
			PipelineCache() = default;

			VkResult CreateCache(Device& device, uint8_t* dataCache, size_t dataSize);

			VkResult SaveCache(const std::string& filename, Device& device);

			VkResult RetrieveCache(const std::string& filename, Device& device);

			const inline std::pair<PipelineCacheHeader, VkPipelineCache> Get() const { return { m_header, m_pipelineCache }; }

			void Destroy(Device& device);

		private:
			PipelineCacheHeader m_header;
			VkPipelineCache m_pipelineCache;
		};
	}
}

