#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>

void Model::createCube(float size) {
    clear();

    float half = size * 0.5f;

    // 直接创建8个位置顶点
    glm::vec3 positions[8] = {
        {-half, -half, half}, { half, -half, half},
        { half,  half, half}, {-half,  half, half},
        {-half, -half, -half}, { half, -half, -half},
        { half,  half, -half}, {-half,  half, -half}
    };

    // 创建6个面的三角形，直接复用位置，但创建不同的法线和纹理坐标
    auto createFace = [&](int p0, int p1, int p2, int p3,
        const glm::vec3& normal,
        const glm::vec3& tangent,
        const glm::vec2& tex0, const glm::vec2& tex1,
        const glm::vec2& tex2, const glm::vec2& tex3,
        const glm::vec3& color) {
            Vertex3D v0, v1, v2, v3;

            v0.position = positions[p0];
            v1.position = positions[p1];
            v2.position = positions[p2];
            v3.position = positions[p3];

            v0.normal = v1.normal = v2.normal = v3.normal = normal;
            v0.tangent = v1.tangent = v2.tangent = v3.tangent = tangent;

            v0.texcoord = tex0;
            v1.texcoord = tex1;
            v2.texcoord = tex2;
            v3.texcoord = tex3;

            v0.color = v1.color = v2.color = v3.color = color;

            // 添加两个三角形（第二个三角形复用v0和v2）
            addTriangle(v0, v1, v2);
            addTriangle(v0, v2, v3);
        };

    // 创建6个面
    createFace(0, 1, 2, 3, glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), // 前
        { 0,0 }, { 1,0 }, { 1,1 }, { 0,1 }, { 1,0,0 });

    createFace(5, 4, 7, 6, glm::vec3(0, 0, -1), glm::vec3(-1, 0, 0),  // 后
        { 0,0 }, { 1,0 }, { 1,1 }, { 0,1 }, { 0,1,0 });

    createFace(4, 0, 3, 7, glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1), // 左
        { 0,0 }, { 1,0 }, { 1,1 }, { 0,1 }, { 0,0,1 });

    createFace(1, 5, 6, 2, glm::vec3(1, 0, 0), glm::vec3(0, 0, -1),  // 右
        { 0,0 }, { 1,0 }, { 1,1 }, { 0,1 }, { 1,1,0 });

    createFace(4, 5, 1, 0, glm::vec3(0, -1, 0), glm::vec3(1, 0, 0), // 下
        { 0,0 }, { 1,0 }, { 1,1 }, { 0,1 }, { 1,0,1 });

    createFace(3, 2, 6, 7, glm::vec3(0, 1, 0), glm::vec3(1, 0, 0),  // 上
        { 0,0 }, { 1,0 }, { 1,1 }, { 0,1 }, { 0,1,1 });
}

void Model::createGround(float width, float depth, int repeatU, int repeatV) {
    clear();

    float hw = width * 0.5f;
    float hd = depth * 0.5f;
    glm::vec3 normal(0, 1, 0);
    glm::vec3 tangent(1, 0, 0);

    Vertex3D v0(glm::vec3(-hw, 0, -hd), glm::vec3(1), normal, tangent, glm::vec2(0, repeatV));
    Vertex3D v1(glm::vec3(hw, 0, -hd), glm::vec3(1), normal, tangent, glm::vec2(repeatU, repeatV));
    Vertex3D v2(glm::vec3(hw, 0, hd), glm::vec3(1), normal, tangent, glm::vec2(repeatU, 0));
    Vertex3D v3(glm::vec3(-hw, 0, hd), glm::vec3(1), normal, tangent, glm::vec2(0, 0));

    // 逆时针顺序，法线朝上
    addTriangle(v0, v2, v1);
    addTriangle(v0, v3, v2);
}

void Model::createPlane(float width, float height, const glm::vec3& normal,
    const glm::vec3& tangent, int repeatU, int repeatV) {
    clear();

    // 确保法线和切线正交（若用户传入非正交，自动修正）
    glm::vec3 n = glm::normalize(normal);
    glm::vec3 t = glm::normalize(tangent);

    // 如果 tangent 与 normal 平行（点积绝对值接近 1），则选择一个不同的默认切线
    if (glm::abs(glm::dot(n, t)) > 0.999f) {
        // 尝试用 (0,1,0) 作为备用
        t = glm::vec3(0, 1, 0);
        if (glm::abs(glm::dot(n, t)) > 0.999f) {
            // 如果法线也是 Y 轴，则用 (0,0,1)
            t = glm::vec3(0, 0, 1);
        }
    }

    // 修正切线：减去法线方向分量，再归一化
    t = glm::normalize(t - n * glm::dot(n, t));
    glm::vec3 b = glm::normalize(glm::cross(n, t));  // 副切线

    // 构建局部坐标系，四个顶点相对于中心偏移
    glm::vec3 center(0, 0, 0);
    glm::vec3 halfW = t * (width * 0.5f);
    glm::vec3 halfH = b * (height * 0.5f);

    Vertex3D v0(center - halfW - halfH, glm::vec3(1), n, t, glm::vec2(0, repeatV));
    Vertex3D v1(center + halfW - halfH, glm::vec3(1), n, t, glm::vec2(repeatU, repeatV));
    Vertex3D v2(center + halfW + halfH, glm::vec3(1), n, t, glm::vec2(repeatU, 0));
    Vertex3D v3(center - halfW + halfH, glm::vec3(1), n, t, glm::vec2(0, 0));

    addTriangle(v0, v1, v2);
    addTriangle(v0, v2, v3);
}

void Model::createSphere(float radius, int sectors, int stacks) {
    clear();

    std::vector<Vertex3D> vertices;
    // 生成顶点
    for (int i = 0; i <= stacks; ++i) {
        float v = (float)i / stacks;
        float theta = v * glm::pi<float>();          // 0 → π
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int j = 0; j <= sectors; ++j) {
            float u = (float)j / sectors;
            float phi = u * 2.0f * glm::pi<float>(); // 0 → 2π
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            float x = radius * sinTheta * cosPhi;
            float y = radius * cosTheta;
            float z = radius * sinTheta * sinPhi;

            glm::vec3 pos(x, y, z);
            glm::vec3 normal = glm::normalize(pos);   // 球心在原点的法线
            glm::vec2 texcoord(u, v);

            vertices.emplace_back(pos, glm::vec3(1.0f), normal, glm::vec3(1, 0, 0), texcoord);
        }
    }

    // 生成三角形索引（两个三角形构成一个四边形格）
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < sectors; ++j) {
            int a = i * (sectors + 1) + j;
            int b = i * (sectors + 1) + j + 1;
            int c = (i + 1) * (sectors + 1) + j;
            int d = (i + 1) * (sectors + 1) + j + 1;

            addTriangle(vertices[a], vertices[b], vertices[c]);
            addTriangle(vertices[b], vertices[d], vertices[c]);
        }
    }
}

glm::mat4 Model::getModelMatrix() const {
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, position);

	// 顺序：Yaw->Pitch->Roll 绕世界坐标系的坐标轴旋转
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));

	model = glm::scale(model, scale);

	return model;
}


void Model::clear() {
	triangles.clear();
}

void Model::addTriangle(const Vertex3D& v0, const Vertex3D& v1, const Vertex3D& v2) {
	triangles.push_back(Triangle3D(v0, v1, v2));
}