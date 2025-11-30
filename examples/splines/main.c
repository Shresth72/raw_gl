#include "raster.c"

int render_font(Spline *spline) {
  FT_Library library = {0};

  FT_Error error = FT_Init_FreeType(&library);
  if (error) {
    fprintf(stderr, "Error initializing FreeType library: %d\n", error);
    return 1;
  }

  const char *const font_file_path = "assets/fonts/ProtoNerdFont.ttf";

  FT_Face face;
  error = FT_New_Face(library, font_file_path, 0, &face);
  if (error == FT_Err_Unknown_File_Format) {
    fprintf(stderr, "ERROR: `%s` has an unknown format\n", font_file_path);
    return 1;
  } else if (error) {
    fprintf(stderr, "ERROR: Could not load file `%s`\n", font_file_path);
    return 1;
  }

  error = FT_Load_Char(face, 'E', FT_LOAD_DEFAULT);
  if (error) {
    fprintf(stderr, "ERROR: Could not load char glyph\n");
    return 1;
  }

  long int min_x = LONG_MAX, max_x = LONG_MIN;
  long int min_y = LONG_MAX, max_y = LONG_MIN;

  for (int i = 0; i < face->glyph->outline.n_points; ++i) {
    FT_Vector p = face->glyph->outline.points[i];
    if (min_x > p.x)
      min_x = p.x;
    if (max_x < p.x)
      max_x = p.x;
    if (min_y > p.y)
      min_y = p.y;
    if (max_y < p.y)
      max_y = p.y;
  }

  float scale = 0.5f;
  int pindex = 0;

  for (int i = 0; i < face->glyph->outline.n_contours; ++i) {
    for (; pindex <= face->glyph->outline.contours[i]; ++pindex) {
      FT_Vector p = face->glyph->outline.points[pindex];
      float x = (p.x - min_x) * scale + 100;
      float y = (max_y - p.y) * scale + 100;
      Vector2 pos = {x, y};
      da_append(spline, pos);
    }
  }

  return 0;
}

int main() {

  // Spline spline_ = {0};
  // int error = render_font(&spline_);
  // if (error != 0)
  //   return 1;
  // render_spline_into_grid(&spline_);

  int dragging = -1;
  Spline spline = {0};
  render_spline_into_grid(&spline);

  size_t factor = 80;
  InitWindow(16 * factor, 9 * factor, "font");
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(GetColor(0x181818));
    display_grid();

    if (IsKeyPressed(KEY_C)) {
      spline.count = 0;
      memset(grid, 0, sizeof(grid));
    }
    edit_control_points(&spline, &dragging);
    EndDrawing();
  }
  CloseWindow();

  return 0;
}
