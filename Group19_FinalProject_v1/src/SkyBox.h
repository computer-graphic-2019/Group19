#ifndef SKYBOX_h
#define SKYBOX_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include <vector>
#include <string>

#include "Shader.h"

// global value
extern unsigned int SCR_WIDTH, SCR_HEIGHT;
extern ResourceManager ResM;
extern GameMove moveController;

class SkyBox {
public:
	//static Shader shader;

	SkyBox() {
		// 加载顶点
		VAO = loadVec();
		// 加载纹理
		std::string path = "miramar/miramar";
		std::vector<std::string> vecSkyfaces;
		vecSkyfaces.push_back("./img/skybox/" + path + "_ft.tga");
		vecSkyfaces.push_back("./img/skybox/" + path + "_bk.tga");
		vecSkyfaces.push_back("./img/skybox/" + path + "_up.tga");
		vecSkyfaces.push_back("./img/skybox/" + path + "_dn.tga");
		vecSkyfaces.push_back("./img/skybox/" + path + "_rt.tga");
		vecSkyfaces.push_back("./img/skybox/" + path + "_lf.tga");
		textureId = loadCubemapTexture(vecSkyfaces);
		// 加载顶点着色器
		ResM.loadShader("skybox", "./ShaderCode/2.skybox_shader.vs", "./ShaderCode/2.skybox_shader.fs");
	};

	void renderSkyBox(float time) {
		// 绘制天空盒
		Shader *shader = ResM.getShader("skybox");
		shader->use();
		glm::mat4 size = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(glm::mat3(moveController.getHumanCamera()->getView())) * glm::scale(size, glm::vec3(2.0f));
		glm::mat4 projection = glm::perspective(glm::radians(moveController.getHumanCamera()->getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		shader->setMat4("view", view);
		shader->setMat4("projection", projection);
		shader->setFloat("time", sin(time));

		glBindVertexArray(VAO);
		{
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(glGetUniformLocation(shader->ID, "skybox"), 0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}
		glBindVertexArray(0);
	}

private:
	GLuint VAO;
	GLuint textureId;
	// 加载顶点
	GLuint loadVec() {
		GLfloat skyboxVec[] = {
			//X+轴面
			100.0f, -100.0f, -100.0f,		100.0f, -100.0f,  100.0f,		100.0f,  100.0f,  100.0f,
			100.0f,  100.0f,  100.0f,		100.0f,  100.0f, -100.0f,		100.0f, -100.0f, -100.0f,
			//X-轴面
			-100.0f, -100.0f,  100.0f,		-100.0f, -100.0f, -100.0f,		-100.0f,  100.0f, -100.0f,
			-100.0f,  100.0f, -100.0f,		-100.0f,  100.0f,  100.0f,		-100.0f, -100.0f,  100.0f,
			//Y+轴面
			-100.0f,  100.0f, -100.0f,		100.0f,  100.0f, -100.0f,		100.0f,  100.0f,  100.0f,
			100.0f,  100.0f,  100.0f,		-100.0f,  100.0f,  100.0f,		-100.0f,  100.0f, -100.0f,
			//Y-轴面
			-100.0f, -100.0f, -100.0f,		-100.0f, -100.0f,  100.0f,		100.0f, -100.0f, -100.0f,
			100.0f, -100.0f, -100.0f,		-100.0f, -100.0f,  100.0f,		100.0f, -100.0f,  100.0f,
			//Z+轴面
			-100.0f, -100.0f, 100.0f,		-100.0f, 100.0f, 100.0f,		100.0f, 100.0f, 100.0f,
			100.0f, 100.0f, 100.0f,			100.0f, -100.0f, 100.0f,		-100.0f, -100.0f, 100.0f,
			//Z-轴面
			-100.0f, 100.0f, -100.0f,     -100.0f, -100.0f, -100.0f,		100.0f, -100.0f, -100.0f,
			100.0f, -100.0f, -100.0f,      100.0f, 100.0f, -100.0f,			-100.0f, 100.0f, -100.0f
		};
		// 缓冲绑定
		GLuint skyboxVAO, skyboxVBO;
		glGenVertexArrays(1, &skyboxVAO);
		glBindVertexArray(skyboxVAO);
		{
			glGenBuffers(1, &skyboxVBO);
			glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVec), skyboxVec, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
		}
		glBindVertexArray(0);
		return skyboxVAO;
	}
	// 加载天空盒纹理
	GLuint loadCubemapTexture(std::vector<std::string> vecSkyfaces)
	{
		GLuint textureId = 0;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
		{
			int nW = 0, nH = 0, nC = 0;
			unsigned char* pChImg = nullptr;

			for (int i = 0; i < vecSkyfaces.size(); i++)
			{
				pChImg = stbi_load(vecSkyfaces[i].c_str(), &nW, &nH, &nC, 0);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
					nW, nH, 0, GL_RGB, GL_UNSIGNED_BYTE, pChImg);
				stbi_image_free(pChImg);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		return textureId;
	}
};

#endif
