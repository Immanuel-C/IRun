#include "PipelineCache.h"

#include "tools/File.h"

namespace IRun {
	namespace Vk {
		constexpr uint32_t MAGIC = 0x2834554734ui32;

		std::pair<uint8_t*, uint32_t> ConvertIRunPipelineCacheToVulkanPipelineCache(PipelineCacheHeader& header, uint8_t* cacheWithoutHeader, VkPipelineCacheHeaderVersion vkPipelineHeaderVersion) {
			if (vkPipelineHeaderVersion != VK_PIPELINE_CACHE_HEADER_VERSION_ONE) {
				I_LOG_ERROR("IRun's pipeline cache only supports converting to Vulkan pipeline cache version one!");
				return { nullptr, 0 };
			}

			// Offset	 Size            Meaning
			// ------    ------------    ------------------------------------------------------------------
			//      0               4    length in bytes of the entire pipeline cache header written as a
			//                           stream of bytes, with the least significant byte first
			//      4               4    a VkPipelineCacheHeaderVersion value written as a stream of bytes,
			//                           with the least significant byte first
			//      8               4    a vendor ID equal to VkPhysicalDeviceProperties::vendorID written
			//                           as a stream of bytes, with the least significant byte first
			//     12               4    a device ID equal to VkPhysicalDeviceProperties::deviceID written
			//                           as a stream of bytes, with the least significant byte first
			//     16    VK_UUID_SIZE    a pipeline cache ID equal to VkPhysicalDeviceProperties::pipelineCacheUUID

			uint32_t sizeOfHeader = 4 + 4 + 4 + 4 + VK_UUID_SIZE;
			uint32_t pipelineHeaderVersion = (uint32_t)vkPipelineHeaderVersion;

			uint8_t* vkPipelineCache = new uint8_t[(sizeOfHeader + header.dataSize)];

			size_t offset = 0;

			memcpy(vkPipelineCache, &sizeOfHeader, 4);
			offset += 4;
			memcpy(vkPipelineCache + offset, &pipelineHeaderVersion, 4);
			offset += 4;
			memcpy(vkPipelineCache + offset, &header.vendorID, 4);
			offset += 4;
			memcpy(vkPipelineCache + offset, &header.deviceID, 4);
			offset += 4;
			memcpy(vkPipelineCache + offset, &header.uuid, VK_UUID_SIZE);
			offset += VK_UUID_SIZE;
			memcpy(vkPipelineCache + offset, cacheWithoutHeader, header.dataSize);

			return { vkPipelineCache, sizeOfHeader };
		}

		std::pair<uint8_t*, size_t> ConvertVulkanPipelineCacheToIRunPipelineCache(uint8_t* vkCacheDataWithHeader, size_t size, PipelineCacheHeader& header) {
			size_t sizeOfVulkanHeader = 4 + 4 + 4 + 4 + VK_UUID_SIZE;
			size_t sizeOfIRunHeader = 4 + 8 + 8 + 4 + 4 + 4 + VK_UUID_SIZE;
			size_t sizeOfCacheDataWithoutHeader = (size - sizeOfVulkanHeader);

			uint8_t* iRunPipelineCache = new uint8_t[(sizeOfCacheDataWithoutHeader + sizeOfIRunHeader)];

			uint8_t* vkCacheDataWithoutHeader = new uint8_t[sizeOfCacheDataWithoutHeader];

			memcpy(vkCacheDataWithoutHeader, vkCacheDataWithHeader + sizeOfVulkanHeader, sizeOfCacheDataWithoutHeader);

			size_t offset = 0;
			memcpy(iRunPipelineCache, &MAGIC, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			memcpy(iRunPipelineCache + offset, &sizeOfCacheDataWithoutHeader, sizeof(size_t));
			offset += sizeof(size_t);
			size_t hash = header.Hash(vkCacheDataWithoutHeader);
			memcpy(iRunPipelineCache + offset, &hash, sizeof(size_t));
			offset += sizeof(size_t);
			memcpy(iRunPipelineCache + offset, &header.vendorID, sizeof(uint32_t));
			offset += sizeof(uint32_t);
			memcpy(iRunPipelineCache + offset, &header.deviceID, sizeof(uint32_t));
			offset += sizeof(uint32_t);
			memcpy(iRunPipelineCache + offset, &header.driverVersion, sizeof(uint32_t));
			offset += sizeof(uint32_t);
			memcpy(iRunPipelineCache + offset, &header.uuid, (VK_UUID_SIZE * sizeof(uint8_t)));
			offset += (VK_UUID_SIZE * sizeof(uint8_t));
			memcpy(iRunPipelineCache + offset, vkCacheDataWithoutHeader, (sizeOfCacheDataWithoutHeader * sizeof(uint8_t)));

			delete[] vkCacheDataWithoutHeader;

			return { iRunPipelineCache, { size + sizeOfIRunHeader } };
		}

		ErrorCode PipelineCache::SaveCache(const std::string& filename, Device& device) {
			size_t dataSize = 0;

			VK_CHECK(vkGetPipelineCacheData(device.Get().first, m_pipelineCache, &dataSize, nullptr), "Failed to get pipeline cache data!");

			uint8_t* cacheData = new uint8_t[dataSize];

			VK_CHECK(vkGetPipelineCacheData(device.Get().first, m_pipelineCache, &dataSize, cacheData), "Failed to get pipeline cache data!");

			m_header.vendorID = device.GetDeviceProperties().vendorID;
			m_header.deviceID = device.GetDeviceProperties().deviceID;
			m_header.driverVersion = device.GetDeviceProperties().driverVersion;
			memcpy(m_header.uuid, device.GetDeviceProperties().pipelineCacheUUID, (VK_UUID_SIZE * sizeof(uint8_t)));

			size_t sizeOfVulkanHeader = 4 + 4 + 4 + 4 + VK_UUID_SIZE;

			std::pair<uint8_t*, size_t> convertedData = ConvertVulkanPipelineCacheToIRunPipelineCache(cacheData, (dataSize + sizeOfVulkanHeader), m_header);

			std::string directories = filename.substr(0, filename.find_last_of("/\\"));

			std::filesystem::create_directories(directories);

			std::ofstream file{};

			file.open(filename, std::ios::trunc | std::ios::binary);

			if (!file.is_open()) {
				I_LOG_ERROR("Failed to save cache at: %s", filename.c_str());
				file.close();
				return ErrorCode::IoError;
			}

			file.write((char*)convertedData.first, convertedData.second);

			file.close();

			delete[] convertedData.first;
			delete[] cacheData;

			return ErrorCode::Success;
		}

		ErrorCode PipelineCache::CreateCache(Device& device, uint8_t* dataCache, size_t dataSize) {
			VkPipelineCacheCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			createInfo.initialDataSize = dataSize;
			createInfo.pInitialData = dataCache;

			VK_CHECK(vkCreatePipelineCache(device.Get().first, &createInfo, nullptr, &m_pipelineCache), "Failed to create pipeline cache!");

			return ErrorCode::Success;
		}

		ErrorCode PipelineCache::RetrieveCache(const std::string& filename, Device& device) {
			std::ifstream file{};

			file.open(filename, std::ios::binary | std::ios::ate);

			if (!file.is_open()) {
				file.close();
				return ErrorCode::IoError;
			}

			size_t cacheDataSize = (size_t)file.tellg();

			uint8_t* cacheData = new uint8_t[cacheDataSize];
			
			file.seekg(std::ios_base::beg);
			file.read((char*)cacheData, cacheDataSize);
			file.close();

			size_t offset = 0;

			memcpy(&m_header.magic, cacheData, 4);
			offset += 4;
			memcpy(&m_header.dataSize, cacheData + offset, 8);
			offset += 8;
			memcpy(&m_header.dataHash, cacheData + offset, 8);
			offset += 8;
			memcpy(&m_header.vendorID, cacheData + offset, 4);
			offset += 4;
			memcpy(&m_header.deviceID, cacheData + offset, 4);
			offset += 4;
			memcpy(&m_header.driverVersion, cacheData + offset, 4);
			offset += 4;
			memcpy(m_header.uuid, cacheData + offset, VK_UUID_SIZE);
			offset += VK_UUID_SIZE;

			uint8_t* cacheDataWithoutHeader = new uint8_t[m_header.dataSize];

			memcpy(cacheDataWithoutHeader, cacheData + offset, m_header.dataSize);

			bool badCache = false;

			if (m_header.Hash(cacheDataWithoutHeader) != m_header.dataHash) badCache = true;

			if (m_header.magic != MAGIC) badCache = true;

			if (m_header.vendorID != device.GetDeviceProperties().vendorID) badCache = true;

			if (m_header.deviceID != device.GetDeviceProperties().deviceID) badCache = true;

			if (m_header.driverVersion != device.GetDeviceProperties().driverVersion) badCache = true;

			if (memcmp(m_header.uuid, device.GetDeviceProperties().pipelineCacheUUID, sizeof(m_header.uuid)) != 0) badCache = true;

			if (badCache) {
				delete[] cacheData;
				delete[] cacheDataWithoutHeader;

				return ErrorCode::Corrupt;
			}

			std::pair<uint8_t*, uint32_t> convertedCache = ConvertIRunPipelineCacheToVulkanPipelineCache(m_header, cacheDataWithoutHeader, VK_PIPELINE_CACHE_HEADER_VERSION_ONE);

			CreateCache(device, convertedCache.first, (convertedCache.second + m_header.dataSize));

			delete[] convertedCache.first;
			delete[] cacheDataWithoutHeader;

			return ErrorCode::Success;
		}

		void PipelineCache::Destroy(Device& device) {
			m_header = {};
			vkDestroyPipelineCache(device.Get().first, m_pipelineCache, nullptr);
		}

	}
}