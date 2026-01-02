#include "Framebuffer.h"

uint32_t Framebuffer::colorToUint32(glm::vec3 color) {

	uint8_t r = static_cast<uint8_t>(glm::clamp(color.r, 0.0f, 1.0f) * 255 + 0.5f);
	uint8_t g = static_cast<uint8_t>(glm::clamp(color.g, 0.0f, 1.0f) * 255 + 0.5f);
	uint8_t b = static_cast<uint8_t>(glm::clamp(color.b, 0.0f, 1.0f) * 255 + 0.5f);

	return (255 << 24) | (r << 16) | (g << 8) | b;
}

glm::vec3 Framebuffer::uint32ToColor(uint32_t color) {

	float r = ((color >> 16) & 0xff) / 255.0f;
	float g = ((color >> 8) & 0xff) / 255.0f;
	float b = (color & 0xff) / 255.0f;

	return glm::vec3(r, g, b);
}

void Framebuffer::clear(uint32_t color) {
	std::fill(colorbuffer.begin(), colorbuffer.end(), color);
}

void Framebuffer::setPixel(int x, int y, uint32_t color) {
	if (x >= 0 && x < width && y >= 0 && y < height) {
		colorbuffer[y * width + x] = color;
	}
}

void Framebuffer::setPixel(int x, int y, glm::vec3 color) {
	setPixel(x, y, colorToUint32(color));
}