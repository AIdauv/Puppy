#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Framebuffer.h"

class Rasterizer 
{
public:
	Rasterizer(Framebuffer& fb)
		: framebuffer(fb) { }

	void drawTriangle2D(const glm::vec2& v0, const glm::vec2& v1,
						const glm::vec2& v2, const glm::vec3& color);


private:
	Framebuffer& framebuffer;

	bool isInsideTriangle(const glm::vec2& p, const glm::vec2& v0,
							const glm::vec2& v1, const glm::vec2& v2);
	void getBoundingBox(const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2, 
						int& minX, int& minY, int& maxX, int& maxY);

};