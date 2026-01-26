#pragma once
#include <vector>
#include <glm/glm.hpp>

class MultisampleFramebuffer
{
public:
	MultisampleFramebuffer(int x, int y, int samples = 4);

	void clear(uint32_t color);
	void clearDepth();

	// MSAA操作
	bool sampleDepthTest(int x, int y, int sampleIndex, float depth);
	void setSample(int x, int y, int sampleIndex, const uint32_t& color);
	void setSample(int x, int y, int sampleIndex, const glm::vec3& color);
	void resolve();

	int getWidth() const { return width; }
	int getHeight() const { return height; }
	int getSamplesPerPixel() const { return samplesPerPixel; }
	uint32_t* getResolvedData() { return resolvedColor.data(); }
	int getPitch() { return width * sizeof(uint32_t); }
	const std::vector<glm::vec2>& getSampleOffsets() const { return sampleOffsets; }
private:
	int width;
	int height;
	int samplesPerPixel;

	// MSAA采样点的颜色与深度缓冲
	std::vector<uint32_t> colorSamples;
	std::vector<float> depthSamples;

	// 解析后的颜色缓冲
	std::vector<uint32_t> resolvedColor;

	// 采样点位置（相对于像素中心）
	std::vector<glm::vec2> sampleOffsets;

	void generateSampleOffsets();
	int getSampleIndex(int x, int y, int sampleIdx) const;
};