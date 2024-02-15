#pragma once

#define GLM_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "math/Vector.h"

namespace IRun {
	struct Vertex {
		Math::Vector3 position;
		Math::Vector2 uv;
	};

	struct Mvp {
		glm::mat4 proj;
		glm::mat4 view;
		glm::mat4 model;
	};
}