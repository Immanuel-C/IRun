#include "ShaderProgram.h"

#include "tools\dxc\HLSLCompiler.h"


namespace IRun {
	namespace GL {
		ShaderProgram::ShaderProgram(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename) {
			uint32_t vertexShader, fragmentShader;
			
			int success;
			char infolog[512];

			//std::string vertexShaderSource = Tools::ReadFile(vertexShaderFilename, std::ios::binary);

			std::array<CComPtr<IDxcBlob>, 2> shaderBin = Tools::DXC::CompileHLSLtoSPRIV(vertexShaderFilename, fragmentShaderFilename);

			vertexShader = glCreateShader(GL_VERTEX_SHADER);
			// const char* vertexShaderSourceCStr = vertexShaderSource.c_str();
			// glShaderSource(vertexShader, 1, &vertexShaderSourceCStr, nullptr);
			// glCompileShader(vertexShader);

			glShaderBinary(1, &vertexShader, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBin[0]->GetBufferPointer(), (int)shaderBin[0]->GetBufferSize());
			glSpecializeShader(vertexShader, "main", 0, 0, 0);

			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(vertexShader, 512, nullptr, infolog);
				I_LOG_FATAL_ERROR("Failed to compile vertex shader: %s", infolog);
			}

			//std::string fragmentShaderSource = Tools::ReadFile(fragmentShaderFilename);

			fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			// const char* fragmentShaderSourceCStr = fragmentShaderSource.c_str();
			// glShaderSource(fragmentShader, 1, &fragmentShaderSourceCStr, nullptr);
			// glCompileShader(fragmentShader);

			glShaderBinary(1, &fragmentShader, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBin[1]->GetBufferPointer(), (int)shaderBin[1]->GetBufferSize());
			glSpecializeShader(fragmentShader, "main", 0, 0, 0);

			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(fragmentShader, 512, nullptr, infolog);
				I_LOG_FATAL_ERROR("Failed to compile fragment shader: %s", infolog);
			}


			m_shaderProgram = glCreateProgram();
			glAttachShader(m_shaderProgram, vertexShader);
			glAttachShader(m_shaderProgram, fragmentShader);
			glLinkProgram(m_shaderProgram);

			glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(m_shaderProgram, 512, nullptr, infolog);
				I_LOG_FATAL_ERROR("Failed to link shaders: %s", infolog);
			}

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			shaderBin[0].Release();
			shaderBin[1].Release();
		}
	}
}