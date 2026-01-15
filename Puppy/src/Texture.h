#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

class Texture
{
public:
	Texture() = default;

	bool loadFromFile(const std::string& fliepath);
	void createSolidColor(int w, int h, const glm::vec3& color);
	void createCheckerboard(int w, int h, int tileSize, 
		const glm::vec3& color1, const glm::vec3& color2);

	glm::vec3 sample(const glm::vec2& uv) const;
	glm::vec3 sample(float u, float v) const;
	glm::vec3 sampleBilinear(float u, float v) const;

	int getWidth() const { return width; }
	int getHeight() const { return height; }
	bool isValid() const { return data.empty(); }
private:
	int width = 0, height = 0, channels = 0;
	std::vector<uint8_t> data;

	glm::vec3 getPixel(int x, int y) const;
};