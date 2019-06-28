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

#include "PhysicsEngine.h"

extern PhysicsEngine physicsEngine;

enum MOVE_DIRECTION {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	JUMP
};

class Camera {
private:	
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;
	glm::vec3 worldUp;
	glm::vec3 targetPos;

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
		targetPos = cameraPos + glm::vec3(5.0f, 5.0f, 5.0f);
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
	/*
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
		cameraPos.y = 5.0f;
	};
	*/
	void ProcessKeyboard(MOVE_DIRECTION direction, float deltaTime, int& times) {
		HandleHoriMove(direction, deltaTime);
		HandleVertMove(direction, deltaTime, times);
		//std::cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << std::endl;
	}

	void HandleVertMove(MOVE_DIRECTION direction, float deltaTime, int& times) {
		if (direction == JUMP) {
			if (!physicsEngine.isJumping) {
				physicsEngine.jumpAndUpdateVelocity();
			}
			physicsEngine.isJumping = true;
		}
		if (times > 0) {
			physicsEngine.updateCameraVertMovement(cameraPos, targetPos);
			times--;
		}
	}

	void HandleHoriMove(MOVE_DIRECTION direction, float deltaTime) {
		float dx = 0, dz = 0;
		float cameraSpeed = this->MovementSpeed * 0.04;// * deltaTime;
		if (direction == UP) {
			dz += cameraSpeed;
		}
		if (direction == DOWN) {
			dz -= cameraSpeed;
		}
		if (direction == LEFT) {
			dx -= cameraSpeed;
		}
		if (direction == RIGHT) {
			dx += cameraSpeed;
		}

		if (dz != 0 || dx != 0) {
			// 行走不改变y轴
			glm::vec3 forward = glm::vec3(this->cameraFront.x, 0.0f, this->cameraFront.z);
			glm::vec3 strafe = glm::vec3(this->cameraRight.x, 0.0f, this->cameraRight.z);

			cameraPos += (dz * forward + dx * strafe);
			targetPos = cameraPos + (dz * forward + dx * strafe) * 1.5f;

			//每次做完坐标变换后，先进行碰撞检测来调整坐标
			physicsEngine.outCollisionTest(cameraPos, targetPos);
			physicsEngine.inCollisionTest(cameraPos, targetPos);
		}
	}

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