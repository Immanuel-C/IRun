#include "IndexBufferObject.h"


namespace IRun {
	namespace GL {
		IndexBufferObject::IndexBufferObject(uint32_t* indices, size_t indicesSize) {
			glGenBuffers(1, &m_ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(uint32_t), indices, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}
}