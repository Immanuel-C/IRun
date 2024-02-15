#include "Camera3D.h"


namespace IRun {
	Camera3D::Camera3D(float fov, float aspect, const glm::vec2& minMaxDepth, const glm::vec3& position, const glm::vec3& front, const glm::vec3& up) 
		: ICamera(glm::perspective(fov, aspect, minMaxDepth.x, minMaxDepth.y), position, front, up), m_fov{ fov }, m_aspect{ aspect }, m_minMaxDepth{ minMaxDepth } {}
	void Camera3D::Update(IStl::NullableReference<IWindow::Window> window) { 
		if (!window.IsNull()) {
			IWindow::Vector2<int32_t> framebufferSize = window.GetValue().GetFramebufferSize();
			m_aspect = (float)framebufferSize.width / framebufferSize.height;
			m_projection = glm::perspective(m_fov, m_aspect, m_minMaxDepth.x, m_minMaxDepth.y);
			// Convert from origin being in bottom left to top left
			m_projection[1][1] *= -1;
		}

		m_view = glm::lookAt(m_position, m_position + m_front, m_up); 
	}
}