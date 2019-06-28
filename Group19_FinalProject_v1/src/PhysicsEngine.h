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

const float HeroHeight = 5.0f;           //����ӵ㵽�ŵĸ߶�

const float GravityAcceler = -9.8f;

const float BoundaryGap = 1.0f;          //��ײ���
const float JumpInitialSpeed = 12.0f;    //�������ٶ�
const float JumpFactor = 0.08f;          //�����ٶ�ϵ��
const float GravityFactor = 0.08f;       //�����ٶ�ϵ��

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

	//���ÿռ��ⲿ��Ե(��������������Χ)
	void setSceneOuterBoundary(glm::vec2 point1, glm::vec2 point2) {
		outerBoundary = glm::vec4(point1, point2);
	}

	//�ⲿ��ײ���
	void outCollisionTest(glm::vec3& cameraPos, glm::vec3& targetPos) {
		glm::vec2 obj1(outerBoundary[0], outerBoundary[1]), obj2(outerBoundary[2], outerBoundary[3]);
		outCollisionTestXZ(obj1, obj2, cameraPos, targetPos);
	}

	//���ÿռ��ڲ���Ե�������ڲ��ı�Ե��
	void setSceneInnerBoundary(glm::vec3 angle1, glm::vec3 angle2) {
		glm::vec3 key = angle1 - glm::vec3(BoundaryGap);
		glm::vec3 value = angle2 + glm::vec3(BoundaryGap);

		innerBoundaryMin.push_back(key);
		innerBoundaryMax.push_back(value);
	}
	//�ڲ���ײ���
	void inCollisionTest(glm::vec3& cameraPos, glm::vec3& targetPos) {
		//���������������ӣ�Ԥ�����ų���ǰ�϶����������ײ������
		for (int i = 0; i < innerBoundaryMin.size(); i++) {
			inCollisionTestWithHeight(innerBoundaryMin[i], innerBoundaryMax[i], cameraPos, targetPos);
		}
	}

	bool isJumping;

	//����space��Ծʱ����
	void jumpAndUpdateVelocity() {
		velocity += glm::vec3(0.f, JumpInitialSpeed, 0.f);
		accelerUp.y = 0.0f;
	}

	//ÿ֡���Ƶ�ʱ������������ֱ�����ƶ�
	void updateCameraVertMovement(glm::vec3& cameraPos, glm::vec3& targetPos) {
		glm::vec3 acceletation = this->gravity + this->accelerUp;
		velocity += acceletation * GravityFactor;
		cameraPos += velocity * JumpFactor;
		targetPos += velocity * JumpFactor;
		//���������ײ��
		for (int i = 0; i < innerBoundaryMin.size(); i++) {
			//�����XZƽ�������ײ����������
			if (insideTheCollider(cameraPos, innerBoundaryMin[i], innerBoundaryMax[i])) {
				//�ŽӴ�����ײ�嶥��
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

	//�ռ��ڲ���Ե��ײ��⣨���Ǹ߶ȣ�
	void inCollisionTestWithHeight(glm::vec3& obj1, glm::vec3& obj2, glm::vec3& cameraPos, glm::vec3& targetPos) {
		if (!(cameraPos.y <= obj1.y || cameraPos.y - HeroHeight >= obj2.y)) {
			glm::vec2 obj1XZ(obj1.x, obj1.z), obj2XZ(obj2.x, obj2.z);
			inCollisionTestXZ(obj1XZ, obj2XZ, cameraPos, targetPos);
		}
	}

	// �߶��ཻ�����㷨
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

	//�ռ��ڲ���Ե��ײ��⣨�����Ǹ߶ȣ���XZƽ�棩
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

	//�ռ��ⲿ��Ե��ײ���
	void outCollisionTestXZ(glm::vec2 obj1, glm::vec2 obj2, glm::vec3& cameraPos, glm::vec3& targetPos) {
		
		const float collisionThres = 0.5f;

		//�����ð�Χ�У��ȿռ��ⲿ��ԵСһ��
		obj1.x += obj1.x < 0 ? 2 : -2;
		obj2.x += obj2.x < 0 ? 2 : -2;
		obj1.y += obj1.y < 0 ? 2 : -2;
		obj2.y += obj2.y < 0 ? 2 : -2;

		//���Ŀ��λ�ó��˰�Χ�У��ȷŻ���
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

		// �����������Ŀ��ľ���
		float distance = sqrt(pow((cameraPos.x - targetPos.x),2) + pow((cameraPos.z - targetPos.z),2));

		//std::cout << "distance: " << distance << std::endl;

		//���ӵ���Ŀ�����̫С����̶�Ŀ��λ�ã��ӵ�������Ŀ����淽���ƶ�
		if (distance <= collisionThres) {
			cameraPos.x = collisionThres * (cameraPos.x - targetPos.x) / distance + targetPos.x;
			cameraPos.z = collisionThres * (cameraPos.z - targetPos.z) / distance + targetPos.z;
		}

		bool isOutOfRange = false;
		//�ٴμ���ӵ�λ���Ƿ���˰�Χ�У��ȷŻ���
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

		//�ظ�����Զ���������Ĳ���
		if (isOutOfRange) {
			distance = sqrt(pow((cameraPos.x - targetPos.x), 2) + pow((cameraPos.z - targetPos.z), 2));

			if (distance <= collisionThres) {
				targetPos.x = collisionThres * (targetPos.x - cameraPos.x) / distance + cameraPos.x;
				targetPos.z = collisionThres * (targetPos.z - cameraPos.z) / distance + cameraPos.z;
			}
		}
	}

	glm::vec3 velocity;        //��ֱ�����ٶ�
	glm::vec3 gravity;         //�������ٶ�
	glm::vec3 accelerUp;       //�������ϵļ��ٶ�

	glm::vec4 outerBoundary;
	std::vector<glm::vec3> innerBoundaryMin;    //��ײ��С��x/y/z����
	std::vector<glm::vec3> innerBoundaryMax;    //��ײ�����x/y/z����
};

#endif