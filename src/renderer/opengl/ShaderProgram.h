#pragma once

#include <glad\glad.h>
#include "tools\File.h"

namespace IRun {
	namespace GL {
		class ShaderProgram {
		public:
			ShaderProgram() = default;
			/// <summary>
			/// Create shader program.
			/// </summary>
			/// <param name="vertexShaderFilename">File path to the vertex shader that is being used.</param>
			/// <param name="fragmentShaderFilename">File path to the fragment shader that is being used.</param>
			ShaderProgram(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename);
			/// <summary>
			/// Bind this shader program.
			/// </summary>
			inline void Bind() const { glUseProgram(m_shaderProgram); }
			/// <summary>
			/// Unbind this shader program.
			/// </summary>
			inline void Unbind() const { glUseProgram(0); }
			/// <summary>
			/// Get the OpenGL shader program handle.
			/// </summary>
			/// <returns>OpenGL shader program handle</returns>
			inline const uint32_t& Get() const { return m_shaderProgram; }
		private:
			uint32_t m_shaderProgram;
		};
	}
}

