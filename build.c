#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "libs/nob.h"

/* ----- PLATFORM VARS ----- */
#ifdef __linux__
#define RAYLIB_PATH "./lib/raylib/raylib-5.5_linux_amd64"
#define RAYLIB_LINKER "-l:libraylib.a"
#define FREETYPE_INCLUDE_PATH "/usr/include/freetype2"
#define FREETYPE_LIBRARY_PATH "/usr/lib/x86_64-linux-gnu"
#elif defined(_WIN32) || defined(_WIN64)
#define RAYLIB_PATH "./lib/raylib/raylib-5.5_windows"
#define RAYLIB_LINKER "./lib/raylib/raylib-5.5_windows/lib/libraylib.a"
#error "Unsupported platform"
#elif defined(__APPLE__)
#define RAYLIB_PATH "./lib/raylib/raylib-5.5_macos"
#define RAYLIB_LINKER "./lib/raylib/raylib-5.5_macos/lib/libraylib.a"
#define FREETYPE_INCLUDE_PATH "/opt/homebrew/include/freetype2"
#define FREETYPE_LIBRARY_PATH "/opt/homebrew/lib"
#else
#error "Unsupported platform"
#endif

#ifdef __APPLE__
#define builder_macos_frameworks(cmd)                                          \
  cmd_append(cmd, "-framework", "Cocoa", "-framework", "IOKit", "-framework",  \
             "CoreFoundation", "-framework", "CoreGraphics")
#else
#define builder_macos_frameworks(cmd)
#endif
/* ---------- */

#define builder_cc(cmd) cmd_append(cmd, "cc")
#define builder_output(cmd, output_path) cmd_append(cmd, "-o", output_path)
#define builder_inputs(cmd, ...) cmd_append(cmd, __VA_ARGS__)
#define builder_libs(cmd) cmd_append(cmd, "-lm")
#define builder_flags(cmd)                                                     \
  cmd_append(cmd, "-Wall", "-Wextra", "-Wswitch-enum", "-ggdb")
#define builder_include_path(cmd, include_path)                                \
  cmd_append(cmd, temp_sprintf("-I%s", include_path))

/* ----- OPENGL ----- */
#define builder_opengl(cmd) cmd_append(cmd, "-lglfw", "-lGL", "-lGLEW", "-ldl")

/* ----- RAYLIB ----- */
#define builder_raylib(cmd)                                                    \
  cmd_append(cmd, temp_sprintf("-I%s/include/", RAYLIB_PATH),                  \
             temp_sprintf("-L%s/lib/", RAYLIB_PATH), RAYLIB_LINKER)

/* ----- FREETYPE2 ----- */
#define builder_freetype2(cmd)                                                 \
  cmd_append(cmd, temp_sprintf("-I%s", FREETYPE_INCLUDE_PATH),                 \
             temp_sprintf("-L%s", FREETYPE_LIBRARY_PATH), "-lfreetype")

/* ----- BUILD FILES ----- */
#define builder_inputs_list(cmd, files)                                        \
  do {                                                                         \
    for (int i = 0; files[i] != NULL; i++) {                                   \
      cmd_append(cmd, files[i]);                                               \
    }                                                                          \
  } while (0)

int main(int argc, char *argv[]) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  const char *BINARY = "build/main";
  const char *SRC_FILES[] = {
      "src/main.c",
      NULL,
  };

  const char *SPLINE_FILES[] = {
      "examples/splines/main.c",
      NULL,
  };

  Nob_Cmd cmd = {0};

  builder_cc(&cmd);
  builder_output(&cmd, BINARY);
  builder_inputs_list(&cmd, SRC_FILES);
  builder_libs(&cmd);
  builder_flags(&cmd);
  builder_opengl(&cmd);
  builder_raylib(&cmd);
  builder_freetype2(&cmd);
  builder_macos_frameworks(&cmd);

  if (!cmd_run_sync_and_reset(&cmd))
    return 1;

  const char *program_name = shift(argv, argc);

  if (argc > 0) {
    const char *subcommand = shift(argv, argc);

    if (strcmp(subcommand, "run") == 0) {
      cmd_append(&cmd, BINARY);
      if (!cmd_run_sync_and_reset(&cmd))
        return 1;
    } else {
      nob_log(ERROR, "Unknown command: %s", subcommand);
      return 1;
    }
  }

  return 0;
}
