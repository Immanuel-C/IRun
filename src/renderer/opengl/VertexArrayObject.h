#pragma once

#include <glad\glad.h>
#include <cstdint>

namespace IRun {
	namespace GL {
		class VertexArrayObject	{
		public:
			/// <summary>
			/// Create this vertex array object
			/// </summary>
			VertexArrayObject();
			/// <summary>
			/// Bind this vertex array object.
			/// </summary>
			/// <returns></returns>
			inline void Bind() const { glBindVertexArray(m_vao); }
			/// <summary>
			/// Unbind this vertex array object.
			/// </summary>
			/// <returns></returns>
			inline void Unbind() const { glBindVertexArray(0); }
			/// <summary>
			/// Get the OpenGL vertex array object handle
			/// </summary>
			/// <returns>OpenGL vertex array object handle</returns>
			inline const uint32_t& Get() const { return m_vao; }
		private:
			uint32_t m_vao;
		};
	}
}

