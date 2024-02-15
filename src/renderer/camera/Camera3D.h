#pragma once

#include <IWindowWindow.h>
#include "ICamera.h"

namespace IRun {
	class Camera3D : public ICamera {
	public:
		Camera3D() = default;
		Camera3D(float fov, float aspect, const glm::vec2& minMaxDepth, const glm::vec3& position, const glm::vec3& front, const glm::vec3& up);

		inline virtual void SetPosition(const glm::vec3& position) override { m_position = position; }
		inline IRUN_NODISCARD virtual const glm::vec3& GetPosition() const override { return m_position; }

		inline virtual void SetFront(const glm::vec3& front) override { m_front = front; }
		inline IRUN_NODISCARD virtual const glm::vec3& GetFront() const override { return m_front; }

		inline virtual void SetUp(const glm::vec3& up) override { m_up = up; }
		inline IRUN_NODISCARD virtual const glm::vec3& GetUp() const override { return m_up; }

		inline virtual void SetProjection(const glm::mat4& projection) { m_projection = projection; }
		inline IRUN_NODISCARD virtual const glm::mat4& GetProjection() const { return m_projection; }

		virtual void SetView(const glm::mat4& view) { m_view = view; }
		inline IRUN_NODISCARD virtual const glm::mat4& GetView() const { return m_view; }

		virtual void Update(IStl::NullableReference<IWindow::Window> window) override;

	private:
		float m_fov, m_aspect;
		glm::vec2 m_minMaxDepth;
	};
}

