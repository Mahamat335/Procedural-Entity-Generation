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
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void ChangePolygonMode();
void ChangeCursorStatus(GLFWwindow *window);

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
	Shader shadowMapShaderProgram = *(ShaderManager::Instance().shadowMapShaderProgram);

	ShapeRenderer::Instance().Setup();

	// imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	float lastCheck = 0;

	game.Start();

	// Shadow Calculations

	unsigned int shadowMapFBO;
	glGenFramebuffers(1, &shadowMapFBO);
	unsigned int shadowMapWidth = 2048, shadowMapHeight = 2048;
	unsigned int shadowMap;
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = {1.0, 1.0, 1.0, 1.0};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm::vec3 lightPos = glm::vec3(0.5f, 0.2f, 0.5f);
	glm::mat4 orthogonalProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
	glm::mat4 lightView = glm::lookAt(20.0f * lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = orthogonalProjection * lightView;

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

		// imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (!io.WantCaptureMouse)
		{
			// u can call processInput() here for disable inputs
			// io.WantCaptureKeyboard is smt.
		}

		// Shadow

		// render scene from light's point of view
		shadowMapShaderProgram.use();
		glUniformMatrix4fv(glGetUniformLocation(shadowMapShaderProgram.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

		glViewport(0, 0, shadowMapWidth, shadowMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		unsigned int depthModel = glGetUniformLocation(shadowMapShaderProgram.ID, "model");
		game.RenderEntities(depthModel, shadowMapShaderProgram);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// we already enabled it: glEnable(GL_DEPTH_TEST);
		/* glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT); */

		// Camera Rendering

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

		// shadow

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		shaderProgram.setInt("shadowMap", 0);

		unsigned int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		game.data.modelLoc = modelLoc;

		game.Update(deltaTime);

		// shadow clear
		// glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// imgui
		if (game.data.isCursorEnabled)
		{
			ImGui::Begin("title?");
			ImGui::Text("hi");
			ImGui::SliderFloat("Move Speed: ", &game.data.moveSpeed, 0.0f, 1.0f);
			if (ImGui::Checkbox("Polygon Mode", &game.data.polygonMode))
			{
				ChangePolygonMode();
			}
			ImGui::InputInt("Spider Count", &game.data.spiderGenerationData.SpiderCount);
			ImGui::InputInt("Min Leg Count", &game.data.spiderGenerationData.LegCountMin);
			ImGui::InputInt("Max Leg Count", &game.data.spiderGenerationData.LegCountMax);
			ImGui::InputFloat("Min Move Speed", &game.data.spiderGenerationData.MoveSpeedMin);
			ImGui::InputFloat("Max Move Speed", &game.data.spiderGenerationData.MoveSpeedMax);
			ImGui::InputFloat("Min Leg Scale", &game.data.spiderGenerationData.LegScaleMin);
			ImGui::InputFloat("Max Leg Scale", &game.data.spiderGenerationData.LegScaleMax);
			ImGui::InputFloat("Min Upper Leg Scale", &game.data.spiderGenerationData.UpperLegSizeScaleMin);
			ImGui::InputFloat("Max Upper Leg Scale", &game.data.spiderGenerationData.UpperLegSizeScaleMax);
			ImGui::InputFloat("Min Middle Leg Scale", &game.data.spiderGenerationData.MiddleLegSizeScaleMin);
			ImGui::InputFloat("Max Middle Leg Scale", &game.data.spiderGenerationData.MiddleLegSizeScaleMax);
			ImGui::InputFloat("Min Lower Leg Scale", &game.data.spiderGenerationData.LowerLegSizeScaleMin);
			ImGui::InputFloat("Max Lower Leg Scale", &game.data.spiderGenerationData.LowerLegSizeScaleMax);
			if (ImGui::Button("Initialize Spiders"))
			{
				game.InitializeSpiders();
			}
			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();

		// error catching
		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			std::cerr << "OpenGL error: " << err << std::endl;
		}
	}

	// imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	game.End();
	ShapeRenderer::Instance().Clear();
	shaderProgram.deleteProgram();
	shadowMapShaderProgram.deleteProgram();
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

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		ChangeCursorStatus(window);
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
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void ChangeCursorStatus(GLFWwindow *window)
{
	if (game.data.isCursorEnabled)
	{

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else
	{

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	game.data.isCursorEnabled = !game.data.isCursorEnabled;
	camera.CameraLock = !camera.CameraLock;
}