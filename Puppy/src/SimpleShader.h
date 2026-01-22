#pragma once

#include "Shader.h"

class SimpleShader : public Shader {
public:
	VertexShaderOutput vertexShader(const Vertex3D& vertex, 
		const ShaderContext& context) const override;

	glm::vec3 fragmentShader(const FragmentShaderInput& fragment, 
		const ShaderContext& context) const override;
};

class TextureShader : public Shader {
public:
	VertexShaderOutput vertexShader(const Vertex3D& vertex,
		const ShaderContext& context) const override;

	glm::vec3 fragmentShader(const FragmentShaderInput& fragment,
		const ShaderContext& context) const override;
};