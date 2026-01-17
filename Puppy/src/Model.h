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