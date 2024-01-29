#pragma once

#include "Device.h"
#include "Error.h"

#include <functional>
#include <string>

namespace IRun {
	namespace Vk {
		/// <summary>
		/// A more secure way to store pipeline cache data to disk
		/// Sources: https://medium.com/@zeuxcg/creating-a-robust-pipeline-cache-with-vulkan-961d09416cda,
		/// https://github.com/LunarG/VulkanSamples/blob/master/API-Samples/pipeline_cache/pipeline_cache.cpp
		/// </summary>
		struct PipelineCacheHeader {
			// a random number that is defined in "renderer/vulkan/PipelineCache.cpp" to make sure its our file
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


			/// <summary>
			/// Hashes the pipeline header and Vulkan pipeline cache data.
			/// PipelineCacheHeader::dataHash is set to this hash for security.
			/// <param name="data">Buffer to Vulkan pipeline cache data without the Vulkan header.</param>
			/// </summary>
			size_t Hash(uint8_t* data) {
				return std::hash<std::string>()(ToString(data));
			}

		private:
			/// <summary>
			/// Turns this struct into a string form excluding PipelineCacheHeader::dataHash.
			/// data is a buffer of the pipeline cache data without the vulkan header.
			/// </summary>
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
			/// <summary>
			/// Creates a VkPipelineCache.
			/// </summary>
			/// <param name="device">A valid IRun::Vk::Device.</param>
			/// <param name="dataCache">A buffer to Vulkan pipeline cache data **with the Vulkan header** that is of version: VK_PIPELINE_CACHE_HEADER_VERSION_ONE.</param>
			/// <param name="device">Size of both the header and cache data.</param>
			/// <returns>IRun::ErrorCode::Success</returns>
			ErrorCode CreateCache(Device& device, uint8_t* dataCache, size_t dataSize);
			/// <summary>
			/// Saves the cache to a file. The file with be saved with the IRun pipeline cache format.
			/// </summary>
			/// <param name="filename">file to save cache data to.</param>
			/// <param name="device">A valid IRun::Vk::Device.</param>
			/// <returns>
			/// Returns IRun::ErrorCode::Success if the function succeeds.
			/// Returns IRun::ErrorCode::IoError if the file failed to open.
			/// </returns>
			ErrorCode SaveCache(const std::string& filename, Device& device);
			/// <summary>
			/// Saves the cache to a file. The file with should be in IRun pipeline cache format and will automatically convert the format to the appropriate Vulkan format and create a VkPipelineCache using that data.
			/// </summary>
			/// <param name="filename">file to get the cache data from.</param>
			/// <param name="device">A valid IRun::Vk::Device.</param>
			/// <returns>
			/// Returns IRun::ErrorCode::Success if the function succeeds.
			/// Returns IRun::ErrorCode::IoError if file failed to open.
			/// </returns>
			ErrorCode RetrieveCache(const std::string& filename, Device& device);
			/// <summary>
			/// Get the IRun::Vk::PipelineCacheHeader and the VkPipelineCache handle
			/// </summary>
			/// <returns>
			/// Returns an std::pair of PipelineCacheHeader and VkPipelineCache.
			/// </returns>
			const inline std::pair<PipelineCacheHeader, VkPipelineCache> Get() const { return { m_header, m_pipelineCache }; }
			/// <summary>
			/// Destroys the VkPipelineCache.
			/// </summary>
			/// <param name="device">A valid IRun::Vk::Device.</param>
			void Destroy(Device& device);
		private:
			PipelineCacheHeader m_header;
			VkPipelineCache m_pipelineCache;
		};
	}
}