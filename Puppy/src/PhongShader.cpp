#include "PhongShader.h"

VertexShaderOutput PhongShader::vertexShader(const Vertex3D& vertex,
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

glm::vec3 PhongShader::fragmentShader(const FragmentShaderInput& fragment,
	const ShaderContext& context) const {

	glm::vec3 baseColor;

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
			baseColor = context.texture->sampleMipmap(
				fragment.texcoord.x,
				fragment.texcoord.y,
				lod
			);
		}
		else {
			baseColor = context.texture->sample(fragment.texcoord.x, fragment.texcoord.y);
		}
	}
	else {
		baseColor = fragment.color;
	}

	if (!context.useLight) {
		return baseColor;
	}


	glm::vec3 normal = glm::normalize(fragment.worldNorm);  // 插值后需要重新归一化
	glm::vec3 lightDir = glm::normalize(context.light.position - fragment.worldPos);
	glm::vec3 viewDir = glm::normalize(context.cameraPos - fragment.worldPos);

	// Blinn-Phong核心改进：使用半程向量替代反射向量
	glm::vec3 halfDir = glm::normalize(lightDir + viewDir);

	// 环境光（不衰减）
	glm::vec3 ambient = ambientStrength * context.light.color * context.light.intensity;

	// 漫反射
	float diff = std::max(0.0f, glm::dot(normal, lightDir));
	glm::vec3 diffuse = diffuseStrength * context.light.color * context.light.intensity * diff;

	// Blinn-Phong镜面反射（高光）
	float spec = std::pow(std::max(0.0f, glm::dot(normal, halfDir)), shininess);
	glm::vec3 specular = specularStrength * context.light.color * context.light.intensity * spec;

	// 计算衰减
	float distance = glm::length(context.light.position - fragment.worldPos);
	float attenuation = context.light.computeAttenuation(distance);

	// 限制最小衰减值（避免过暗）
	attenuation = std::max(attenuation, 0.05f);

	// 组合光照分量：环境光不衰减，漫反射和镜面反射衰减
	glm::vec3 result = (ambient + (diffuse + specular) * attenuation) * baseColor;

	return glm::clamp(result, 0.0f, 1.0f);
}