#pragma once

#include "Light.h"
#include "Texture.h"
#include "Vertex.h"
#include <glm/glm.hpp>

struct ShaderContext
{
	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 viewportMatrix;

	Light light;
	glm::vec3 cameraPos;

	Texture* texture = nullptr;

	bool usePerspective = true;
	bool useTexture = true;
	bool useLight = true;
};

class Shader 
{
public:
	virtual ~Shader() = default;

	virtual Vertex3DTransformed vertexShader(const Vertex3D& vertex, const ShaderContext& context) = 0;

	virtual glm::vec3 fragmentShader(const Vertex3DTransformed& fragment, const ShaderContext& context) = 0;

    
    // 插值辅助函数（透视校正）
    template<typename T>
    static T interpolateAttribute(const T& attr0, const T& attr1, const T& attr2,
        const glm::vec3& barycentric,
        float oneOverW0, float oneOverW1, float oneOverW2,
        float interpolatedOneOverW,
        bool usePerspectiveCorrection = true) {

        if (usePerspectiveCorrection) {
            // 透视校正插值：先插值 attribute/w，再除以 1/w
            auto attrOverW = barycentric.x * (attr0 * oneOverW0) +
                barycentric.y * (attr1 * oneOverW1) +
                barycentric.z * (attr2 * oneOverW2);
            return attrOverW / interpolatedOneOverW;
        }
        else {
            // 简单线性插值
            return barycentric.x * attr0 + barycentric.y * attr1 + barycentric.z * attr2;
        }
    }
};