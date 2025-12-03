#include "game_app.h"
#include <GLFW/glfw3.h>

GameApp *game_app_create(GameAppCreateInfo *createInfo) {
  GameApp *app = (GameApp *)malloc(sizeof(GameApp));
  app->appInfo = createInfo;

  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    free(app);
    return NULL;
  }

  app->window = make_window(app->appInfo->width, app->appInfo->height);
  if (!app->window) {
    GLCall(glfwTerminate());
    free(app);
    return NULL;
  }

  GLCall(glfwSetWindowUserPointer(app->window, app));
  GLCall(
      glfwSetFramebufferSizeCallback(app->window, framebuffer_size_callback));
  GLCall(glfwSetMouseButtonCallback(app->window, mouse_button_callback));
  GLCall(glfwSetKeyCallback(app->window, key_callback));

  glfwSwapInterval(20);

  // TODO: Renderer and Engine

  GLCall(app->appInfo->lastTime = glfwGetTime());
  app->appInfo->currentTime = app->appInfo->lastTime;
  app->appInfo->numFrames = 0;

  return app;
}

returnCode game_app_main_loop(GameApp *app) {
  calculate_frame_rate(app);

  // TODO: Update mouse position in Engine
  // GLCall(glfwGetCursorPos(app->window, &app->renderer->mouseX,
  // &app->renderer->mouseY));

  GLCall(glViewport(0, 0, app->appInfo->width, app->appInfo->height));
  GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

  // TODO: Engine render

  GLCall(glfwSwapBuffers(app->window));
  GLCall(glfwPollEvents());

  if (glfwWindowShouldClose(app->window)) {
    return QUIT;
  }
  return CONTINUE;
}

void game_app_destroy(GameApp *app) {
  // engine_destroy(app->renderer);
  GLCall(glfwDestroyWindow(app->window));
  GLCall(glfwTerminate());
  free(app);
}

void calculate_frame_rate(GameApp *app) {
  GLCall(app->appInfo->currentTime = glfwGetTime());
  app->appInfo->numFrames++;
  if (app->appInfo->currentTime - app->appInfo->lastTime >= 1.0) {
    printf("\rFPS: %d", app->appInfo->numFrames);
    fflush(stdout);
    app->appInfo->numFrames = 0;
    app->appInfo->lastTime += 1.0;
  }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  GLCall(GameApp *app = (GameApp *)glfwGetWindowUserPointer(window));
  GLCall(glViewport(0, 0, width, height));
  app->appInfo->width = width;
  app->appInfo->height = height;
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  GLCall(GameApp *app = (GameApp *)glfwGetWindowUserPointer(window));
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    // TODO: Left Mouse callback in Engine
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    // TODO:
  }

  // NOTE: If needed handle right button
}

void key_callback(GLFWwindow *window, int button, int action, int mods) {
  GLCall(GameApp *app = (GameApp *)glfwGetWindowUserPointer(window));
}
