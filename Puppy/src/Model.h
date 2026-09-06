#pragma once

#include "Vertex.h"
#include <vector>
#include <string>

class Model
{
public:
	Model() : position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1) {}

	// TODO : 加载OBJ文件
	// bool loadFromOBJ(const std::string& filepath);

	void createCube(float size = 1.0f);
	void createGround(float width, float depth, int repeatU = 1, int repeatV = 1);
	void createPlane(float width, float height, const glm::vec3& normal,
		const glm::vec3& tangent = glm::vec3(1, 0, 0),
		int repeatU = 1, int repeatV = 1);
	void createSphere(float radius, int sectors, int stacks);

	const std::vector<Triangle3D>& getTriangles() { return triangles; }

	glm::mat4 getModelMatrix() const;

	void setPosition(const glm::vec3& pos) { position = pos; }
	void setRotation(const glm::vec3& rot) { rotation = rot; }
	void setScale(const glm::vec3& scl) { scale = scl; }

	void clear();
private:
	// 暂时使用三角形列表
	std::vector<Triangle3D> triangles;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	void addTriangle(const Vertex3D& v0, const Vertex3D& v1, const Vertex3D& v2);
};