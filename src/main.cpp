#include "AnalyticsManager.h"
#include "ConfigManager.h"
#include <Camera.h>
#include <EBO.h>
#include <GLFW/glfw3.h>
#include <Game.h>
#include <Node.h>
#include <Shader.h>
#include <ShaderManager.h>
#include <ShapeRenderer.h>
#include <ShapeType.h>
#include <Texture.h>
#include <Transform.h>
#include <VAO.h>
#include <VBO.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <math.h>
#include <stb/stb_image.h>
#include <vector>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void ChangePolygonMode();
void ChangeCursorStatus(GLFWwindow *window);
void ImGuiUpdate();

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

// config
char saveFileName[64] = "MyConfig";
std::string selectedConfig = "";
std::vector<std::string> configFiles;
bool needRefresh = true;

// ImGui analytics icin state
std::string selectedRunFile = ""; // Name degil File
int selectedRunIndex = -1;
std::vector<std::string> runFiles;
bool needAnalyticsRefresh = true;

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(width, height, "Opengl", NULL, NULL);
  if (window == NULL) {
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
  Shader shadowMapProgram = *(ShaderManager::Instance().shadowMapShaderProgram);

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

  // Create Frame Buffer Object
  unsigned int FBO;
  glGenFramebuffers(1, &FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  // Create Framebuffer Texture
  unsigned int framebufferTexture;
  glGenTextures(1, &framebufferTexture);
  glBindTexture(GL_TEXTURE_2D, framebufferTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                  GL_CLAMP_TO_EDGE); // Prevents edge bleeding
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                  GL_CLAMP_TO_EDGE); // Prevents edge bleeding

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         framebufferTexture, 0);
  glDrawBuffer(GL_NONE); // No color buffer is drawn to
  glReadBuffer(GL_NONE); // No color buffer is drawn to
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
  // Matrices needed for the light's perspective
  glm::mat4 orthgonalProjection =
      glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 100.0f);
  glm::mat4 lightView =
      glm::lookAt(50.0f * lightPos, glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 lightProjection = orthgonalProjection * lightView;

  shadowMapProgram.use();
  glUniformMatrix4fv(
      glGetUniformLocation(shadowMapProgram.ID, "lightProjection"), 1, GL_FALSE,
      glm::value_ptr(lightProjection));

  // config init
  ConfigManager::Instance().Initialize();

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    if (lastCheck > 0.5f) {
      std::string fps = "FPS: " + std::to_string(1 / deltaTime);
      glfwSetWindowTitle(window, fps.c_str());
      lastCheck = 0;
    }
    processInput(window);
    lastCheck += deltaTime;

    // Depth testing needed for Shadow Map
    glEnable(GL_DEPTH_TEST);

    // Preparations for the Shadow Map
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Draw scene for shadow map
    unsigned int shadowModelLoc =
        glGetUniformLocation(shadowMapProgram.ID, "model");
    game.RenderEntities(shadowModelLoc, shadowMapProgram, true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // imgui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (!io.WantCaptureMouse) {
      // u can call processInput() here for disable inputs
      // io.WantCaptureKeyboard is smt.
    }

    // Shadow

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
    projection = glm::perspective(glm::radians(camera.Zoom),
                                  (float)width / (float)height, 0.1f, 100.0f);
    game.data.view = view;
    game.data.projection = projection;
    shaderProgram.setVec3("viewPos", camera.Position);

    // retrieve the matrix uniform locations

    unsigned int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
    // pass them to the shaders (3 different ways)
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    // note: currently we set the projection matrix each frame, but since the
    // projection matrix rarely changes it's often best practice to set it
    // outside the main loop only once.
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "projection"), 1,
                       GL_FALSE, glm::value_ptr(projection));

    // shadow
    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram.ID, "lightProjection"), 1, GL_FALSE,
        glm::value_ptr(lightProjection));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture);
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "shadowMap"), 0);

    unsigned int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
    game.data.modelLoc = modelLoc;

    game.Update(deltaTime);

    // shadow clear
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // imgui
    ImGuiUpdate();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();

    // error catching
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
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
  shadowMapProgram.deleteProgram();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
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

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
    ChangeCursorStatus(window);
  }
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
/*
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
 */
void ChangeCursorStatus(GLFWwindow *window) {
  if (game.data.isCursorEnabled) {

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  } else {

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  game.data.isCursorEnabled = !game.data.isCursorEnabled;
  camera.CameraLock = !camera.CameraLock;
}

void ImGuiUpdate() {
  if (game.data.isCursorEnabled) {
    ImGui::Begin("Game Settings");

    // ---------------------------------------------------------
    // BOLUM 1: CONFIGURATION MANAGER (SAVE / LOAD)
    // ---------------------------------------------------------
    if (ImGui::CollapsingHeader("Configuration Manager",
                                ImGuiTreeNodeFlags_DefaultOpen)) {
      // Dosya listesini guncelle
      if (needRefresh) {
        configFiles = ConfigManager::Instance().GetConfigFileList();
        needRefresh = false;
      }

      // SAVE
      ImGui::Text("Save Configuration:");
      ImGui::InputText("##savefile", saveFileName, IM_ARRAYSIZE(saveFileName));
      ImGui::SameLine();
      if (ImGui::Button("Save")) {
        ConfigManager::Instance().SaveConfig(saveFileName, game.data);
        needRefresh = true; // Listeyi yenile ki yeni dosya gozuksun
      }

      // LOAD
      ImGui::Spacing();
      ImGui::Text("Load Configuration:");

      // Dropdown Menu
      const char *preview =
          selectedConfig.empty() ? "Select Config..." : selectedConfig.c_str();
      if (ImGui::BeginCombo("##loadfile", preview)) {
        for (const auto &file : configFiles) {
          bool isSelected = (selectedConfig == file);
          if (ImGui::Selectable(file.c_str(), isSelected)) {
            selectedConfig = file;
          }
          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }

      ImGui::SameLine();
      if (ImGui::Button("Load") && !selectedConfig.empty()) {
        ConfigManager::Instance().LoadConfig(selectedConfig, game.data);

        // İstersen Load işleminden sonra dünyayı otomatik yenilemek için:
        // game.InitializeSpiders();
        // game.InitializeCaterpillars();
        // game.InitializeProducers();
        // Ama şimdilik manuel butona basarak yenilemek daha güvenli olabilir.
      }
    }

    ImGui::Separator();

    // ---------------------------------------------------------
    // BOLUM 2: SIMULATION PARAMETERS (SENİN ESKİ KODLARIN)
    // ---------------------------------------------------------

    if (ImGui::Button("Start Simulation")) {
      AnalyticsManager::Instance().EndCurrentRun(glfwGetTime());
      size_t count = AnalyticsManager::Instance().GetRunFileList().size();
      std::string runName = "Run_" + std::to_string(count + 1);
      AnalyticsManager::Instance().StartNewRun(game.data, runName);

      game.InitializeSpiders();
      game.InitializeCaterpillars();
      game.InitializeProducers();

      std::cout << "[ImGui] Started simulation with " << runName << std::endl;
    }

    ImGui::SameLine();
    if (ImGui::Button("End Current Simulation")) {
      AnalyticsManager::Instance().EndCurrentRun(glfwGetTime());
    }

    ImGui::Text("General Settings");
    ImGui::Checkbox("Polygon Mode", &game.data.polygonMode);
    ImGui::Checkbox("Show Colliders", &game.data.showColliders);
    ImGui::Checkbox("Spider Movement", &game.data.areSpidersMoving);

    ImGui::Separator();

    ImGui::Text("Spider Settings");
    ImGui::InputInt("Spider Count",
                    &game.data.spiderGenerationData.EntityCount);
    ImGui::InputInt("Min Leg Pair Count",
                    &game.data.spiderGenerationData.LegPairCountMin);
    ImGui::InputInt("Max Leg Pair Count",
                    &game.data.spiderGenerationData.LegPairCountMax);
    ImGui::InputFloat("Min Move Speed",
                      &game.data.spiderGenerationData.MoveSpeedMin);
    ImGui::InputFloat("Max Move Speed",
                      &game.data.spiderGenerationData.MoveSpeedMax);
    ImGui::InputFloat("Min Upper Leg Scale",
                      &game.data.spiderGenerationData.UpperLegSizeScaleMin);
    ImGui::InputFloat("Max Upper Leg Scale",
                      &game.data.spiderGenerationData.UpperLegSizeScaleMax);
    ImGui::InputFloat("Min Middle Leg Scale",
                      &game.data.spiderGenerationData.MiddleLegSizeScaleMin);
    ImGui::InputFloat("Max Middle Leg Scale",
                      &game.data.spiderGenerationData.MiddleLegSizeScaleMax);
    ImGui::InputFloat("Min Lower Leg Scale",
                      &game.data.spiderGenerationData.LowerLegSizeScaleMin);
    ImGui::InputFloat("Max Lower Leg Scale",
                      &game.data.spiderGenerationData.LowerLegSizeScaleMax);
    if (ImGui::Button("Initialize Spiders")) {
      game.InitializeSpiders();
    }

    ImGui::Separator();

    ImGui::Text("Caterpillar Settings");
    ImGui::InputInt("Caterpillars Count",
                    &game.data.caterpillarGenerationData.EntityCount);
    ImGui::InputInt("Caterpillars Min Segment Count",
                    &game.data.caterpillarGenerationData.LegPairCountMin);
    ImGui::InputInt("Caterpillars Max SegmentCount",
                    &game.data.caterpillarGenerationData.LegPairCountMax);
    ImGui::InputFloat("Caterpillars Min Move Speed",
                      &game.data.caterpillarGenerationData.MoveSpeedMin);
    ImGui::InputFloat("Caterpillars Max Move Speed",
                      &game.data.caterpillarGenerationData.MoveSpeedMax);
    if (ImGui::Button("Initialize Caterpillars")) {
      game.InitializeCaterpillars();
    }

    ImGui::Separator();

    ImGui::Text("Producer Settings");
    ImGui::InputInt("Producers Count",
                    &game.data.producerGenerationData.EntityCount);
    if (ImGui::Button("Initialize Producers")) {
      game.InitializeProducers();
    }

    ImGui::End();

    // BOLUM 3: ANALYTICS WINDOW (YENI PENCERE)
    // ---------------------------------------------------------

    ImGui::SetNextWindowPos(ImVec2(width - 400, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(380, 500), ImGuiCond_FirstUseEver);

    ImGui::Begin("Simulation Analytics"); // Yeni bir pencere baslatiyoruz

    // 1. CANLI DURUM
    const auto *currentRun =
        AnalyticsManager::Instance().GetCurrentRunResults();
    if (currentRun) {
      // ... (Bu kisim ayni) ...
      // Grafikleri canli gormek istersen buraya da PlotLines ekleyebilirsin
      // Ama simdilik snapshot sayisini yazdiralim
      ImGui::Text("Data Points Collected: %d",
                  (int)currentRun->Snapshots.size());
    }

    // 2. GECMIS KOSULAR (DOSYADAN)
    if (ImGui::CollapsingHeader("Historical Runs (Files)",
                                ImGuiTreeNodeFlags_DefaultOpen)) {
      // Listeyi yenile (Refresh butonu veya flag ile)
      if (ImGui::Button("Refresh List") || needAnalyticsRefresh) {
        runFiles = AnalyticsManager::Instance().GetRunFileList();
        needAnalyticsRefresh = false;
      }

      const char *preview = selectedRunFile.empty() ? "Select Run File..."
                                                    : selectedRunFile.c_str();
      if (ImGui::BeginCombo("##selectrun", preview)) {
        for (int i = 0; i < runFiles.size(); ++i) {
          bool isSelected = (selectedRunFile == runFiles[i]);
          if (ImGui::Selectable(runFiles[i].c_str(), isSelected)) {
            selectedRunFile = runFiles[i];
            // DOSYAYI YUKLE
            AnalyticsManager::Instance().LoadRun(selectedRunFile);
          }
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
    }

    // 3. YUKLENEN KOSUNUN GRAFIKLERI
    const auto *loadedRun = AnalyticsManager::Instance().GetLoadedRunResults();

    if (loadedRun) {
      ImGui::Separator();
      ImGui::Text("Loaded: %s", loadedRun->RunName.c_str());
      ImGui::Text("Duration: %.2f s", loadedRun->TotalRunTime);
      // ... Olum istatistikleri vs ...

      ImGui::Separator();

      // GRAFIKLER
      const auto &spiders = AnalyticsManager::Instance().PlotSpiderCounts;
      const auto &caterpillars =
          AnalyticsManager::Instance().PlotCaterpillarCounts;

      if (!spiders.empty()) {
        // Max deger bulma
        float max_val = 0.0f;
        for (float v : spiders)
          if (v > max_val)
            max_val = v;
        for (float v : caterpillars)
          if (v > max_val)
            max_val = v;
        max_val = std::max(max_val, 10.0f);

        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Red: Spiders");
        ImGui::PlotLines("##s_graph", spiders.data(), (int)spiders.size(), 0,
                         NULL, 0.0f, max_val, ImVec2(0, 80));

        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Green: Caterpillars");
        ImGui::PlotLines("##c_graph", caterpillars.data(),
                         (int)caterpillars.size(), 0, NULL, 0.0f, max_val,
                         ImVec2(0, 80));
      } else {
        ImGui::TextDisabled("No data points available in this file.");
      }
    }

    ImGui::End(); // Simulation Analytics penceresi bitti
  }
}