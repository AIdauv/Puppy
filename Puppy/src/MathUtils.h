#pragma once

#include "Vertex.h"
#include <glm/glm.hpp>

namespace MathUtils 
{

	enum CullingMode
	{
		NONE,
		BACK,
		FRONT,
		ALL
	};

	inline bool isCulled(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, CullingMode mode = CullingMode::BACK) {
		glm::vec3 edgeA = v1 - v0;
		glm::vec3 edgeB = v2 - v0;

		glm::vec3 center = (v0 + v1 + v2) / 3.0f;

		glm::vec3 normal = glm::cross(edgeA, edgeB);
		glm::vec3 viewDir = -center;  // 视图空间相机在原点

		switch (mode)
		{
		case MathUtils::NONE:
			return false;
		case MathUtils::BACK:
			return glm::dot(normal, viewDir) <= 0.0f;
		case MathUtils::FRONT:
			return glm::dot(normal, viewDir) >= 0.0f;
		case MathUtils::ALL:
			return true;
		default:
			return false;
		}


	}

	inline glm::vec3 viewportTransform(const glm::vec3& ndc, int width, int height) {
		float x = (ndc.x + 1) * 0.5f * width;
		float y = (1 - ndc.y) * 0.5f * height;
		float z = ndc.z;

		return { x, y, z };
	}

	// 解析法，更优
	inline glm::vec3 barycentric2D(const glm::vec2& A,
		const glm::vec2& B,
		const glm::vec2& C,
		const glm::vec2& P) {

		glm::vec2 v0 = B - A;
		glm::vec2 v1 = C - A;
		glm::vec2 v2 = P - A;

		float d00 = glm::dot(v0, v0);
		float d01 = glm::dot(v0, v1);
		float d11 = glm::dot(v1, v1);
		float d20 = glm::dot(v2, v0);
		float d21 = glm::dot(v2, v1);

		float denom = d00 * d11 - d01 * d01;

		float beta = (d11 * d20 - d01 * d21) / denom;
		float gamma = (d00 * d21 - d01 * d20) / denom;
		float alpha = 1.0f - beta - gamma;

		return { alpha, beta, gamma };
	}


	// 面积法
	inline glm::vec3 barycentric3D(const glm::vec3& A, 
		const glm::vec3& B, 
		const glm::vec3& C, 
		const glm::vec3& P) {
		
		glm::vec3 normalABC = glm::cross(B - A, C - A);

		glm::vec3 crossPAB = glm::cross(A - P, B - P);
		glm::vec3 crossPBC = glm::cross(B - P, C - P);
		glm::vec3 crossPCA = glm::cross(C - P, A - P);

		float areaPAB = glm::dot(normalABC, crossPAB);  // |normal| * |crossPBC| * cosθ -> 2*areaABC*2*areaPAB -> 4*areaABC*areaPAB
		float areaPBC = glm::dot(normalABC, crossPBC);
		float areaPCA = glm::dot(normalABC, crossPCA);

		float totalArea = areaPAB + areaPBC + areaPCA;  // 4*areaABC*areaPAB + 4*areaABC*areaPBC + 4*areaABC*areaPCA

		float alpha = areaPBC / totalArea;
		float beta = areaPCA / totalArea;
		float gamma = areaPAB / totalArea;

		return { alpha, beta, gamma };
	}
}