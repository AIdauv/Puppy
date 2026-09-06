#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

enum SampleMode
{
	Nearest,
	Bilinear
};

class Texture
{
public:
	Texture() = default;

	bool loadFromFile(const std::string& fliepath);
	void createSolidColor(int w, int h, const glm::vec3& color);
	void createCheckerboard(int w, int h, int tileSize, 
		const glm::vec3& color1, const glm::vec3& color2);
	void createStripes(int w, int h, int stripeWidth, 
		const glm::vec3& color1, const glm::vec3& color2, bool vertical = true);
	void createFractalCheckerboard(int w, int h, int tileSize, int subDivisions,
		const glm::vec3& color1, const glm::vec3& color2,
		const glm::vec3& color3);

	glm::vec3 sample(const glm::vec2& uv) const;
	glm::vec3 sample(float u, float v) const;

	// …˙≥…mipmap¡¥
	void generateMipmaps();
	glm::vec3 sampleMipmap(float u, float v, float lod) const;

	// º∆À„LOD
	float calculateLOD(const glm::vec2& dTdx, const glm::vec2& dTdy, const glm::vec2& textureSize) const;

	int getWidth() const { return width; }
	int getHeight() const { return height; }
	int getMipLevels() const { return mipLevels; }

	void setSampleMode(SampleMode mode) { sampleMode = mode; }
	
	bool isValid() const { return !data.empty(); }
private:
	int width = 0, height = 0, channels = 0;
	SampleMode sampleMode = Bilinear;
	std::vector<uint8_t> data;

	std::vector<std::vector<uint8_t>> mipmaps;
	std::vector<int> mipWidths;
	std::vector<int> mipHeights;
	int mipLevels = 0;

	glm::vec3 getPixel(int x, int y, int width, int channels, const std::vector<uint8_t>& data) const;

	glm::vec3 sampleNearest(float u, float v, const std::vector<uint8_t>& data, int width, int height, int channels) const;
	glm::vec3 sampleBilinear(float u, float v, const std::vector<uint8_t>& data, int width, int height, int channels) const;

	void generateMipmapLevel(int sourceLevel);
	glm::vec3 sampleLevel(float u, float v, int level) const;
};