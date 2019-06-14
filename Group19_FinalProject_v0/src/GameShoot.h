#ifndef GAME_SHOOT_H
#define GAME_SHOOT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <deque>
#include <ft2build.h>
#include FT_FREETYPE_H  

#include "Camera.h"
#include "GameResource.h"
#include "GameObject.h"
#include "GameMove.h"

class GameShoot {
private:
	glm::vec3 bulletPos;
	glm::vec3 direction;
	GameObject bullet;
	bool isHit;
	int point;
public:
	GameShoot();
    // 控制开枪（鼠标左键）
    void Fire();
	// 子弹轨迹
	void showBullet(float deltaTime);
    // 检查命中位置
    void CheckCollisionWithTarget();
    // 计算命中得分
    void CalculateScore();
	// 显示得分
	void showScore(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
	void initialTextShader();
};

#endif