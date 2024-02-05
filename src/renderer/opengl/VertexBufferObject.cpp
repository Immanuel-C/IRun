#include "VertexBufferObject.h"

namespace IRun {
	namespace GL {
		VertexBufferObject::VertexBufferObject(float* vertices, size_t verticesSize) {
			glGenBuffers(1, &m_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(float), vertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)offsetof(Vertex, position));
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)offsetof(Vertex, uv));
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

	}
}