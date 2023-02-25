#pragma once

#include "Utils/LoadFile.h"
#include "Device.h"

#include <vulkan/vulkan.hpp>

namespace IRun {
	namespace Vulkan {
		enum struct ShaderType {
			Vertex = (int)vk::ShaderStageFlagBits::eVertex,
			Fragment = (int)vk::ShaderStageFlagBits::eFragment
		};

		class ShaderModule {
		public:
			ShaderModule(const std::string& fileName, ShaderType shaderType, Device& device);
			void Destroy(Device& device);
			vk::ShaderModule& Get();
		private:
			vk::ShaderModule m_shaderModule;
		};
	}
}

