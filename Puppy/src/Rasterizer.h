#pragma once

#include "Framebuffer.h"
#include "MultisampleFramebuffer.h"
#include "Vertex.h"
#include "MathUtils.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <vector>

class Rasterizer 
{
public:
	Rasterizer() = default;

	void drawTriangle2D(const glm::vec2& v0, const glm::vec2& v1,
						const glm::vec2& v2, const glm::vec3& color, Framebuffer& framebuffer);

	void drawTriangle3D(const Triangle3D& tri,
						const glm::mat4& modelMat,
						const glm::mat4& viewMat,
						const glm::mat4& projectionMat,
						Framebuffer& framebuffer,
						MathUtils::CullingMode cullMode = MathUtils::CullingMode::NONE);

	void drawTriangle3D(const Triangle3D& tri,
		const Shader& shader,
		const ShaderContext& context,
		Framebuffer& framebuffer,
		MathUtils::CullingMode cullMode = MathUtils::CullingMode::NONE);

	void drawTriangle3D(const Triangle3D& tri,
		const Shader& shader,
		const ShaderContext& context,
		MultisampleFramebuffer& framebuffer,
		MathUtils::CullingMode cullMode = MathUtils::CullingMode::NONE);

private:

	bool isInsideTriangle2D(const glm::vec2& p, const glm::vec2& v0,
							const glm::vec2& v1, const glm::vec2& v2);
	template<typename VecType, typename FrameBufferType>
	void getBoundingBox(const VecType& v0, const VecType& v1, const VecType& v2,
						int& minX, int& minY, int& maxX, int& maxY, FrameBufferType framebuffer);

	// 辅助函数：插值顶点属性
	FragmentShaderInput interpolateVertex(const VertexShaderOutput& vo0,
		const VertexShaderOutput& vo1,
		const VertexShaderOutput& vo2,
		const glm::vec3& barycentric,
		float interpolatedOneOverW,
		const ShaderContext& context);

	bool needGradient(const ShaderContext& context);

	// 三角形梯度计算（三角形整体的纹理坐标梯度，每个三角形只需计算一次）
	static void computeTriangleGradient(const VertexShaderOutput& v0,
		const VertexShaderOutput& v1,
		const VertexShaderOutput& v2,
		const glm::vec2& ScPos0,
		const glm::vec2& ScPos1,
		const glm::vec2& ScPos2,
		glm::vec2& dTdx, glm::vec2& dTdy);

	// 像素四元组梯度计算（2x2块的纹理坐标梯度，每个块计算一次）
	static void computeGradientForPixelQuad(const glm::vec3& bary00, const glm::vec3& bary10,
		const glm::vec3& bary01, const glm::vec3& bary11,
		const VertexShaderOutput& vo0,
		const VertexShaderOutput& vo1,
		const VertexShaderOutput& vo2,
		glm::vec2& texcoordGradX,
		glm::vec2& texcoordGradY,
		const glm::vec2& triangleGradX,
		const glm::vec2& triangleGradY);
};