#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "src/libs/nob.h"

/* ---------- */
#ifdef __linux__
#define RAYLIB_PATH "./lib/raylib/raylib-5.5_linux_amd64"
#define RAYLIB_LINKER "-l:libraylib.a"
#elif defined(_WIN32) || defined(_WIN64)
#define RAYLIB_PATH "./lib/raylib/raylib-5.5_windows"
#define RAYLIB_LINKER "./lib/raylib/raylib-5.5_windows/lib/libraylib.a"
#elif defined(__APPLE__)
#define RAYLIB_PATH "./lib/raylib/raylib-5.5_macos"
#define RAYLIB_LINKER "./lib/raylib/raylib-5.5_macos/lib/libraylib.a"
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

static int is_c_source(const char *name) {
  size_t n = strlen(name);
  if (n < 2)
    return 0;
  char a = name[n - 2], b = name[n - 1];
  return (a == '.' && (b == 'c' || b == 'C'));
}

static void collect_c_files(Nob_Cmd *cmd, const char *dirpath) {
  DIR *dir = opendir(dirpath);
  if (!dir)
    return;

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    const char *name = entry->d_name;
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
      continue;

    char full[PATH_MAX];
    int wrote = snprintf(full, sizeof(full), "%s/%s", dirpath, name);
    if (wrote < 0 || (size_t)wrote >= sizeof(full)) {
      continue;
    }

    struct stat st;
    if (stat(full, &st) != 0)
      continue;

    if (S_ISDIR(st.st_mode)) {
      collect_c_files(cmd, full);
    } else {
      if (is_c_source(name)) {
        builder_inputs(cmd, full);
      }
    }
  }

  closedir(dir);
}
#define builder_collect_sources(cmd, root) collect_c_files_recursive(cmd, root)

int main(int argc, char *argv[]) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  Nob_Cmd cmd = {0};

  builder_cc(&cmd);
  builder_output(&cmd, "build/main");
  builder_inputs(&cmd, "src/main.c");
  builder_libs(&cmd);
  builder_flags(&cmd);
  builder_macos_frameworks(&cmd);

  if (!cmd_run_sync_and_reset(&cmd))
    return 1;

  const char *program_name = shift(argv, argc);

  if (argc > 0) {
    const char *subcommand = shift(argv, argc);

    if (strcmp(subcommand, "run") == 0) {
      cmd_append(&cmd, "./build/main");
      if (!cmd_run_sync_and_reset(&cmd))
        return 1;
    } else {
      nob_log(ERROR, "Unknown command: %s", subcommand);
      return 1;
    }
  }

  return 0;
}
