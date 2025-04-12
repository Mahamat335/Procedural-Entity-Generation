#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <stack>
#include <memory>
#include <Shader.h>
#include <Camera.h>
#include <Node.h>
#include <Transform.h>
#include <math.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <VAO.h>
#include <VBO.h>
#include <EBO.h>
#include <Texture.h>
#include <Game.h>
#include <ShapeType.h>
#include <ShapeRenderer.h>
#include <ShaderManager.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void ChangePolygonMode();

const unsigned int width = 1200;
const unsigned int height = 1200;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Game game;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(width, height, "Opengl", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	gladLoadGL();
	glViewport(0, 0, width, height);

	// POLYGONMODE
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	// vsynch
	// glfwSwapInterval(0);

	// Textures

	Shader shaderProgram = *(ShaderManager::Instance().defaultShaderProgram);
	ShapeRenderer::Instance().Setup();

	float lastCheck = 0;

	game.Start();
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if (lastCheck > 1 / 2)
		{
			std::string fps = std::to_string(1 / deltaTime);
			glfwSetWindowTitle(window, fps.c_str());
			processInput(window);
			lastCheck = 0;
		}
		lastCheck += deltaTime;

		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProgram.use();

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);

		shaderProgram.setVec3("viewPos", camera.Position);

		// retrieve the matrix uniform locations

		unsigned int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
		// pass them to the shaders (3 different ways)
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		unsigned int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		game.data.modelLoc = modelLoc;

		game.Update(deltaTime);

		glfwSwapBuffers(window);
		glfwPollEvents();

		// error catching
		/* GLenum err= glGetError();
		if (err != GL_NO_ERROR) {
			std::cerr << "OpenGL error: " << err << std::endl;
		} */
	}

	game.End();
	ShapeRenderer::Instance().Clear();
	shaderProgram.deleteProgram();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)
		camera.SetSpeed(camera.MovementSpeed - 0.1f);
	if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS)
		camera.SetSpeed(camera.MovementSpeed + 0.1f);
	if (glfwGetKey(window, GLFW_KEY_SLASH) == GLFW_PRESS)
		camera.SetSpeed();

	game.data.playerVel = glm::vec3(0.0f, 0.0f, 0.0f);
	game.data.playerAngularSpeed = 0;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		game.data.playerVel.y = game.data.playerSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		game.data.playerVel.y = -game.data.playerSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		game.data.playerVel.x = game.data.playerSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		game.data.playerVel.x = -game.data.playerSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		game.data.playerVel.z = game.data.playerSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		game.data.playerVel.z = -game.data.playerSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		game.data.playerAngularSpeed = 1.0f;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		game.data.playerAngularSpeed = -1.0f;

	if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS))
	{
		ChangePolygonMode();
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void ChangePolygonMode()
{
	if (game.data.polygonMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	game.data.polygonMode = !game.data.polygonMode;
}