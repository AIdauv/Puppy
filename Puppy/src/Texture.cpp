#include "Texture.h"
#include <stb_image.h>
#include <iostream>

bool Texture::loadFromFile(const std::string& filepath) {
	unsigned char* imageData = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
	if (!imageData) {
		std::cerr << "纹理加载失败" << filepath << std::endl;
		return false;
	}

	size_t size = width * height * channels;
	data.resize(size);
	std::copy(imageData, imageData + size, data.begin());

	stbi_image_free(imageData);

	std::cout << "加载纹理: " << filepath << " (" << width << "x" << height
		<< ", " << channels << " channels)" << std::endl;

	return true;
}

void Texture::createSolidColor(int w, int h, const glm::vec3& color) {
	width = w;
	height = h;
	channels = 3;

	data.resize(w * h * 3);

	for (int i = 0; i < w * h; i++) {
		data[i * 3 + 0] = static_cast<uint8_t>(glm::clamp(color.r, 0.0f, 1.0f) * 255 + 0.5f);
		data[i * 3 + 1] = static_cast<uint8_t>(glm::clamp(color.g, 0.0f, 1.0f) * 255 + 0.5f);
		data[i * 3 + 2] = static_cast<uint8_t>(glm::clamp(color.b, 0.0f, 1.0f) * 255 + 0.5f);
	}
}

void Texture::createCheckerboard(int w, int h, int tileSize, 
	const glm::vec3& color1, const glm::vec3& color2) {

	width = w;
	height = h;
	channels = 3;

	data.resize(w * h * 3);

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			int tileX = x / tileSize;
			int tileY = y / tileSize;

			bool isEven = ((tileX + tileY) % 2) == 0;
			glm::vec3 color = isEven ? color1 : color2;

			int index = (y * w + x) * 3;

			data[index + 0] = static_cast<uint8_t>(glm::clamp(color.r, 0.0f, 1.0f) * 255 + 0.5f);
			data[index + 1] = static_cast<uint8_t>(glm::clamp(color.g, 0.0f, 1.0f) * 255 + 0.5f);
			data[index + 2] = static_cast<uint8_t>(glm::clamp(color.b, 0.0f, 1.0f) * 255 + 0.5f);
		}
	}
}

glm::vec3 Texture::sample(const glm::vec2& uv) const{
	return sample(uv.x, uv.y);
}

glm::vec3 Texture::sample(float u, float v) const {
	if (!isValid()) {
		return { 1, 0, 1 };  // 紫色表示纹理丢失
	}

	switch (sampleMode)
	{
	case Nearest:
		return sampleNearest(u, v, data, width, height);
	case Bilinear:
		return sampleBilinear(u, v, data, width, height);
	default:
		return { 1, 0, 1 };
	}
}

void Texture::generateMipmaps() {
	if (!isValid()) return;

	// 清空现有的Mipmap
	mipmaps.clear();
	mipWidths.clear();
	mipHeights.clear();

	// 第0级：原始纹理
	mipmaps.push_back(data);
	mipWidths.push_back(width);
	mipHeights.push_back(height);

	// 生成后续级别，直到尺寸为1x1
	int currentWidth = width;
	int currentHeight = height;
	int currentLevel = 0;

	while (currentWidth > 1 || currentHeight > 1) {
		generateMipmapLevel(currentLevel);

		currentLevel++;
		currentWidth = std::max(1, currentWidth / 2);
		currentHeight = std::max(1, currentHeight / 2);

		mipWidths.push_back(currentWidth);
		mipHeights.push_back(currentHeight);
	}

	mipLevels = static_cast<int>(mipmaps.size());
}

glm::vec3 Texture::sampleMipmap(float u, float v, float lod) const {
	if (!isValid() || mipmaps.empty()) {
		return { 1, 0, 1 };  // 紫色表示纹理丢失
	}

	lod = glm::clamp(lod, 0.0f, static_cast<float>(mipLevels - 1));

	int level0 = static_cast<int>(std::floor(lod));
	int level1 = std::min(level0 + 1, mipLevels - 1);
	
	float t = lod - level0;

	if (level0 == level1) {
		// 直接采样单个级别
		return sampleLevel(u, v, level0);
	}

	// 三线性过滤：在两个级别之间线性插值
	glm::vec3 color0 = sampleLevel(u, v, level0);
	glm::vec3 color1 = sampleLevel(u, v, level1);

	return glm::mix(color0, color1, t);
}

glm::vec3 Texture::sampleNearest(float u, float v, const std::vector<uint8_t>& data, int width, int height) const {

	// 最近邻
	int x = static_cast<int>(u * width) % width;
	int y = static_cast<int>((1.0f - v) * height) % height;

	x = glm::clamp(x, 0, width - 1);
	y = glm::clamp(y, 0, height - 1);

	return getPixel(x, y, width, data);
}

glm::vec3 Texture::sampleBilinear(float u, float v, const std::vector<uint8_t>& data, int width, int height) const {

	u = glm::fract(u);  // 取小数部分
	v = glm::fract(v);

	float x = u * width - 0.5f;  // 像素中心对其
	float y = (1.0f - v) * height - 0.5f;

	int x0 = static_cast<int>(std::floor(x));
	int x1 = x0 + 1;
	int y0 = static_cast<int>(std::floor(y));
	int y1 = y0 + 1;

	x0 = glm::clamp(x0, 0, width - 1);
	x1 = glm::clamp(x1, 0, width - 1);
	y0 = glm::clamp(y0, 0, height - 1);
	y1 = glm::clamp(y1, 0, height - 1);

	float fx = x - x0;
	float fy = y - y0;

	glm::vec3 p00 = getPixel(x0, y0, width, data);
	glm::vec3 p10 = getPixel(x1, y0, width, data);
	glm::vec3 p01 = getPixel(x0, y1, width, data);
	glm::vec3 p11 = getPixel(x1, y1, width, data);

	glm::vec3 clr0 = glm::mix(p00, p10, fx);
	glm::vec3 clr1 = glm::mix(p01, p11, fx);

	return glm::mix(clr0, clr1, fy);
}

void Texture::generateMipmapLevel(int sourceLevel) {
	const std::vector<uint8_t>& sourceData = mipmaps[sourceLevel];
	int srcWidth = mipWidths[sourceLevel];
	int srcHeight = mipHeights[sourceLevel];

	int dstWidth = std::max(1, srcWidth / 2);
	int dstHeight = std::max(1, srcHeight / 2);

	std::vector<uint8_t> dstData(dstWidth * dstHeight * channels);

	// 使用盒式过滤生成下一级Mipmap
	for (int y = 0; y < dstHeight; y++) {
		for (int x = 0; x < dstWidth; x++) {

			int srcX0 = x * 2;
			int srcY0 = y * 2;
			int srcX1 = std::min(srcX0 + 1, srcWidth - 1);
			int srcY1 = std::min(srcY0 + 1, srcHeight - 1);

			// 计算4个像素的平均值
			for (int c = 0; c < channels; c++) {

				int sum = 0;
				int count = 0;

				auto getChannel = [&](int px, int py) {
					int idx = (py * srcWidth + px) * channels + c;
					return sourceData[idx];
				};

				sum += getChannel(srcX0, srcY0);
				count++;

				// 排除在边界处srcX1与srcX0相等的情况，不计入均值计算，
				// 否则相当于(srcX0, srcY0)参与两次,下同
				if (srcX1 > srcX0) {
					sum += getChannel(srcX1, srcY0);
					count++;
				}
				
				if (srcY1 > srcY0) {
					sum += getChannel(srcX0, srcY1);
					count++;
				}

				if (srcX1 > srcX0 && srcY1 > srcY0) {
					sum += getChannel(srcX1, srcY1);
					count++;
				}

				int avg = sum / count;
				int dstIdx = (y * dstWidth + x) * channels + c;
				dstData[dstIdx] = static_cast<uint8_t>(avg);
			}
		}
	}

	mipmaps.push_back(dstData);
}

glm::vec3 Texture::sampleLevel(float u, float v, int level) const {
	if (level < 0 || level >= mipLevels) {
		return glm::vec3(0, 0, 0);
	}

	int w = mipWidths[level];
	int h = mipHeights[level];
	const std::vector<uint8_t>& levelData = mipmaps[level];

	return sampleBilinear(u, v, levelData, w, h);
	
}

glm::vec3 Texture::getPixel(int x, int y, int width, const std::vector<uint8_t>& data) const {
	int index = (y * width + x) * channels;

	if (channels >= 3) {
		return {
			data[index + 0] / 255.0f,
			data[index + 1] / 255.0f,
			data[index + 2] / 255.0f
		};
	}
	else if (channels == 1) {
		return {
			data[index] / 255.0f,
			data[index] / 255.0f,
			data[index] / 255.0f
		};
	}

	return glm::vec3(0, 0, 0);
}