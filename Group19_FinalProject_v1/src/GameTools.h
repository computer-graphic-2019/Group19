#ifndef GAME_TOOLS_H
#define GAME_TOOLS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Camera.h"
#include "GameResource.h"
#include "GameMove.h"
#include "SkyBox.h"
#include "GameShoot.h"
#include "Particle.h"

#include "PhysicsEngine.h"

#include <vector>

extern unsigned int SCR_WIDTH, SCR_HEIGHT;
extern ResourceManager ResM;
extern GameMove moveController;
extern PhysicsEngine physicsEngine;

extern std::map<std::string, GameObject> targetList;
extern std::map<std::string, GameObject> movingTargetList;
extern std::map<std::string, GameObject> explodeTargeList;
extern std::map<std::string, bool> explodeTargeRec;
extern std::deque<std::string> recoverList;

int numOfTree = 8;
int numOfTree3 = 16;
int numOfGrass = 200;
int numOfStone = 100;
int numOfTarget = 4;
int numOfMovingTarget = 4;
int numOfExplodeTarget = 4;

int coverWidth = 30;
int coveLength = 50;

class GameTools {
private:
	// shading
	GLuint depthMapFBO, depthMap;
	GLuint SHADOW_WIDTH, SHADOW_HEIGHT;
	glm::mat4 lightSpaceMatrix;
	glm::vec3 ambient_light, diffuse_light, specular_light;
	// model matrices
	std::vector<glm::mat4> treeModelMatrices;
	std::vector<glm::mat4> tree3ModelMatrices;
	std::vector<glm::mat4> grassModelMatrices;
	std::vector<glm::mat4> stoneModelMatrices;

public:
	// particle
	ParticleSystem fireParticle;

	GameTools(glm::vec3 light, float ambient, float diffuse, float specular) {
		// 初始化光照参数
		this->ambient_light = ambient * light;
		this->diffuse_light = diffuse * light;
		this->specular_light = specular * light;

		// 加载着色器
		ResM.loadShader("debug", "./ShaderCode/debug.vs", "./ShaderCode/debug.fs");
		ResM.loadShader("depthShader", "./ShaderCode/3.depth_mapping.vs", "./ShaderCode/3.depth_mapping.fs", "./ShaderCode/4.depth_explode_shading.gs");
		ResM.loadShader("model", "./ShaderCode/3.phong_shading.vs", "./ShaderCode/3.phong_shading.fs", "./ShaderCode/4.explode_shading.gs");
		ResM.loadShader("instancingModel", "./ShaderCode/instancing_phong_shading.vs", "./ShaderCode/instancing_phong_shading.fs");
		ResM.loadShader("instancingDepthShader", "./ShaderCode/instancing_depth_mapping.vs", "./ShaderCode/instancing_depth_mapping.fs");
		ResM.loadShader("textShader", "./ShaderCode/5.text_loading.vs", "./ShaderCode/5.text_loading.fs");
		ResM.loadShader("particleShader", "./ShaderCode/1.particle_shader.vs", "./ShaderCode/1.particle_shader.fs");
		ResM.loadShader("hdrShader", "./ShaderCode/hdr.vs", "./ShaderCode/hdr.fs");
		ResM.loadShader("normal", "./ShaderCode/normal.vs", "./ShaderCode/normal.fs");

		// 加载模型
		ResM.loadModel("place", "./models/place/scene1.obj");
		ResM.loadModel("target", "./models/target/target1.obj");
		ResM.loadModel("explodeTarget", "./models/explodeTarget/explodeTarget.obj");
		ResM.loadModel("tree", "./models/scene/tree.obj");
		ResM.loadModel("tree3", "./models/scene/tree3.obj");
		ResM.loadModel("grass", "./models/scene/grass.obj");
		ResM.loadModel("stone", "./models/scene/stone.obj");

		ResM.loadModel("gun", "./models/gun/m24.obj");
		ResM.loadModel("gunOnFire", "./models/gun/m24OnFire.obj");
		ResM.loadModel("bullet", "./models/bullet/bullet.obj");

		ResM.loadModel("box", "./models/scene/box.obj");

		// 加载贴图
		ResM.loadTexture("fire", "./img/particle/smoke.png");
		ResM.loadTexture("normalWall", "./img/wall/brickwall_normal.jpg");
		ResM.loadTexture("wall", "./img/wall/brickwall.jpg");

		// 初始化阴影贴图
		SHADOW_WIDTH = 4096;
		SHADOW_HEIGHT = 4096;
		glGenFramebuffers(1, &depthMapFBO);
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);	

		srand(1561273370); // initialize random seed	
		// 随机位置参数
		for (int i = 0; i < numOfTree; i++) {
			int x = 56;
			int z = 0;
			if (i < numOfTree/2) {
				z = 60 - 9 * (i+1);
			}
			else {
				z = -60 + 9 * (i- numOfTree/2 + 1);
			}
			float scale = rand() % 20 / (float)40 + 0.9;
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(x, 0.0f, z));
			model = glm::scale(model, glm::vec3(scale, scale, scale));
			treeModelMatrices.push_back(model);
			physicsEngine.setSceneInnerBoundary(glm::vec3(x - 4.0f, -10.0f, z - 4.0f), glm::vec3(x + 4.0f, 10.0f, z + 4.0f));
		}
		
		for (int i = 0; i < numOfTree3; i++) {
			int x = 0;
			int z = -56;
			if (i < numOfTree3 / 2) {
				z = -56;
				x = -60 + 12 * (i + 1);
			}
			else {
				z = 56;
				x = -60 + 12 * (i - numOfTree3 / 2 + 1);
			}
			float scale = rand() % 20 / (float)40 + 1.0;
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(x, 0.0f, z));
			model = glm::scale(model, glm::vec3(scale, scale, scale));
			tree3ModelMatrices.push_back(model);
			physicsEngine.setSceneInnerBoundary(glm::vec3(x - 2.0f, -8.0f, z - 2.0f), glm::vec3(x + 2.0f, 8.0f, z + 2.0f));
		}

		for (int i = 0; i < numOfGrass; i++) {
			int x = rand() % (2 * coveLength) - coveLength;
			int z = rand() % (2 * coverWidth) - coverWidth;
			if (z > 0) z += 22;
			else z -= 22;
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(x, -1.0f, z));
			model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
			grassModelMatrices.push_back(model);
			//physicsEngine.setSceneInnerBoundary(glm::vec3(x - 2.0f, 2.0f, z - 2.0f), glm::vec3(x + 2.0f, 2.0f, z + 2.0f));
		}
		
		for (int i = 0; i < numOfStone; i++) {
			int x = rand() % (2 * coveLength) - coveLength;
			int z = rand() % (2 * coverWidth) - coverWidth;
			if (z > 0) z += 22;
			else z -= 22;
			float scale = rand() % 20 / (float)40 + 1.0;
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(x, 0.0f, z));
			model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
			stoneModelMatrices.push_back(model);
		}

		// 设置靶子位置
		for (int i = 0; i < numOfTarget * 2; i++) {
			std::string name = "target";
			name += ('0' + i);
			if (i < numOfTarget) {
				glm::vec3 pos(48.0f, 6.0f, -60.0f + 8 * (i + 1));
				glm::vec3 size(1.0f, 2.2f, 2.2f);
				GameObject go(pos - size, size * 2.0f);
				targetList.insert_or_assign(name, go);
				physicsEngine.setSceneInnerBoundary(pos - size, pos + size);
			}
			else {
				glm::vec3 pos(48.0f, 6.0f, 60.0f - 8 * ((i - numOfTarget) + 1));
				glm::vec3 size(1.0f, 2.2f, 2.2f);
				GameObject go(pos - size, size * 2.0f);
				targetList.insert_or_assign(name, go);
				physicsEngine.setSceneInnerBoundary(pos - size, pos + size);
			}		
		}

		for (int i = 0; i < numOfExplodeTarget; i++) {
			std::string name = "target";
			name += ('0' + i);
			glm::vec3 pos(48.0f, 26.0f, -20.0f + 8 * (i + 1));
			glm::vec3 size(1.0f, 1.0f, 1.0f);
			GameObject go(pos - size, size * 2.0f);
			explodeTargeList.insert_or_assign(name, go);
			explodeTargeRec.insert_or_assign(name, false);
			physicsEngine.setSceneInnerBoundary(pos - size, pos + size);
		}

		for (int i = 0; i < numOfMovingTarget; i++) {
			break;
		}

		// set boundary
		// 设置地板碰撞盒
		physicsEngine.setSceneInnerBoundary(glm::vec3(-60.0f, 0.0f, -60.0f), glm::vec3(60.0f, 0.0f, 60.0f));
		physicsEngine.setSceneOuterBoundary(glm::vec2(-60.0f, -60.0f), glm::vec2(60.0f, 60.0f));

		// 阶梯
		int x = -34, z = -16, y = 1;
		for (int i = 0; i < 4; i++) {
			physicsEngine.setSceneInnerBoundary(glm::vec3(x - 6.0f, y - 20.0f, z - 2.0f), glm::vec3(x + 6.0f, y + 1.0f, z + 2.0f));
			z += 5;
			y += 6;
		}
		z = 16;
		y = 1;
		for (int i = 0; i < 4; i++) {
			physicsEngine.setSceneInnerBoundary(glm::vec3(x - 6.0f, y - 20.0f, z - 2.0f), glm::vec3(x + 6.0f, y + 1.0f, z + 2.0f));
			z -= 5;
			y += 6;
		}

		// 中部平台
		glm::vec3 platform(16.0f, 10.0f, 0.0f);
		glm::vec3 size(44.5f, 10.0f, 18.5f);
		physicsEngine.setSceneInnerBoundary(platform-size, platform+size);

		// 火焰平台
		glm::vec3 firePos(-23.0f, 22.0f, 0.0f);
		size = glm::vec3(0.5f, 18.0f, 0.5f);
		physicsEngine.setSceneInnerBoundary(firePos-size, firePos+size);

		ConfigureInstancedArray();

		// 火焰粒子
		this->fireParticle.init(ResM.getShader("particleShader"), ResM.getTexture("fire")->getTexture(), 1000);
	}

	void ConfigureInstancedArray() {
		unsigned int treeBuffer;
		glGenBuffers(1, &treeBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, treeBuffer);
		glBufferData(GL_ARRAY_BUFFER, numOfTree * sizeof(glm::mat4), &treeModelMatrices[0], GL_STATIC_DRAW);
		for (unsigned int i = 0; i < ResM.getModel("tree")->meshes.size(); i++) {
			unsigned int VAO = ResM.getModel("tree")->meshes[i].VAO;
			glBindVertexArray(VAO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}

		unsigned int tree3Buffer;
		glGenBuffers(1, &tree3Buffer);
		glBindBuffer(GL_ARRAY_BUFFER, tree3Buffer);
		glBufferData(GL_ARRAY_BUFFER, numOfTree3 * sizeof(glm::mat4), &tree3ModelMatrices[0], GL_STATIC_DRAW);
		for (unsigned int i = 0; i < ResM.getModel("tree3")->meshes.size(); i++) {
			unsigned int VAO = ResM.getModel("tree3")->meshes[i].VAO;
			glBindVertexArray(VAO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}

		unsigned int grassBuffer;
		glGenBuffers(1, &grassBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, grassBuffer);
		glBufferData(GL_ARRAY_BUFFER, numOfGrass * sizeof(glm::mat4), &grassModelMatrices[0], GL_STATIC_DRAW);
		for (unsigned int i = 0; i < ResM.getModel("grass")->meshes.size(); i++) {
			unsigned int VAO = ResM.getModel("grass")->meshes[i].VAO;
			glBindVertexArray(VAO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}

		unsigned int stoneBuffer;
		glGenBuffers(1, &stoneBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, stoneBuffer);
		glBufferData(GL_ARRAY_BUFFER, numOfStone * sizeof(glm::mat4), &stoneModelMatrices[0], GL_STATIC_DRAW);
		for (unsigned int i = 0; i < ResM.getModel("stone")->meshes.size(); i++) {
			unsigned int VAO = ResM.getModel("stone")->meshes[i].VAO;
			glBindVertexArray(VAO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}
	}

	// 深度贴图
	void RenderDepthMap(glm::vec3 lightPos) {
		// config
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glDisable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);
		// 参数
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 lightProjection = glm::ortho(-120.0f, 120.0f, -120.0f, 120.0f, 0.0f, 300.0f);
		// 渲染
		Shader* shader = ResM.getShader("depthShader");
		shader->use();
		lightSpaceMatrix = lightProjection * lightView;
		shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		RenderObject(shader);
		RenderBox(shader);

		// 实例化数组阴影
		shader = ResM.getShader("instancingDepthShader");
		shader->use();
		shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		RenderInstances(shader);
		// 解绑FrameBuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_CULL_FACE);
	}

	// 实际场景
	void RenderScene(glm::vec3 lightPos) {
		// config
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glCullFace(GL_BACK);
		// 参数
		glm::vec3 viewPos = moveController.getHumanCamera()->getPosition();
		glm::mat4 view = moveController.getHumanCamera()->getView();
		glm::mat4 projection = glm::perspective(glm::radians(moveController.getHumanCamera()->getZoom()),
			(float)SCR_WIDTH / (float)SCR_HEIGHT,
			0.1f, 3000.0f);

		// 渲染火焰
		fireParticle.Draw(view, projection);

		// 渲染场景
		Shader* shader = ResM.getShader("model");
		shader->use();
		shader->setInt("shadowMap", 31);
		shader->setMat4("view", view);
		shader->setMat4("projection", projection);
		shader->setVec3("viewPos", viewPos);
		shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		shader->setVec3("light.ambient", ambient_light);
		shader->setVec3("light.diffuse", diffuse_light);
		shader->setVec3("light.specular", specular_light);
		shader->setVec3("light.position", lightPos);
		shader->setBool("isExplode", false);
		glActiveTexture(GL_TEXTURE31);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		RenderObject(shader);

		// 实例化数组渲染
		shader = ResM.getShader("instancingModel");
		shader->use();
		shader->setInt("shadowMap", 31);
		shader->setMat4("view", view);
		shader->setMat4("projection", projection);
		shader->setVec3("viewPos", viewPos);
		shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

		shader->setVec3("light.ambient", ambient_light);
		shader->setVec3("light.diffuse", diffuse_light);
		shader->setVec3("light.specular", specular_light);
		shader->setVec3("light.position", lightPos);

		shader->setBool("isExplode", false);

		// render
		glActiveTexture(GL_TEXTURE31);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		RenderInstances(shader);

		//法线贴图
		shader = ResM.getShader("normal");
		shader->use();
		shader->setInt("shadowMap", 31);
		shader->setInt("diffuseMap", 30);
		shader->setInt("normalMap", 29);
		shader->setMat4("view", view);
		shader->setMat4("projection", projection);
		shader->setVec3("viewPos", viewPos);
		shader->setVec3("lightPos", lightPos);
		shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

		shader->setVec3("light.ambient", ambient_light);
		shader->setVec3("light.diffuse", diffuse_light);
		shader->setVec3("light.specular", specular_light);
		shader->setVec3("light.position", lightPos);

		// render
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glActiveTexture(GL_TEXTURE31);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glActiveTexture(GL_TEXTURE30);
		glBindTexture(GL_TEXTURE_2D, ResM.getTexture("wall")->getTexture());
		glActiveTexture(GL_TEXTURE29);
		glBindTexture(GL_TEXTURE_2D, ResM.getTexture("normalWall")->getTexture());
		RenderBox(shader);
	}

	//渲染楼梯
	void RenderBox(Shader* shader) {
		glm::mat4 model = glm::mat4(1.0f);
		shader->setMat4("model", model);
		ResM.getModel("box")->Draw(*shader);
	}

	// 渲染所有物体
	void RenderObject(Shader* shader) {
		glm::mat4 model = glm::mat4(1.0f);

		// 地板
		model = glm::mat4(1.0f);
		shader->setMat4("model", model);
		ResM.getModel("place")->Draw(*shader);

		// 固定靶子
		for (std::map<std::string, GameObject>::iterator ptr = targetList.begin(); ptr != targetList.end(); ptr++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, ptr->second.Position + ptr->second.Size * 0.5f);
			//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			shader->setMat4("model", model);
			ResM.getModel("target")->Draw((*shader));
		}

		// 爆炸靶子
		std::map<std::string, GameObject>::iterator posPtr = explodeTargeList.begin();
		for (std::map<std::string, bool>::iterator ptr = explodeTargeRec.begin(); posPtr != explodeTargeList.end() && ptr != explodeTargeRec.end(); ptr++, posPtr++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, posPtr->second.Position + posPtr->second.Size * 0.5f);
			//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			shader->setMat4("model", model);
			if (ptr->second) {
				shader->setBool("isExplode", true);
				ResM.getModel("explodeTarget")->Draw((*shader));
			}
			else {
				shader->setBool("isExplode", false);
				ResM.getModel("explodeTarget")->Draw((*shader));
			}
		}
		shader->setBool("isExplode", false);
	}

	// 渲染实例
	void RenderInstances(Shader* shader) {

		// 两种树
		for (unsigned int i = 0; i < ResM.getModel("tree")->meshes.size(); i++) {
			ResM.getModel("tree")->DrawTexture(i, *shader);
			glBindVertexArray(ResM.getModel("tree")->meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, ResM.getModel("tree")->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, numOfTree);
			glBindVertexArray(0);
		}

		for (unsigned int i = 0; i < ResM.getModel("tree3")->meshes.size(); i++) {
			ResM.getModel("tree3")->DrawTexture(i, *shader);
			glBindVertexArray(ResM.getModel("tree3")->meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, ResM.getModel("tree3")->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, numOfTree3);
			glBindVertexArray(0);
		}

		// 草
		for (unsigned int i = 0; i < ResM.getModel("grass")->meshes.size(); i++) {
			ResM.getModel("grass")->DrawTexture(i, *shader);
			glBindVertexArray(ResM.getModel("grass")->meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, ResM.getModel("grass")->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, numOfGrass);
			glBindVertexArray(0);
		}

		// 石头
		for (unsigned int i = 0; i < ResM.getModel("stone")->meshes.size(); i++) {
			ResM.getModel("stone")->DrawTexture(i, *shader);
			glBindVertexArray(ResM.getModel("stone")->meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, ResM.getModel("stone")->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, numOfStone);
			glBindVertexArray(0);
		}
	}

	void testMap(GLFWwindow* window) {
		Shader* shader = ResM.getShader("debug");
		shader->use();
		//shader->setFloat("near_plane", near_plane);
		//shader->setFloat("far_plane", far_plane);
		shader->setInt("shadowMap", 0);
		// clear
		glfwSetWindowSize(window, SHADOW_WIDTH, SHADOW_HEIGHT);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		GLuint quadVAO, quadVBO;

		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDeleteVertexArrays(1, &quadVAO);
		glDeleteBuffers(1, &quadVBO);
	}
};

#endif