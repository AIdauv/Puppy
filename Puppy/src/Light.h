#pragma once

#include <glm/glm.hpp>

struct Light
{
	glm::vec3 position;
	glm::vec3 color;
	float intensity;

	// 二次衰减模型，衰减系数：attenuation = 1.0 / (constant + linear * d + quadratic * d²)
	float constant;              // 常数衰减 Kc (通常为1.0)
	float linear;                // 线性衰减 Kl
	float quadratic;             // 二次衰减 Kq

	Light() : position(0, 5, 5), color(1, 1, 1), intensity(1.0f),
		constant(1.0f), linear(0.09f), quadratic(0.032f) {
	}

	Light(glm::vec3 pos, glm::vec3 col, float i = 1.0f, 
		float c = 1.0f, float l = 0.09f, float q = 0.032f)
		: position(pos), color(col), intensity(i), 
		constant(c), linear(l), quadratic(q) {
	}

	// 计算衰减因子
	float computeAttenuation(float distance) const {
		return 1.0f / (constant + linear * distance + quadratic * (distance * distance));
	}
};