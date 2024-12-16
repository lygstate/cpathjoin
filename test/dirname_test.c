#include "cpj_test.h"
#include <memory.h>
#include <stdlib.h>

int dirname_relative(void)
{
  const cpj_char_t *path;
  cpj_size_t length;

  path = "../one/two.txt";
  cpj_path_get_dirname_test(CPJ_STYLE_UNIX, path, &length);

  if (length != 7) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int dirname_three_segments(void)
{
  const cpj_char_t *path;
  cpj_size_t length;

  path = "/one/two/three.txt";
  cpj_path_get_dirname_test(CPJ_STYLE_UNIX, path, &length);

  if (length != 9) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int dirname_root(void)
{
  const cpj_char_t *path;
  cpj_size_t length;

  path = "/";
  cpj_path_get_dirname_test(CPJ_STYLE_UNIX, path, &length);

  if (length != 1) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int dirname_special_directories(void)
{
  const cpj_char_t *path;
  cpj_size_t length;

  path = "..";
  cpj_path_get_dirname_test(CPJ_STYLE_UNIX, path, &length);

  if (length != 0) {
    return EXIT_FAILURE;
  }

  path = ".";
  cpj_path_get_dirname_test(CPJ_STYLE_UNIX, path, &length);

  if (length != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int dirname_no_separators(void)
{
  const cpj_char_t *path;
  cpj_size_t length;

  path = "file_name";
  cpj_path_get_dirname_test(CPJ_STYLE_UNIX, path, &length);

  if (length != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int dirname_trailing_separators(void)
{
  const cpj_char_t *path;
  cpj_size_t length;

  path = "/my/path.txt////";
  cpj_path_get_dirname_test(CPJ_STYLE_UNIX, path, &length);

  if (length != 4) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int dirname_trailing_separator(void)
{
  const cpj_char_t *path;
  cpj_size_t length;

  path = "/my/path.txt/";
  cpj_path_get_dirname_test(CPJ_STYLE_UNIX, path, &length);

  if (length != 4) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int dirname_empty(void)
{
  const cpj_char_t *path;
  cpj_size_t length;

  path = "";
  cpj_path_get_dirname_test(CPJ_STYLE_UNIX, path, &length);

  if (length != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int dirname_simple(void)
{
  const cpj_char_t *path;
  cpj_size_t length;

  path = "/my/path.txt";
  cpj_path_get_dirname_test(CPJ_STYLE_UNIX, path, &length);

  if (length != 4) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
