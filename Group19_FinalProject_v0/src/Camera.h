#ifndef GAME_CAMERA_h
#define GAME_CAMERA_h

// GLEW/GLAD
#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
// OTHER
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <map>

enum MOVE_DIRECTION {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

class Camera {
private:	
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;
	glm::vec3 worldUp;

	float yaw;
	float pitch;
	float lastX;
	float lastY;
	float zoom;

	float MovementSpeed;
	float TurnSensitivity;
	bool constrainPitch;

	void updateVector() {
		glm::vec3 front;
		front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		front.y = sin(glm::radians(this->pitch));
		front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

		this->cameraFront = glm::normalize(front);
		this->cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
		this->cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
	}

public:

	void setSpeed(float move = 2.5f, float turn = 0.5f) {
		// 调整镜头移动速度
		MovementSpeed = move;
		TurnSensitivity = turn;
	}

	Camera(glm::vec3 position = glm::vec3(0.0f,0.0f,3.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
		float YAW = -90.0f, float PITCH = 0.0f, float ZOOM = 45.0f){
		
		cameraPos = position;
		worldUp = up;

		// yaw is initialized to -90.0 degrees since a yaw of 0.0 
		// results in a direction vector pointing to the right 
		// so we initially rotate a bit to the left.
		yaw = YAW;
		pitch = PITCH;
		zoom = ZOOM;

		cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
		constrainPitch = true;

		setSpeed();

		updateVector();
	};

	void moveForward(float cameraSpeed) {
		cameraPos += cameraSpeed * this->cameraFront;
	};

	void moveBack(float cameraSpeed) {
		cameraPos -= cameraSpeed * this->cameraFront;
	};

	void moveLeft(float cameraSpeed) {
		cameraPos -= this->cameraRight * cameraSpeed;
	};

	void moveRight(float cameraSpeed) {
		cameraPos += this->cameraRight * cameraSpeed;
	};

	void ProcessKeyboard(MOVE_DIRECTION direction, float deltaTime) {
		float cameraSpeed = this->MovementSpeed * deltaTime;
		if (direction == UP) {
			this->moveForward(cameraSpeed);
		}
		if (direction == DOWN) {
			this->moveBack(cameraSpeed);
		}
		if (direction == LEFT) {
			this->moveLeft(cameraSpeed);
		}
		if (direction == RIGHT) {
			this->moveRight(cameraSpeed);
		}
		cameraPos.y = 0.0f;
	};

	void ProcessMouseMove(double xoffset, double yoffset) {
		
		// 设置敏感度
		xoffset *= TurnSensitivity;
		yoffset *= TurnSensitivity;
		// 改变俯仰角
		this->yaw += xoffset;
		this->pitch += yoffset;

		if (this->constrainPitch) {
			if (this->pitch > 89.0f) {
				this->pitch = 89.0f;
			}
			if (this->pitch < -89.0f) {
				this->pitch = -89.0f;
			}
		}
		updateVector();
	};

	void ProcessMouseScroll(float yoffset) {
		if (this->zoom >= 1.0f && this->zoom <= 45.0f)
			this->zoom -= yoffset;
		if (this->zoom <= 1.0f)
			this->zoom = 1.0f;
		if (this->zoom >= 45.0f)
			this->zoom = 45.0f;
	}

	void setZoom(float input) {
		this->zoom = input;
	}

	glm::mat4 getView() const {
		return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}

	glm::vec3 getPosition() const {
		return this->cameraPos;
	}

	glm::vec3 getFrontVec() const {
		return this->cameraFront;
	}

	float getZoom() const {
		return this->zoom;
	}

	float getYaw() const {
		return this->yaw;
	}

	float getPitch() const {
		return this->pitch;
	}
};

#endif