#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

Camera::Camera()
	: position(0, 0, 5),
	  target(0, 0, 0),
	  up(0, 1, 0),
	  near(0.1f),
	  far(100.0f),
	  fov(45.0f),
	  aspectRatio(4.0f/3.0f),
	  needUpdate(true) {
}

void Camera::updateMatrices() {
	if (!needUpdate)
		return;

	viewMatrix = glm::lookAt(position, target, up);
	projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, near, far);
	needUpdate = false;
}

void Camera::setPosition(glm::vec3 pos) {
	position = pos;
	needUpdate = true;
}

void Camera::setTarget(glm::vec3 tgt) {
	target = tgt;
	needUpdate = true;
}

void Camera::setUpVector(glm::vec3 upVec) {
	up = upVec;
	needUpdate = true;
}

void Camera::setPerspective(float n, float f, float fovDegrees, float aspect) {
	near = n;
	far = f;
	fov = fovDegrees;
	aspectRatio = aspect;
	needUpdate = true;
}

void Camera::moveForward(float distance) {
	position += getForward() * distance;
	target += getForward() * distance;
	needUpdate = true;
}

void Camera::moveUp(float distance) {
	position += getUp() * distance;
	target += getUp() * distance;
	needUpdate = true;
}

void Camera::moveRight(float distance) {
	position += getRight() * distance;
	target += getRight() * distance;
	needUpdate = true;
}

void Camera::rotate(float yaw, float pitch) {
	pitch = glm::clamp(pitch, -85.0f, 85.0f);

	float yawRad = glm::radians(yaw);
	float pitchRad = glm::radians(pitch);

	glm::vec3 forward = getForward();
	glm::vec3 right = getRight();

	glm::quat yawQuat = glm::angleAxis(yawRad, up);
	glm::quat pitchQuat = glm::angleAxis(pitchRad, right);

	glm::quat totalRot = pitchQuat * yawQuat;

	glm::vec3 newForward = totalRot * forward;
	target = position + newForward;

	glm::vec3 newRight = glm::normalize(glm::cross(newForward, glm::vec3(0, 1, 0)));
	up = glm::normalize(glm::cross(newRight, newForward));

	needUpdate = true;
}

const glm::mat4& Camera::getViewMatrix() {
	updateMatrices();
	return viewMatrix;
}

const glm::mat4& Camera::getProjectionMatrix() {
	updateMatrices();
	return projectionMatrix;
}

glm::mat4 Camera::getViewProjectionMatrix() {
	updateMatrices();
	return projectionMatrix * viewMatrix;
}