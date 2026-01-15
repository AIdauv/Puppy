#pragma once

#include <glm/glm.hpp>

struct Light
{
	glm::vec3 position;
	glm::vec3 color;
	float intensity;

	Light() : position(0, 5, 5), color(1, 1, 1), intensity(1.0f) {}
	Light(glm::vec3 pos, glm::vec3 col, float i = 1.0f)
		: position(pos), color(col), intensity(i) {
	}
};