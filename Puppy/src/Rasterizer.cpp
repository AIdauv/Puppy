#include "Rasterizer.h"

bool Rasterizer::isInsideTriangle(const glm::vec2& p, const glm::vec2& v0,
	const glm::vec2& v1, const glm::vec2& v2) {

	auto cross = [](const glm::vec2& a, const glm::vec2& b) {
		return a.x * b.y - a.y * b.x;
		};

	glm::vec2 edge0 = v1 - v0;
	glm::vec2 edge1 = v2 - v1;
	glm::vec2 edge2 = v0 - v2;

	glm::vec2 v0p = p - v0;
	glm::vec2 v1p = p - v1;
	glm::vec2 v2p = p - v2;

	float c0 = cross(edge0, v0p);
	float c1 = cross(edge1, v1p);
	float c2 = cross(edge2, v2p);

	return (c0 >= 0 && c1 >= 0 && c2 >= 0) || (c0 <= 0 && c1 <= 0 && c2 <= 0);
}

void Rasterizer::getBoundingBox(const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2,
	int& minX, int& minY, int& maxX, int& maxY) {

	minX = static_cast<int>(std::floor(std::min(v0.x, v1.x, v2.x)));
	minY = static_cast<int>(std::floor(std::min(v0.y, v1.y, v2.y)));
	maxX = static_cast<int>(std::ceil(std::max(v0.x, v1.x, v2.x)));
	maxY = static_cast<int>(std::ceil(std::max(v0.y, v1.y, v2.y)));

	minX = std::max(0, minX);
	minY = std::max(0, minY);
	maxX = std::min(framebuffer.getWidth()-1, maxX);
	maxY = std::min(framebuffer.getHeight()-1, maxY);

}

void Rasterizer::drawTriangle2D(const glm::vec2& v0, const glm::vec2& v1,
	const glm::vec2& v2, const glm::vec3& color) {

	int minX, minY, maxX, maxY;

	getBoundingBox(v0, v1, v2, minX, minY, maxX, maxY);

	for (int y = minY; y <= maxY; y++) {
		for (int x = minX; x <= maxX; x++) {
			float px = x + 0.5f;
			float py = y + 0.5f;

			if (isInsideTriangle(glm::vec2(px, py), v0, v1, v2)) {
				framebuffer.setPixel(x, y, color);
			}
		}
	}

}
