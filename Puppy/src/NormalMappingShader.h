#pragma once

#include "Shader.h"

// 计算模式
enum NormalMode
{
    VERTEX_NORMALS = 0,    // 仅使用顶点法线
    NORMAL_MAP = 1,        // 仅使用法线贴图
    BUMP_MAP = 2,          // 从高度图实时计算法线
    HYBRID = 3             // 混合模式
};

class NormalMappingShader : public Shader 
{
public:
    VertexShaderOutput vertexShader(const Vertex3D& vertex,
        const ShaderContext& context) const override;

    glm::vec3 fragmentShader(const FragmentShaderInput& fragment,
        const ShaderContext& context) const override;

    // 设置参数
    void setNormalMode(NormalMode mode) { normalMode = mode; }
    void setNormalMapStrength(float strength) { normalMapStrength = strength; }
    void setBumpMapStrength(float strength) { bumpMapStrength = strength; }
    void setBumpScale(float scale) { bumpScale = scale; }
    void setBumpStrength(float strength) { bumpStrength = strength; }
    void setHybridWeight(float weight) { hybridWeight = weight; }

    void setAmbientStrength(float s) { ambientStrength = s; }
    void setDiffuseStrength(float s) { diffuseStrength = s; }
    void setSpecularStrength(float s) { specularStrength = s; }
    void setShininess(float s) { shininess = s; }

    // 获取参数
    NormalMode getNormalMode() const { return normalMode; }
    float getNormalMapStrength() const { return normalMapStrength; }
    float getBumpMapStrength() const { return bumpMapStrength; }
private:
    NormalMode normalMode = NORMAL_MAP;
    float normalMapStrength = 1.0f;     // 法线贴图强度
    float bumpMapStrength = 1.0f;       // 凹凸贴图强度
    float bumpScale = 0.1f;             // 凹凸缩放
    float bumpStrength = 10.0f;         // 凹凸强度
    float hybridWeight = 0.5f;          // 混合权重

    // 光照参数
    float ambientStrength = 0.1f;
    float diffuseStrength = 0.8f;
    float specularStrength = 0.5f;
    float shininess = 32.0f;

    // 法线计算辅助函数
    glm::vec3 calculateNormal(const FragmentShaderInput& fragment,
        const ShaderContext& context) const;

    glm::vec3 calculateNormalFromMap(const FragmentShaderInput& fragment,
        const ShaderContext& context) const;

    glm::vec3 calculateNormalFromBumpMap(const FragmentShaderInput& fragment,
        const ShaderContext& context) const;

    glm::vec3 calculateNormalHybrid(const FragmentShaderInput& fragment,
        const ShaderContext& context) const;

    // 光照计算
    glm::vec3 computeLighting(const glm::vec3& baseColor,
        const glm::vec3& normal,
        const FragmentShaderInput& fragment,
        const ShaderContext& context) const;
};