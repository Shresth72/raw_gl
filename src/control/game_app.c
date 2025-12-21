#include "game_app.h"

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

GLFWwindow *make_window(int width, int height) {
  GLCall(glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3));
  GLCall(glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3));
  GLCall(glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE));

  GLCall(glfwWindowHint(GLFW_DECORATED, GLFW_FALSE));

  GLCall(GLFWwindow *window =
             glfwCreateWindow(width, height, "GAME WINDOW", NULL, NULL));
  if (!window) {
    GLCall(glfwTerminate());
    return NULL;
  }

  GLCall(glfwMakeContextCurrent(window));
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return NULL;
  }

  return window;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  GameApp *app = (GameApp *)glfwGetWindowUserPointer(window);

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
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
