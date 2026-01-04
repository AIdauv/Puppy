#pragma once
#include <glm/glm.hpp>


struct Vertex3D
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;

	Vertex3D() : position(0, 0, 0), color(1, 1, 1), normal(0, 1, 0) {}
	Vertex3D(glm::vec3 pos, glm::vec3 clr) : position(pos), color(clr), normal(0, 1, 0) {}
};

struct Triangle3D
{
	Vertex3D v0, v1, v2;

	Triangle3D() = default;
	Triangle3D(const Vertex3D& a, const Vertex3D& b, const Vertex3D& c) 
		: v0(a), v1(b), v2(c) {}
};