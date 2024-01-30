#pragma once

namespace IRun {
	enum struct ShaderLanguage {
		HLSL,
		Spirv,
		Max
	};

	inline std::string ShaderLanguageToString(ShaderLanguage lang) {
		switch (lang)
		{
		case IRun::ShaderLanguage::HLSL:
			return "IRun::ShaderLanguage::HLSL";
			break;
		case IRun::ShaderLanguage::Spirv:
			return "IRun::ShaderLanguage::Spirv";
			break;
		default:
			return "Invalid shader language";
			break;
		}
	}

	inline ShaderLanguage StringToShaderLanguage(const std::string& str) {
		if (str == "IRun::ShaderLanguage::HLSL")
			return ShaderLanguage::HLSL;
		else if (str == "IRun::ShaderLanguage::Spirv")
			return ShaderLanguage::Spirv;
		else
			return (ShaderLanguage)INT64_MAX;

	}
}