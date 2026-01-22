#pragma once
#include <glm/glm.hpp>


struct Vertex3D
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 texcoord;

	Vertex3D() 
		: position(0, 0, 0), color(1, 1, 1), normal(0, 1, 0), texcoord(0, 0) {}
	Vertex3D(glm::vec3 pos, glm::vec3 clr) 
		: position(pos), color(clr), normal(0, 1, 0), texcoord(0, 0) {}
	Vertex3D(glm::vec3 pos, glm::vec3 clr, glm::vec3 norm, glm::vec2 tex) 
		: position(pos), color(clr), normal(norm), texcoord(tex) {}

};

struct VertexShaderOutput
{
	glm::vec4 clipPos;  // 裁剪空间的齐次坐标（透视除法前）
	glm::vec3 worldPos;  // 世界坐标
	glm::vec3 color;
	glm::vec3 worldNorm;  // 世界空间法线
	glm::vec2 texcoord;

	float oneOverW;  // 1/w，用于透视矫正

	VertexShaderOutput()
		: clipPos(0, 0, 0, 1), 
		worldPos(0, 0, 0), 
		color(1, 1, 1), 
		worldNorm(0, 1, 0), 
		texcoord(0, 0), 
		oneOverW(1.0f) {}
};

struct FragmentShaderInput {
	glm::vec3 worldPos;    
	glm::vec3 color;       
	glm::vec3 worldNorm;      
	glm::vec2 texcoord;    

	FragmentShaderInput() 
		: worldPos(0, 0, 0), 
		color(1, 1, 1), 
		worldNorm(0, 1, 0), 
		texcoord(0, 0) {}
};

struct Triangle3D
{
	Vertex3D v0, v1, v2;

	Triangle3D() = default;
	Triangle3D(const Vertex3D& a, const Vertex3D& b, const Vertex3D& c) 
		: v0(a), v1(b), v2(c) {}
};