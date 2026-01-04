#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera();

	void setPosition(glm::vec3 pos);
	void setTarget(glm::vec3 tgt); 
	void setUpVector(glm::vec3 upVec);
	void setPerspective(float n, float f, float fovDegrees, float aspect);

	void moveForward(float distance);	
	void moveUp(float distance);
	void moveRight(float distance);
	void rotate(float yaw, float pitch);

	const glm::mat4& getViewMatrix();
	const glm::mat4& getProjectionMatrix();
	glm::mat4 getViewProjectionMatrix();

	glm::vec3 getPosition() { return position; }
	glm::vec3 getForward() { return glm::normalize(target - position); }
	glm::vec3 getUp() { return glm::normalize(up); }
	glm::vec3 getRight() { return glm::normalize(glm::cross(getForward(), up)); }

private:
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 up;

	float near;
	float far;
	float fov;
	float aspectRatio;

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	bool needUpdate;

	void updateMatrices();
};