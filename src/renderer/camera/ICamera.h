#pragma once

#define GLM_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Nullable/IStlNullableReference.hpp>

#include "Core.h"

namespace IRun {
	/// Abstract/interface class
	class ICamera {
	public:
		ICamera() = default;
		ICamera(const glm::mat4& projection, const glm::vec3& position, const glm::vec3& front, const glm::vec3& up) : m_view{ 1.0f }, m_projection{ projection }, m_position{ position }, m_front{ front }, m_up{ up } {}

		virtual void SetPosition(const glm::vec3& position) = 0;
		IRUN_NODISCARD virtual const glm::vec3& GetPosition() const = 0;

		virtual void SetFront(const glm::vec3& front) = 0;
		IRUN_NODISCARD virtual const glm::vec3& GetFront() const = 0;

		virtual void SetUp(const glm::vec3& up) = 0;
		IRUN_NODISCARD virtual const glm::vec3& GetUp() const = 0;

		virtual void SetProjection(const glm::mat4& projection) = 0;
		IRUN_NODISCARD virtual const glm::mat4& GetProjection() const = 0;

		virtual void SetView(const glm::mat4& view) = 0;
		IRUN_NODISCARD virtual const glm::mat4& GetView() const = 0;

		virtual void Update(IStl::NullableReference<IWindow::Window> window) = 0;

		virtual ~ICamera() = default;
	protected:
		glm::mat4 m_view{ 1.0f };
		glm::mat4 m_projection{ 1.0f };

		glm::vec3 m_position{ 1.0f };
		glm::vec3 m_front{ 1.0f };
		glm::vec3 m_up{ 1.0f };
	};
}