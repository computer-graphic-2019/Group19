#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>
#include <vector>

const float HeroHeight = 5.0f;           //玩家视点到脚的高度

const float GravityAcceler = -9.8f;

const float BoundaryGap = 1.0f;          //碰撞间距
const float JumpInitialSpeed = 12.0f;    //起跳初速度
const float JumpFactor = 0.08f;          //跳起速度系数
const float GravityFactor = 0.08f;       //下落速度系数

class PhysicsEngine {
public:
	PhysicsEngine() {
		this->isJumping = false;
		this->velocity = glm::vec3(0.0f);
		this->gravity = glm::vec3(0.0f, GravityAcceler, 0.0f);
		this->accelerUp = glm::vec3(0.0f);
		this->outerBoundary = glm::vec4(0.0f);
	}

	~PhysicsEngine() {

	}

	//设置空间外部边缘(整个场景的最外围)
	void setSceneOuterBoundary(glm::vec2 point1, glm::vec2 point2) {
		outerBoundary = glm::vec4(point1, point2);
	}

	//外部碰撞检测
	void outCollisionTest(glm::vec3& cameraPos, glm::vec3& targetPos) {
		glm::vec2 obj1(outerBoundary[0], outerBoundary[1]), obj2(outerBoundary[2], outerBoundary[3]);
		outCollisionTestXZ(obj1, obj2, cameraPos, targetPos);
	}

	//设置空间内部边缘（场景内部的边缘）
	void setSceneInnerBoundary(glm::vec3 angle1, glm::vec3 angle2) {
		glm::vec3 key = angle1 - glm::vec3(BoundaryGap);
		glm::vec3 value = angle2 + glm::vec3(BoundaryGap);

		innerBoundaryMin.push_back(key);
		innerBoundaryMax.push_back(value);
	}
	//内部碰撞检测
	void inCollisionTest(glm::vec3& cameraPos, glm::vec3& targetPos) {
		//后面可以在这里添加：预处理，排除当前肯定不会产生碰撞的物体
		for (int i = 0; i < innerBoundaryMin.size(); i++) {
			inCollisionTestWithHeight(innerBoundaryMin[i], innerBoundaryMax[i], cameraPos, targetPos);
		}
	}

	bool isJumping;

	//按下space跳跃时调用
	void jumpAndUpdateVelocity() {
		velocity += glm::vec3(0.f, JumpInitialSpeed, 0.f);
		accelerUp.y = 0.0f;
	}

	//每帧绘制的时候更新摄像机垂直方向移动
	void updateCameraVertMovement(glm::vec3& cameraPos, glm::vec3& targetPos) {
		glm::vec3 acceletation = this->gravity + this->accelerUp;
		velocity += acceletation * GravityFactor;
		cameraPos += velocity * JumpFactor;
		targetPos += velocity * JumpFactor;
		//检测所有碰撞体
		for (int i = 0; i < innerBoundaryMin.size(); i++) {
			//如果在XZ平面进入碰撞体所在区域
			if (insideTheCollider(cameraPos, innerBoundaryMin[i], innerBoundaryMax[i])) {
				//脚接触到碰撞体顶部
				if (cameraPos.y - HeroHeight <= innerBoundaryMax[i].y
					&& cameraPos.y >= innerBoundaryMax[i].y) {
					isJumping = false;
					accelerUp.y = -GravityAcceler;
					velocity.y = 0.0f;
					cameraPos.y = innerBoundaryMax[i].y + HeroHeight;
					break;
 				}

				if (cameraPos.y - HeroHeight <= innerBoundaryMin[i].y
					&& cameraPos.y >= innerBoundaryMin[i].y) {
					velocity.y = 0.0f;
					cameraPos.y = innerBoundaryMin[i].y;
					break;
				}
			}
			else {
				accelerUp.y = 0.0f;
			}
		}
	}

protected:

	inline float Direction(glm::vec2& pi, glm::vec2& pj, glm::vec2& pk) {
		return (pk.x - pi.x) * (pj.y - pi.y) - (pj.x - pi.x) * (pk.y - pi.y);
	}

	inline bool onSegment(glm::vec2& pi, glm::vec2& pj, glm::vec2& pk) {
		return (std::min(pi.x, pj.x) <= pk.x) && (pk.x <= std::max(pi.x, pj.x))
			&& (std::min(pi.y, pj.y) <= pk.y) && (pk.y <= std::max(pi.y, pj.y));
	}

	inline bool insideTheCollider(glm::vec3& cameraPos, glm::vec3& innerMin, glm::vec3& innerMax) {
		float camX = cameraPos.x;
		float camZ = cameraPos.z;
		float minX = innerMin.x;
		float minZ = innerMin.z;
		float maxX = innerMax.x;
		float maxZ = innerMax.z;

		return (minX <= camX && camX <= maxX && minZ <= camZ && camZ <= maxZ);
	}

private:

	//空间内部边缘碰撞检测（考虑高度）
	void inCollisionTestWithHeight(glm::vec3& obj1, glm::vec3& obj2, glm::vec3& cameraPos, glm::vec3& targetPos) {
		if (!(cameraPos.y <= obj1.y || cameraPos.y - HeroHeight >= obj2.y)) {
			glm::vec2 obj1XZ(obj1.x, obj1.z), obj2XZ(obj2.x, obj2.z);
			inCollisionTestXZ(obj1XZ, obj2XZ, cameraPos, targetPos);
		}
	}

	// 线段相交快速算法
	bool segmentIntersect(glm::vec2& p1, glm::vec2& p2, glm::vec2& p3, glm::vec2& p4) {
		float d1 = 0, d2 = 0, d3 = 0, d4 = 0;
		d1 = Direction(p3, p4, p1);
		d2 = Direction(p3, p4, p2);
		d3 = Direction(p1, p2, p3);
		d4 = Direction(p1, p2, p4);
		if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2>0)) && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4>0)))
			return true;
		else if (d1 == 0 && onSegment(p3, p4, p1))
			return true;
		else if (d2 == 0 && onSegment(p3, p4, p2))
			return true;
		else if (d3 == 0 && onSegment(p1, p2, p3))
			return true;
		else if (d4 == 0 && onSegment(p1, p2, p4))
			return true;
		else
			return false;
	}

	//空间内部边缘碰撞检测（不考虑高度，即XZ平面）
	void inCollisionTestXZ(glm::vec2& obj1, glm::vec2& obj2, glm::vec3& cameraPos, glm::vec3& targetPos) {
		float tarX = targetPos.x, tarZ = targetPos.z,
			camX = cameraPos.x, camZ = cameraPos.z;
		float len = sqrt(pow(tarX - camX, 2) + pow(tarZ - camZ, 2));
		
		glm::vec2 d1(cameraPos.x, cameraPos.z), d2(targetPos.x, targetPos.z);
		glm::vec2 d3(obj1.x, obj1.y), d4(obj1.x, obj2.y), d5(obj2.x, obj1.y), d6(obj2.x, obj2.y);

		if (segmentIntersect(d1, d2, d4, d6)) {
			if (targetPos.z < cameraPos.z) {
				//printf("1\n");
				targetPos.z = obj2.y;
				cameraPos.z += targetPos.z - tarZ;
			}
			else if(targetPos.z > cameraPos.z) {
				//printf("2\n");
				cameraPos.z = obj2.y;
				targetPos.z += cameraPos.z - camZ;
			}
		}
		else if (segmentIntersect(d1, d2, d5, d6)) {
			if (targetPos.x < cameraPos.x) {
				//printf("3\n");
				targetPos.x = obj2.x;
				cameraPos.x += targetPos.x - tarX;
			}
			else if (targetPos.x > cameraPos.x) {
				//printf("4\n");
				cameraPos.x = obj2.x;
				targetPos.x += cameraPos.x - camX;
			}
		}
		else if (segmentIntersect(d1, d2, d3, d5)) {
			if (targetPos.z > cameraPos.z) {
				//printf("5\n");
				targetPos.z = obj1.y;
				cameraPos.z += targetPos.z - tarZ;
			}
			else if (targetPos.z < cameraPos.z) {
				//printf("6\n");
				cameraPos.z = obj1.y;
				targetPos.z += cameraPos.z - camZ;
			}
		}
		else if (segmentIntersect(d1, d2, d3, d4)) {
			if (targetPos.x > cameraPos.x) {
				//printf("7\n");
				targetPos.x = obj1.x;
				cameraPos.x += targetPos.x - tarX;
			}
			else if (targetPos.x < cameraPos.x) {
				//printf("8\n");
				cameraPos.x = obj1.x;
				targetPos.x += cameraPos.x - camX;
			}
		}
	}

	//空间外部边缘碰撞检测
	void outCollisionTestXZ(glm::vec2 obj1, glm::vec2 obj2, glm::vec3& cameraPos, glm::vec3& targetPos) {
		
		const float collisionThres = 0.5f;

		//先设置包围盒：比空间外部边缘小一点
		obj1.x += obj1.x < 0 ? 2 : -2;
		obj2.x += obj2.x < 0 ? 2 : -2;
		obj1.y += obj1.y < 0 ? 2 : -2;
		obj2.y += obj2.y < 0 ? 2 : -2;

		//如果目标位置出了包围盒，先放回来
		if (targetPos.x < obj1.x) {
			targetPos.x = obj1.x;
		}
		if (targetPos.x > obj2.x) {
			targetPos.x = obj2.x;
		}
		if (targetPos.z < obj1.y) {
			targetPos.z = obj1.y;
		}
		if (targetPos.z > obj2.y) {
			targetPos.z = obj2.y;
		}

		//std::cout << "CamPos: " << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << std::endl;
		//std::cout << "TarPos: " << targetPos.x << " " << targetPos.y << " " << targetPos.z << std::endl;

		// 计算摄像机与目标的距离
		float distance = sqrt(pow((cameraPos.x - targetPos.x),2) + pow((cameraPos.z - targetPos.z),2));

		//std::cout << "distance: " << distance << std::endl;

		//若视点与目标距离太小，则固定目标位置，视点沿正对目标的逆方向移动
		if (distance <= collisionThres) {
			cameraPos.x = collisionThres * (cameraPos.x - targetPos.x) / distance + targetPos.x;
			cameraPos.z = collisionThres * (cameraPos.z - targetPos.z) / distance + targetPos.z;
		}

		bool isOutOfRange = false;
		//再次检测视点位置是否出了包围盒，先放回来
		if (cameraPos.x < obj1.x) {
			cameraPos.x = obj1.x;
			isOutOfRange = true;
		}
		if (cameraPos.x > obj2.x) {
			cameraPos.x = obj2.x;
			isOutOfRange = true;
		}
		if (cameraPos.z < obj1.y) {
			cameraPos.z = obj1.y;
			isOutOfRange = true;
		}
		if (cameraPos.z > obj2.y) {
			cameraPos.z = obj2.y;
			isOutOfRange = true;
		}

		//重复上述远离两点距离的操作
		if (isOutOfRange) {
			distance = sqrt(pow((cameraPos.x - targetPos.x), 2) + pow((cameraPos.z - targetPos.z), 2));

			if (distance <= collisionThres) {
				targetPos.x = collisionThres * (targetPos.x - cameraPos.x) / distance + cameraPos.x;
				targetPos.z = collisionThres * (targetPos.z - cameraPos.z) / distance + cameraPos.z;
			}
		}
	}

	glm::vec3 velocity;        //垂直方向速度
	glm::vec3 gravity;         //重力加速度
	glm::vec3 accelerUp;       //方向向上的加速度

	glm::vec4 outerBoundary;
	std::vector<glm::vec3> innerBoundaryMin;    //碰撞器小的x/y/z坐标
	std::vector<glm::vec3> innerBoundaryMax;    //碰撞器大的x/y/z坐标
};

#endif