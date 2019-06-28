#include "Camera.h"
#include "GameResource.h"
#include "GameMove.h"
#include "GameShoot.h"
#include "GameObject.h"
#include "PhysicsEngine.h"

#include <deque>

// settings
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 960;

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Movement
bool gunRaiseUp = false;

//Resouce Handler
ResourceManager ResM;
GameMove moveController;
GameShoot shootController;
PhysicsEngine physicsEngine;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// gun
bool firstTimeShowBullet = false;

// objectList
std::map<std::string, GameObject> targetList;
std::map<std::string, GameObject> movingTargetList;
std::map<std::string, GameObject> explodeTargeList;
std::map<std::string, bool> explodeTargeRec;
std::deque<std::string> recoverList;

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	moveController.humanMove(window, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	// camera view move horizon 
	moveController.humanRotate(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (gunRaiseUp) {
		//moveController.getHumanCamera()->ProcessMouseScroll(yoffset);
	}
}

void processMouseClick(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			// Éä»÷
			shootController.Fire();
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			gunRaiseUp = !gunRaiseUp;
		}
	}
}
