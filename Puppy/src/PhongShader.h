#pragma once

#include "Shader.h"

class PhongShader : public Shader
{
public:
	VertexShaderOutput vertexShader(const Vertex3D& vertex,
		const ShaderContext& context) const override;

	glm::vec3 fragmentShader(const FragmentShaderInput& fragment,
		const ShaderContext& context) const override;


	void setAmbientStrength(float strength) { ambientStrength = strength; }
	void setDiffuseStrength(float strength) { diffuseStrength = strength; }
	void setSpecularStrength(float strength) { specularStrength = strength; }
	void setShininess(float s) { shininess = s; }

private:
	float ambientStrength = 0.1f;
	float diffuseStrength = 0.8f;
	float specularStrength = 0.5f;
	float shininess = 32.0f;
};