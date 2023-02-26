#include "ShaderModule.h"

namespace IRun {
	namespace Vulkan {
		ShaderModule::ShaderModule(const std::string& fileName, ShaderType shaderType, Device& device) : m_shaderType{ shaderType } {
			std::vector<char> shaderContents = Utils::ReadFile(fileName, std::ios::binary);

			vk::ShaderModuleCreateInfo createInfo{};
			createInfo.codeSize = shaderContents.size();
			createInfo.pCode = (const uint32_t*)shaderContents.data();

			m_shaderModule = device.Get().createShaderModule(createInfo);
		
#ifdef _DEBUG
			std::string shaderTypeString;
			switch (m_shaderType) {
				case ShaderType::Vertex:
					shaderTypeString = "Vertex Shader";
					break;
				case ShaderType::Fragment:
					shaderTypeString = "Fragment Shader";
					break;
			}
			I_DEBUG_LOG_INFO("Shader Module: %p, Type: %s", m_shaderModule, shaderTypeString.c_str());
#endif

		}

		void ShaderModule::Destroy(Device& device) {
#ifdef _DEBUG
			std::string shaderTypeString;
			switch (m_shaderType) {
			case ShaderType::Vertex:
				shaderTypeString = "Vertex Shader";
				break;
			case ShaderType::Fragment:
				shaderTypeString = "Fragment Shader";
				break;
			}
			I_DEBUG_LOG_INFO("Shader Module Deleted: %p, Type: %s", m_shaderModule, shaderTypeString.c_str());
#endif
			device.Get().destroyShaderModule(m_shaderModule);

		}

		vk::ShaderModule& ShaderModule::Get() { return m_shaderModule; }
	}
}
