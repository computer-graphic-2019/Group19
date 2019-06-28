#include "GameShoot.h"

extern ResourceManager ResM;
extern GameMove moveController;
extern bool firstTimeShowBullet;
extern unsigned int SCR_WIDTH, SCR_HEIGHT;
extern bool gunRaiseUp;
extern unsigned int SCR_WIDTH, SCR_HEIGHT;

extern std::map<std::string, GameObject> targetList;
extern std::map<std::string, GameObject> movingTargetList;
extern std::map<std::string, GameObject> explodeTargeList;
extern std::map<std::string, bool> explodeTargeRec;
extern std::deque<std::string> recoverList;

struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;

GLuint textVAO = 0, textVBO = 0;

GameShoot::GameShoot() {
	this->point = 0;
}

// 控制开枪（鼠标左键）
void GameShoot::Fire() {
	// 开镜后才能开枪
	if (gunRaiseUp) {
		firstTimeShowBullet = true;
	}
}

void GameShoot::Shoot() {
	if (firstTimeShowBullet) {
		bulletInfo bif(GameObject(moveController.getHumanCamera()->getPosition(), glm::vec3(1.0f)),
			moveController.getHumanCamera()->getFrontVec(),
			false);
		this->bulletList.push_back(bif);
		firstTimeShowBullet = false;
	}
}

void GameShoot::showBullet(float deltaTime) {
	const float bulletSpeedRate = 2.0f;

	Shoot();

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	if (gunRaiseUp) {
		projection = glm::perspective(glm::radians(30.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	}
	
	ResM.getShader("model")->use();
	ResM.getShader("model")->setMat4("view", moveController.getHumanCamera()->getView());
	ResM.getShader("model")->setMat4("projection", projection);

	for (std::vector<bulletInfo>::iterator ptr = this->bulletList.begin(); ptr != this->bulletList.end(); ) {
		if (!ptr->isHit) {
			ptr->go.Position += bulletSpeedRate * ptr->direction;
			model = glm::mat4(1.0f);
			model = glm::translate(model, ptr->go.Position);
			model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
			ResM.getShader("model")->setMat4("model", model);
			ResM.getModel("bullet")->Draw((*ResM.getShader("model")));
			ptr++;
		}
		else {
			ptr = this->bulletList.erase(ptr);
		}
	}
}

// 检查命中位置
void GameShoot::CheckCollisionWithTarget() {
	for (int i = 0; i < this->bulletList.size(); i++) {
		// 固定靶子
		if (!this->bulletList[i].isHit) {
			for (std::map<std::string, GameObject>::iterator ptr = targetList.begin(); ptr != targetList.end(); ptr++) {
				if (ptr->second.CheckCollision(this->bulletList[i].go)) {
					this->bulletList[i].isHit = true;
					CalculateScore();
					break;
				}
			}
		}
		// 移动靶子
		if (!this->bulletList[i].isHit) {
			for (std::map<std::string, GameObject>::iterator ptr = movingTargetList.begin(); ptr != movingTargetList.end(); ptr++) {
				if (ptr->second.CheckCollision(this->bulletList[i].go)) {
					this->bulletList[i].isHit = true;
					CalculateScore();
					break;
				}
			}
		}
		// 有爆炸效果的靶子
		if (!this->bulletList[i].isHit) {
			for (std::map<std::string, GameObject>::iterator ptr = explodeTargeList.begin(); ptr != explodeTargeList.end(); ptr++) {
				if (!explodeTargeRec[ptr->first] && ptr->second.CheckCollision(this->bulletList[i].go)) {
					this->bulletList[i].isHit = true;
					explodeTargeRec[ptr->first] = true;
					recoverList.push_back(ptr->first);
					CalculateScore();
					break;
				}
			}
		}
	}
	char score[50];
	sprintf_s(score, 50, "Score: %d", this->point);
	std::string res = score;
	showScore(res, 25.0f, 25.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.2f));
};

// 计算命中得分
void GameShoot::CalculateScore() {
	this->point++;
};

void GameShoot::initialTextShader() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH), 0.0f, static_cast<GLfloat>(SCR_HEIGHT));
	ResM.getShader("textShader")->use();
	ResM.getShader("textShader")->setMat4("projection", projection);

	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void GameShoot::showScore(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
	// Activate corresponding render state
	ResM.getShader("textShader")->use();
	ResM.getShader("textShader")->setVec3("textColor", color);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);
	
	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update textVBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of textVBO memory
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
};