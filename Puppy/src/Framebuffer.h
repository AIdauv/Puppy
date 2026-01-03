#pragma once

#include <vector>
#include <glm/glm.hpp>

class Framebuffer 
{
public:
	Framebuffer(int x, int y)
		: width(x), height(y) { 

		colorbuffer.resize(width * height);
	}

	void clear(uint32_t color);
	void setPixel(int x, int y, uint32_t color);
	void setPixel(int x, int y, glm::vec3 color);

	int getWidth() { return width; }
	int getHeight() { return height; }
	uint32_t* getData() { return colorbuffer.data(); }
	int getPitch() { return width * sizeof(uint32_t); }

	static uint32_t colorToUint32(glm::vec3 color);
	static glm::vec3 uint32ToColor(uint32_t color);
private:
	int width, height;
	std::vector<uint32_t> colorbuffer;
};