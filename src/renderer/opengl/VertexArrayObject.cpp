#include "VertexArrayObject.h"

namespace IRun {
	namespace GL {
		VertexArrayObject::VertexArrayObject() {
			glGenVertexArrays(1, &m_vao);
		}
	}
}