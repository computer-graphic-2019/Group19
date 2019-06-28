#include "GameMove.h"

extern unsigned int SCR_WIDTH, SCR_HEIGHT;
extern ResourceManager ResM;
extern GameMove moveController;
extern PhysicsEngine physicsEngine;

GameMove::GameMove() {
	this->humanCamera = new Camera(glm::vec3(-50.0f, 5.0f, 0.0f));
	this->humanCamera->setSpeed(10.0f, 0.5f);
}

// 控制人物前移(w)
void GameMove::humanMoveForward() {
	this->humanCamera->ProcessKeyboard(UP, this->deltatime, this->jumpTimes);
}

// 控制人物后退(s)
void GameMove::humanMoveBackward() {
	this->humanCamera->ProcessKeyboard(DOWN, this->deltatime, this->jumpTimes);
}

// 控制人物左转(a)
void GameMove::humanMoveLeft() {
	this->humanCamera->ProcessKeyboard(LEFT, this->deltatime, this->jumpTimes);
}

// 控制人物右转(d)
void GameMove::humanMoveRight() {
	this->humanCamera->ProcessKeyboard(RIGHT, this->deltatime, this->jumpTimes);
}

// 控制人物跳跃
void GameMove::humanJump() {
	this->humanCamera->ProcessKeyboard(JUMP, this->deltatime, this->jumpTimes);
}

// 控制人物旋转
void GameMove::humanRotate(float xoffset, float yoffset) {
	this->humanCamera->ProcessMouseMove(xoffset, yoffset);
}

// 人物移动总控制
void GameMove::humanMove(GLFWwindow *window, float deltaTime) {
	this->deltatime = deltaTime;
	bool anyKeyPress = false;
	this->jumpTimes = 1;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		humanMoveForward();
		anyKeyPress = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		humanMoveBackward();
		anyKeyPress = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		humanMoveLeft();
		anyKeyPress = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		humanMoveRight();
		anyKeyPress = true;
	}
	if (!physicsEngine.isJumping && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		humanJump();
	}
	if (!anyKeyPress && physicsEngine.isJumping) {
		humanJump();
	}
}

Camera* GameMove::getHumanCamera() const {
	return this->humanCamera;
}

// 控制举枪（右键开镜）
void GameMove::raiseUpGun() {
	// 场景放大（有可能导致子弹飞行的视觉误差）
	moveController.getHumanCamera()->setZoom(30.0f);
	// 绘制枪
	glm::mat4 projection = glm::perspective(glm::radians(30.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -0.8848f, -0.66f));
	model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ResM.getShader("model")->use();
	ResM.getShader("model")->setMat4("view", glm::mat4(1.0f));
	ResM.getShader("model")->setMat4("model", model);
	ResM.getShader("model")->setMat4("projection", projection);
	ResM.getModel("gunOnFire")->Draw((*ResM.getShader("model")));
}

// 控制收枪（右键关镜）
void GameMove::putDownGun() {
	// 场景缩小
	moveController.getHumanCamera()->setZoom(45.0f);
	// 绘制枪
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.1f, -0.1f, -0.5f));
	model = glm::scale(model, glm::vec3(0.013f, 0.013f, 0.013f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 gunRotate = glm::rotate(glm::mat4(1.0f), glm::radians(moveController.getHumanCamera()->getPitch()), glm::vec3(1.0f, 0.0f, 0.0f));
	gunRotate = glm::rotate(gunRotate, glm::radians(moveController.getHumanCamera()->getYaw()), glm::vec3(0.0f, 1.0f, 0.0f));
	
	ResM.getShader("model")->use();
	ResM.getShader("model")->setMat4("gunRotate", gunRotate);
	ResM.getShader("model")->setMat4("view", glm::mat4(1.0f));
	ResM.getShader("model")->setMat4("model", model);
	ResM.getShader("model")->setMat4("projection", projection);
	ResM.getModel("gun")->Draw((*ResM.getShader("model")));
}

// 枪械动作总控制
void GameMove::gunMove(bool signal) {
	this->isGunRaiseUp = signal;
	if (this->isGunRaiseUp) {
		this->raiseUpGun(); 
	}
	else {
		this->putDownGun();
	}
}