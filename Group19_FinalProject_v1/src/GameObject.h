#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class GameObject {
public:
	glm::vec3 Size;
	glm::vec3 Position;

	GameObject() {
		this->Size = glm::vec3(1.0f);
		this->Position = glm::vec3(1.0f);
	}

	GameObject(glm::vec3 p, glm::vec3 s) {
		this->Size = s;
		this->Position = p;
	}

	bool CheckCollision(GameObject &other) // AABBCC - AABBCC collision
	{
		// x轴方向碰撞？
		bool collisionX = this->Position.x + this->Size.x >= other.Position.x &&
			other.Position.x + other.Size.x >= this->Position.x;
		// y轴方向碰撞？
		bool collisionY = this->Position.y + this->Size.y >= other.Position.y &&
			other.Position.y + other.Size.y >= this->Position.y;
		// z轴方向碰撞？
		bool collisionZ = this->Position.z + this->Size.z >= other.Position.z &&
			other.Position.z + other.Size.z >= this->Position.z;
		// 只有三个轴向都有碰撞时才碰撞
		return collisionX && collisionY && collisionZ;
	}
};


#endif