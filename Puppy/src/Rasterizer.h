#pragma once

#include "Framebuffer.h"
#include "Vertex.h"
#include "MathUtils.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <vector>

class Rasterizer 
{
public:
	Rasterizer(Framebuffer& fb)
		: framebuffer(fb) { }

	void drawTriangle2D(const glm::vec2& v0, const glm::vec2& v1,
						const glm::vec2& v2, const glm::vec3& color);

	void drawTriangle3D(const Triangle3D& tri,
						const glm::mat4& modelMat,
						const glm::mat4& viewMat,
						const glm::mat4& projectionMat,
						MathUtils::CullingMode cullMode = MathUtils::CullingMode::NONE);

	void drawTriangle3D(const Triangle3D& tri,
		const Shader& shader,
		const ShaderContext& context,
		MathUtils::CullingMode cullMode = MathUtils::CullingMode::NONE);

private:
	Framebuffer& framebuffer;

	bool isInsideTriangle2D(const glm::vec2& p, const glm::vec2& v0,
							const glm::vec2& v1, const glm::vec2& v2);
	template<typename VecType>
	void getBoundingBox(const VecType& v0, const VecType& v1, const VecType& v2,
						int& minX, int& minY, int& maxX, int& maxY);

	// 辅助函数：插值顶点属性
	FragmentShaderInput interpolateVertex(const VertexShaderOutput& vo0,
		const VertexShaderOutput& vo1,
		const VertexShaderOutput& vo2,
		const glm::vec3& barycentric,
		float interpolatedOneOverW,
		const ShaderContext& context);
};