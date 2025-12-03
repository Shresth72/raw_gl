#pragma once

#include <GLFW/glfw3.h>
#include <freetype2/ft2build.h>
#include <stdio.h>
#include <stdlib.h>

#include "../utils/utils.h"

typedef struct {
  int width;
  int height;
  const char *font_path;

  double lastTime;
  double currentTime;
  int numFrames;
} GameAppCreateInfo;

typedef enum { CONTINUE, QUIT } returnCode;

typedef struct {
  GLFWwindow *window;
  GameAppCreateInfo *appInfo;

  // Engine *engine;
  // Renderer *renderer;
} GameApp;

GameApp *game_app_create(GameAppCreateInfo *createInfo);
returnCode game_app_main_loop(GameApp *app);
void game_app_destroy(GameApp *app);
GLFWwindow *make_window(int width, int height);

void calculate_frame_rate(GameApp *app);

// Action callbacks
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods);
void key_callback(GLFWwindow *window, int button, int action, int mods);
