#pragma once

#include "ShaderProgram.h"
#include <unordered_map>

namespace IRun {
	namespace GL {
		class ShaderBank {
		public:
			ShaderBank() = default;
			void AddShader(std::pair<std::string, ShaderProgram> shader);
			void RemoveShader(const std::string& key);
		private:
			std::unordered_map<std::string, ShaderProgram> m_shaders;
		};
	}
}

