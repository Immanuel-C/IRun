#pragma once

#include "tools\File.h"

#include <atlbase.h>
#include <dxc\dxcapi.h>
#include <array>


namespace IRun {
	namespace Tools {
		namespace DXC {
			/// <summary>
			/// Allows the compilation of HLSL to SPIRV 
			/// </summary>
			/// <param name="vertShaderFilename">The file path to the vertex HLSL code</param>
			/// <param name="fragmentShaderFilename">The file path to the fragment HLSL code</param>
			/// <returns>Array of SPRIV byte code the 1st index is the vertex shader code and the 2nd index is the fragment shader code. Must call CComPtr::Release when finished with buffer.</returns>
			std::array<CComPtr<IDxcBlob>, 2> CompileHLSLtoSPRIV(const std::string& vertShaderFilename, const std::string& fragmentShaderFilename);
		}
	}
}

