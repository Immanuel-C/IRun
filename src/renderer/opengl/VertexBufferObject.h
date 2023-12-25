#pragma once

#include <array>
#include <glad\glad.h>

namespace IRun {
	namespace GL {
		class VertexBufferObject {
		public:
			VertexBufferObject() = default;
			/// <summary>
			/// Creates the vertex buffer object.
			/// </summary>
			/// <param name="vertices">Array of vertex positions.</param>
			/// <param name="verticesSize">Size of the array.</param>
			/// <returns></returns>
			VertexBufferObject(float* vertices, size_t verticesSize);
			/// <summary>
			/// Bind this vertex buffer object.
			/// </summary>
			inline void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_vbo); }
			/// <summary>
			/// Unbind this vertex buffer object.
			/// </summary>
			inline void Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
			/// <summary>
			/// Get the OpenGL vertex buffer object handle.
			/// </summary>
			/// <returns>OpenGL vertex buffer object.</returns>
			inline const uint32_t& Get() const { return m_vbo; };
		private:
			uint32_t m_vbo = 0;
		};
	}
}

