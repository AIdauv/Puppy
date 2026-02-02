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