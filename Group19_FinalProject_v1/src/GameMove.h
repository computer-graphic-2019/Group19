#ifndef GAME_MOVE_H
#define GAME_MOVE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Camera.h"
#include "GameResource.h"
#include "PhysicsEngine.h"

#include <iostream>

class GameMove {
private:
    bool isGunRaiseUp = false;
	Camera* humanCamera;
	float deltatime;
	int jumpTimes;
public:
	GameMove();
    // 控制人物前移(w)
    void humanMoveForward();
    // 控制人物后退(s)
    void humanMoveBackward();
    // 控制人物左转(a)
    void humanMoveLeft();
    // 控制人物右转(d)
    void humanMoveRight();
	// 控制人物跳跃
	void humanJump();
    // 人物移动总控制
	void humanRotate(float x, float y);
    void humanMove(GLFWwindow *window, float deltaTime);
	// 获取人物视角
	Camera* getHumanCamera() const;
    // 控制举枪（右键开镜）
    void raiseUpGun();
    // 控制收枪（右键关镜）
    void putDownGun();
    // 枪械动作总控制
    void gunMove(bool signal);
};

#endif