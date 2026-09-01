#include "NormalMappingShader.h"

VertexShaderOutput NormalMappingShader::vertexShader(const Vertex3D& vertex,
    const ShaderContext& context) const {

    VertexShaderOutput result;

    glm::vec4 worldPos = context.modelMatrix * glm::vec4(vertex.position, 1.0f);
    result.worldPos = glm::vec3(worldPos);

    glm::vec4 clipPos = context.projectionMatrix * context.viewMatrix * worldPos;
    result.clipPos = clipPos;

    result.oneOverW = 1.0f / clipPos.w;

    // 变换法线（需要模型矩阵的逆转置）、切线、到世界坐标
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(context.modelMatrix)));
    
    result.worldNorm = glm::normalize(normalMatrix * vertex.normal);
    result.worldTangent = glm::normalize(glm::mat3(context.modelMatrix) * vertex.tangent);

    result.texcoord = vertex.texcoord;
    result.color = vertex.color;

    return result;
}

glm::vec3 NormalMappingShader::fragmentShader(const FragmentShaderInput& fragment,
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

    // 计算法线（根据模式选择）
    glm::vec3 normal = calculateNormal(fragment, context);

    // 计算光照
    return computeLighting(baseColor, normal, fragment, context);
}

glm::vec3 NormalMappingShader::calculateNormal(const FragmentShaderInput& fragment,
    const ShaderContext& context) const {

    switch (normalMode) {
    case VERTEX_NORMALS:
        return glm::normalize(fragment.worldNorm);
    case NORMAL_MAP:
        return calculateNormalFromMap(fragment, context);
    case BUMP_MAP:
        return calculateNormalFromBumpMap(fragment, context);
    case HYBRID:
        return calculateNormalHybrid(fragment, context);
    default:
        return glm::normalize(fragment.worldNorm);
    }

}

glm::vec3 NormalMappingShader::calculateNormalFromMap(const FragmentShaderInput& fragment,
    const ShaderContext& context) const {

    if (!context.normalMap) {
        return glm::normalize(fragment.worldNorm);
    }

    // 采样法线贴图
    glm::vec3 tangentNormal;
    if (context.useMipmapForNormalMap && context.normalMap->getMipLevels() > 0) {
        // 计算LOD
        glm::vec2 normalMapSize = {
            context.normalMap->getWidth(),
            context.normalMap->getHeight()
        };

        float lod = context.normalMap->calculateLOD(
            fragment.texcoordGradX,
            fragment.texcoordGradY,
            normalMapSize
        );

        lod += context.lodBias;

        // 使用Mipmap采样
        tangentNormal = context.normalMap->sampleMipmap(
            fragment.texcoord.x,
            fragment.texcoord.y,
            lod
        );
    }
    else {
        tangentNormal = context.normalMap->sample(fragment.texcoord.x, fragment.texcoord.y);
    }

    // 从[0,1]颜色空间映射到[-1,1]法线空间
    // （纹理值原先用来存储rgb，都为正数，但现在用于存储法线分量，有负数，所以0.5映射到0）
    tangentNormal = tangentNormal * 2.0f - 1.0f;

    // 转换到世界空间
    glm::vec3 worldNormal = glm::normalize(fragment.TBN * tangentNormal);

    // 应用强度控制
    if (normalMapStrength < 1.0f) {
        worldNormal = glm::normalize(glm::mix(
            glm::normalize(fragment.worldNorm),
            worldNormal,
            normalMapStrength));
    }

    return worldNormal;
}

glm::vec3 NormalMappingShader::calculateNormalFromBumpMap(const FragmentShaderInput& fragment,
    const ShaderContext& context) const {

    if (!context.heightMap) {
        return glm::normalize(fragment.worldNorm);
    }

    // 高度图采样辅助函数（暂不支持Mipmap）
    auto getHeight = [&](float u, float v) -> float {
        glm::vec3 sample = context.heightMap->sample(u, v);
        return (sample.r + sample.g + sample.b) / 3.0f;
        };

    // 使用屏幕空间梯度确定采样偏移（获取周围像素的uv）
    glm::vec2 gradX = fragment.texcoordGradX;
    glm::vec2 gradY = fragment.texcoordGradY;

    float u = fragment.texcoord.x;
    float v = fragment.texcoord.y;

    // 采样高度值
    float h_center = getHeight(u, v);
    float h_right = getHeight(u + gradX.x, v + gradX.y);
    float h_left = getHeight(u - gradX.x, v - gradX.y);
    float h_down = getHeight(u + gradY.x, v + gradY.y);
    float h_up = getHeight(u - gradY.x, v - gradY.y);

    // 计算梯度（中心差分）
    float dU = h_right - h_left * 0.5f * bumpStrength;
    float dV = h_down - h_up * 0.5f * bumpStrength;

    // 构造切线向量并叉乘得到法线（切线空间）
    glm::vec3 tangentU = glm::vec3(1.0f, 0.0f, dU * bumpScale);
    glm::vec3 tangentV = glm::vec3(0.0f, 1.0f, dV * bumpScale);
    glm::vec3 tangentNormal = glm::normalize(glm::cross(tangentU, tangentV));

    // 转换到世界空间
    glm::vec3 worldNormal = glm::normalize(fragment.TBN * tangentNormal);

    // 应用强度控制
    if (bumpMapStrength < 1.0f) {
        worldNormal = glm::normalize(glm::mix(
            glm::normalize(fragment.worldNorm),
            worldNormal,
            bumpMapStrength));
    }

    return worldNormal;
}

glm::vec3 NormalMappingShader::calculateNormalHybrid(const FragmentShaderInput& fragment,
    const ShaderContext& context) const {

    glm::vec3 normalFromMap = calculateNormalFromMap(fragment, context);
    glm::vec3 normalFromBump = calculateNormalFromBumpMap(fragment, context);

    // 混合两种法线
    glm::vec3 hybridNormal = glm::normalize(glm::mix(
        normalFromMap,
        normalFromBump,
        hybridWeight));

    return hybridNormal;
}

glm::vec3 NormalMappingShader::computeLighting(const glm::vec3& baseColor,
    const glm::vec3& normal,
    const FragmentShaderInput& fragment,
    const ShaderContext& context) const {

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