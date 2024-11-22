#include <cpj.h>
#include <memory.h>
#include <stdlib.h>

int dirname_relative(void)
{
  const char *path;
  size_t length;

  cpj_path_set_style(CPJ_STYLE_UNIX);
  path = "../one/two.txt";
  cpj_path_get_dirname(path, &length);

  if (length != 7) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int dirname_three_segments(void)
{
  const char *path;
  size_t length;

  cpj_path_set_style(CPJ_STYLE_UNIX);
  path = "/one/two/three.txt";
  cpj_path_get_dirname(path, &length);

  if (length != 9) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int dirname_root(void)
{
  const char *path;
  size_t length;

  cpj_path_set_style(CPJ_STYLE_UNIX);
  path = "/";
  cpj_path_get_dirname(path, &length);

  if (length != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int dirname_special_directories(void)
{
  const char *path;
  size_t length;

  cpj_path_set_style(CPJ_STYLE_UNIX);
  path = "..";
  cpj_path_get_dirname(path, &length);

  if (length != 0) {
    return EXIT_FAILURE;
  }

  path = ".";
  cpj_path_get_dirname(path, &length);

  if (length != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int dirname_no_separators(void)
{
  const char *path;
  size_t length;

  cpj_path_set_style(CPJ_STYLE_UNIX);
  path = "file_name";
  cpj_path_get_dirname(path, &length);

  if (length != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int dirname_trailing_separators(void)
{
  const char *path;
  size_t length;

  cpj_path_set_style(CPJ_STYLE_UNIX);
  path = "/my/path.txt////";
  cpj_path_get_dirname(path, &length);

  if (length != 4) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int dirname_trailing_separator(void)
{
  const char *path;
  size_t length;

  cpj_path_set_style(CPJ_STYLE_UNIX);
  path = "/my/path.txt/";
  cpj_path_get_dirname(path, &length);

  if (length != 4) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int dirname_empty(void)
{
  const char *path;
  size_t length;

  cpj_path_set_style(CPJ_STYLE_UNIX);
  path = "";
  cpj_path_get_dirname(path, &length);

  if (length != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int dirname_simple(void)
{
  const char *path;
  size_t length;

  cpj_path_set_style(CPJ_STYLE_UNIX);
  path = "/my/path.txt";
  cpj_path_get_dirname(path, &length);

  if (length != 4) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
