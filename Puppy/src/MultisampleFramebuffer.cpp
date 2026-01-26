#include "MultisampleFramebuffer.h"
#include "Framebuffer.h"
#include <random>

MultisampleFramebuffer::MultisampleFramebuffer(int x, int y, int samples) 
	: width(x), height(y), samplesPerPixel(samples) {

	int totalSamples = width * height * samplesPerPixel;

	colorSamples.resize(totalSamples);
	depthSamples.resize(totalSamples, 1.0f);
	resolvedColor.resize(width * height);

	generateSampleOffsets();
}

void MultisampleFramebuffer::generateSampleOffsets() {
	if (samplesPerPixel == 1) {
		// 无MSAA
		sampleOffsets.push_back({ 0.0f, 0.0f });
	}
	else if (samplesPerPixel == 4)
	{
		// 4xMSAA：经典的旋转网格模式
		float offsets[4][2] = {
			{-0.125f, -0.375f},
			{ 0.375f, -0.125f},
			{-0.375f,  0.125f},
			{ 0.125f,  0.375f}
		};

		for (int i = 0; i < 4; i++) {
			sampleOffsets.push_back({ offsets[i][0], offsets[i][1] });
		}
	}
	else if (samplesPerPixel == 8)
	{
		// 8xMSAA 优化过的固定8点采样（NVIDIA-style）
		float offsets[8][2] = {
		{0.0625f, -0.1875f},   // 1
		{-0.0625f, 0.1875f},   // 2
		{0.3125f, 0.0625f},    // 3
		{-0.1875f, -0.3125f},  // 4
		{-0.3125f, 0.3125f},   // 5
		{0.1875f, 0.4375f},    // 6
		{-0.4375f, -0.0625f},  // 7
		{0.4375f, -0.4375f}    // 8
		};

		for (int i = 0; i < 8; i++) {
			sampleOffsets.push_back(glm::vec2(offsets[i][0], offsets[i][1]));
		}
	}
}

int MultisampleFramebuffer::getSampleIndex(int x, int y, int sampleIdx) const {
	return (y * width + x) * samplesPerPixel + sampleIdx;
}

void MultisampleFramebuffer::clear(uint32_t color) {
	std::fill(colorSamples.begin(), colorSamples.end(), color);
	std::fill(resolvedColor.begin(), resolvedColor.end(), color);
}

void MultisampleFramebuffer::clearDepth() {
	std::fill(depthSamples.begin(), depthSamples.end(), 1.0f);
}

bool MultisampleFramebuffer::sampleDepthTest(int x, int y, int sampleIndex, float depth) {
	int idx = getSampleIndex(x, y, sampleIndex);

	if (depth < depthSamples[idx]) {
		depthSamples[idx] = depth;
		return true;
	}

	return false;
}

void MultisampleFramebuffer::setSample(int x, int y, int sampleIndex, const uint32_t& color) {
	int idx = getSampleIndex(x, y, sampleIndex);
	colorSamples[idx] = color;
}

void MultisampleFramebuffer::setSample(int x, int y, int sampleIndex, const glm::vec3& color) {
	setSample(x, y, sampleIndex, Framebuffer::colorToUint32(color));
}

void MultisampleFramebuffer::resolve() {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			glm::vec3 accumulatedColor(0.0f);

			// 平均所有采样点的颜色
			for (int s = 0; s < samplesPerPixel; s++) {
				int idx = getSampleIndex(x, y, s);
				accumulatedColor += Framebuffer::uint32ToColor(colorSamples[idx]);
			}

			resolvedColor[y * width + x] = Framebuffer::colorToUint32(accumulatedColor / float(samplesPerPixel));
		}
	}
}