#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Camera
{
	glm::vec3 position;
	glm::vec3 target;
	glm::mat4 viewMatrix;
	void updateViewMatrix() {
		viewMatrix = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
	}
public:
	Camera() {};
	Camera(glm::vec3 position, glm::vec3 target) {
		this->position = position;
		this->target = target;
		updateViewMatrix();
	}
	void setTarget(glm::vec3 target) {
		this->target = target;
		updateViewMatrix();
	}
	void setPosition(glm::vec3 position) {
		this->position = position;
		updateViewMatrix();
	}
	glm::mat4 getViewMatrix() {
		return viewMatrix;
	}
	glm::vec3 getPosition() {
		return position;
	}
	// rotates the target around the camera position
	void rotateYaw(float yaw) {
		target = position + glm::vec3(glm::rotate(glm::mat4(),yaw, glm::vec3(0,1,0)) * glm::vec4((target - position), 1.0f));
		updateViewMatrix();
	}
	// rotates the camera around the target
	void panYaw(float yaw) {
		position = target + glm::vec3(glm::rotate(glm::mat4(), yaw, glm::vec3(0, 1, 0)) * glm::vec4((position - target), 1.0f));
		updateViewMatrix();
	}
	void panPitch(float pitch) {
		glm::vec3 pitchDirection = glm::normalize(glm::cross(glm::vec3(0.0,1.0,0.0), target-position));
		position = target + glm::vec3(glm::rotate(glm::mat4(), pitch, pitchDirection) * glm::vec4((position - target), 1.0f));
		updateViewMatrix();
	}
	void rotatePitch(float pitch) {
		glm::vec3 pitchDirection = glm::normalize(glm::cross(glm::vec3(0.0, 1.0, 0.0), target - position));
		target = position + glm::vec3(glm::rotate(glm::mat4(), pitch, pitchDirection) * glm::vec4((target - position), 1.0f));
		updateViewMatrix();
	}
	void setPanPitch(float pitch) {
		float distance = glm::length(position - target);
		float dx = position.x - target.x;
		float dz = position.z - target.z;
		float newY = target.y + distance * glm::sin(pitch);
		float dy = newY - target.y;
		float scaleFactor = glm::sqrt((distance*distance-dy*dy)/(dx*dx + dz*dz));
		float newX = target.x + dx * scaleFactor;
		float newZ = target.z + dz * scaleFactor;

		position = glm::vec3(newX, newY, newZ);
		updateViewMatrix();
	}

	void setPanYaw(float yaw) {
		float dx = position.x - target.x;
		float dz = position.z - target.z;
		float r = glm::sqrt(dx*dx + dz*dz);
		auto tmp = position.y;
		position = glm::vec3(glm::rotate(glm::mat4(), yaw, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, 1, 1)) * r;
		position.y = tmp;
		updateViewMatrix();
	 }
	void setCameraDistance(float distance) {
		position = target + glm::normalize(position - target) * distance;
		updateViewMatrix();
	}
	void changeCameraDistance(float relativeChange) {
		position = target + (position - target) * relativeChange;
		updateViewMatrix();
	}
};

