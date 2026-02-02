#include "SimpleShader.h"

VertexShaderOutput SimpleShader::vertexShader(const Vertex3D& vertex,
	const ShaderContext& context) const {

	VertexShaderOutput result;

	glm::vec4 worldPos = context.modelMatrix * glm::vec4(vertex.position, 1.0f);
	result.worldPos = glm::vec3(worldPos);

	glm::vec4 clipPos = context.projectionMatrix * context.viewMatrix * worldPos;
	result.clipPos = clipPos;

	result.oneOverW = 1.0f / clipPos.w;

	// 变换法线到世界坐标（法线变换需要模型矩阵的逆转置）
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(context.modelMatrix)));
	result.worldNorm = glm::normalize(normalMatrix * vertex.normal);

	result.texcoord = vertex.texcoord;
	result.color = vertex.color;

	return result;
}

glm::vec3 SimpleShader::fragmentShader(const FragmentShaderInput& fragment,
	const ShaderContext& context) const {

	return fragment.color;
}

VertexShaderOutput TextureShader::vertexShader(const Vertex3D& vertex,
	const ShaderContext& context) const {

	VertexShaderOutput result;

	glm::vec4 worldPos = context.modelMatrix * glm::vec4(vertex.position, 1.0f);
	result.worldPos = glm::vec3(worldPos);

	glm::vec4 clipPos = context.projectionMatrix * context.viewMatrix * worldPos;
	result.clipPos = clipPos;

	result.oneOverW = 1.0f / clipPos.w;

	// 变换法线到世界坐标（法线变换需要模型矩阵的逆转置）
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(context.modelMatrix)));
	result.worldNorm = glm::normalize(normalMatrix * vertex.normal);

	result.texcoord = vertex.texcoord;
	result.color = vertex.color;

	return result;
}

glm::vec3 TextureShader::fragmentShader(const FragmentShaderInput& fragment,
	const ShaderContext& context) const {

	if (context.texture && context.useTexture) {
		if (context.useMipmap && context.texture->getMipLevels() > 1) {
			// 计算LOD
			glm::vec2 textureSize = {
				context.texture->getWidth(),
				context.texture->getHeight()
			};

			float lod = context.texture->calculateLOD(
				fragment.texcoordGradX,
				fragment.texcoordGradY,
				textureSize
			);

			lod += context.lodBias;

			// 使用Mipmap采样
			return context.texture->sampleMipmap(
				fragment.texcoord.x,
				fragment.texcoord.y,
				lod
			);
		}
		else {
			return context.texture->sample(fragment.texcoord.x, fragment.texcoord.y);
		}
	}

	return fragment.color;
}