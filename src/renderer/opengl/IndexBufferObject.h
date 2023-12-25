#pragma once

#include <glad\glad.h>
#include <array>

namespace IRun {
	namespace GL {
		/// <summary>
		/// Called IndexBufferObject instead of ElementArrayObject to be consistent with Direct3D11.
		/// </summary>
		class IndexBufferObject {
		public:
			IndexBufferObject() = default;
			/// <summary>
			/// Create index buffer object
			/// </summary>
			/// <typeparam name="arraySize">size of the indices array</typeparam>
			/// <param name="indices">contains the indices of the vertices in the vertex buffer object</param>
			IndexBufferObject(uint32_t* indices, size_t indicesSize);
			/// <summary>
			/// Binds this index buffer object
			/// </summary>
			inline void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo); }
			/// <summary>
			/// Unbinds this index buffer object
			/// </summary>
			inline void Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
			/// <summary>
			/// Get the OpenGL index buffer object handle
			/// </summary>
			/// <returns>OpenGL index buffer object handle</returns>
			inline uint32_t Get() const { return m_ibo; }

		private:
			uint32_t m_ibo;
		};
	}
}

