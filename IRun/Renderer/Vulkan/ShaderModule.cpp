#include "ShaderModule.h"

namespace IRun {
	namespace Vulkan {
		ShaderModule::ShaderModule(const std::string& fileName, ShaderType shaderType, Device& device) {
			std::string shaderContents = Utils::ReadFile(fileName, std::ios::binary);

			vk::ShaderModuleCreateInfo createInfo{};
			createInfo.codeSize = shaderContents.size();
			createInfo.pCode = (const uint32_t*)shaderContents.data();

			m_shaderModule = device.Get().createShaderModule(createInfo);
			
		}

		void ShaderModule::Destroy(Device& device) {
			device.Get().destroyShaderModule();
		}

		vk::ShaderModule& ShaderModule::Get() { return m_shaderModule; }
	}
}
