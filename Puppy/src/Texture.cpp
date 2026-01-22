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

	// 最近邻
	int x = static_cast<int>(u * width) % width;
	int y = static_cast<int>((1.0f - v) * height) % height;

	x = glm::clamp(x, 0, width - 1);
	y = glm::clamp(y, 0, height - 1);

	return getPixel(x, y);
}

glm::vec3 Texture::sampleBilinear(float u, float v) const {
	if (!isValid()) {
		return glm::vec3(1, 0, 1);
	}

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

	glm::vec3 p00 = getPixel(x0, y0);
	glm::vec3 p10 = getPixel(x1, y0);
	glm::vec3 p01 = getPixel(x0, y1);
	glm::vec3 p11 = getPixel(x1, y1);

	glm::vec3 clr0 = glm::mix(p00, p10, fx);
	glm::vec3 clr1 = glm::mix(p01, p11, fx);

	return glm::mix(clr0, clr1, fy);
}

glm::vec3 Texture::getPixel(int x, int y) const {
	int index = (y * width + x) * channels;

	if (channels >= 3) {
		return {
			data[index + 0] / 255.0f,
			data[index + 1] / 255.0f,
			data[index + 2] / 255.0f
		};
	} else if(channels == 1) {
		return {
			data[index] / 255.0f,
			data[index] / 255.0f,
			data[index] / 255.0f
		};
	}

	return glm::vec3(0, 0, 0);
}