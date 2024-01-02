#pragma once

// In Vulkan SDK
#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/ResourceLimits.h>

#include <string>
#include <vector>

#include "tools\File.h"
namespace IRun {
	namespace Tools {
		namespace Shaders {

			enum struct ShaderLanguage {
				GLSL = GLSLANG_SOURCE_GLSL,
				HLSL = GLSLANG_SOURCE_HLSL
			};

			enum struct ShaderType {
				Vertex = GLSLANG_STAGE_VERTEX,
				Fragment = GLSLANG_STAGE_FRAGMENT,
			};

			std::vector<uint32_t> CompileToSpirvBinaryUint32_t(const std::string& fileName, ShaderType type, ShaderLanguage lang);
			std::vector<const char*> CompileToSpirvBinary(const std::string& fileName, ShaderType type, ShaderLanguage lang);
				
		}
	}
}

