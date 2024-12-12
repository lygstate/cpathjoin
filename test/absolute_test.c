#include <cpj.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

int absolute_check(void)
{
  const cpj_char_t *relative_paths[] = {"..", "test", "test/test", "../another_test",
    "./simple", ".././simple"};
  const cpj_char_t *absolute_paths[] = {"/", "/test", "/../test/", "/../another_test",
    "/./simple", "/.././simple"};
  cpj_size_t i;

  for (i = 0; i < ARRAY_SIZE(relative_paths); ++i) {
    if (cpj_path_is_absolute(CPJ_STYLE_UNIX, relative_paths[i])) {
      return EXIT_FAILURE;
    }
  }

  for (i = 0; i < ARRAY_SIZE(absolute_paths); ++i) {
    if (!cpj_path_is_absolute(CPJ_STYLE_UNIX, absolute_paths[i])) {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

int absolute_too_far(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_absolute(CPJ_STYLE_UNIX, "/hello/there", "../../../../../", buffer,
    sizeof(buffer));

  if (length != 1) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "/") != 0) {
    return EXIT_FAILURE;
  }

  length = cpj_path_get_absolute(CPJ_STYLE_WINDOWS, "C:/hello/there", "../../../../../", buffer,
    sizeof(buffer));

  if (length != 3) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "C:\\") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int absolute_normalization(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_absolute(CPJ_STYLE_UNIX, "/hello//../there", "test//thing", buffer,
    sizeof(buffer));

  if (length != 17) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "/there/test/thing") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int absolute_mixed(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_absolute(CPJ_STYLE_UNIX, "hello/there", "/test", buffer,
    sizeof(buffer));

  if (length != 5) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "/test") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int absolute_unix_relative_base(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_absolute(CPJ_STYLE_UNIX, "hello/there", "test", buffer, sizeof(buffer));

  if (length != 17) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "/hello/there/test") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int absolute_windows_relative_base(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_absolute(CPJ_STYLE_WINDOWS, "hello\\there", "test", buffer, sizeof(buffer));

  if (length != 17) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "\\hello\\there\\test") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int absolute_absolute_path(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_absolute(CPJ_STYLE_UNIX, "/hello/there", "/test", buffer,
    sizeof(buffer));

  if (length != 5) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "/test") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int absolute_simple(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_absolute(CPJ_STYLE_UNIX, "/hello/there", "..", buffer, sizeof(buffer));

  if (length != 6) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "/hello") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}


int absolute_buffer_reuse(void)
{
  cpj_char_t path[FILENAME_MAX];

  memset(path, 1, FILENAME_MAX);
  path[0] = '\0';

  cpj_path_get_absolute(CPJ_STYLE_UNIX, path, "/", path, FILENAME_MAX);
  if (strcmp(path, "/") != 0) {
    return EXIT_FAILURE;
  }
  cpj_path_get_absolute(CPJ_STYLE_UNIX, path, "see", path, FILENAME_MAX);
  if (strcmp(path, "/see") != 0) {
    return EXIT_FAILURE;
  }

  cpj_path_get_absolute(CPJ_STYLE_UNIX, path, "dog", path, FILENAME_MAX);
  if (strcmp(path, "/see/dog") != 0) {
    return EXIT_FAILURE;
  }

  cpj_path_get_absolute(CPJ_STYLE_UNIX, path, "..", path, FILENAME_MAX);
  if (strcmp(path, "/see") != 0) {
    return EXIT_FAILURE;
  }

  cpj_path_get_absolute(CPJ_STYLE_UNIX, path, "cat", path, FILENAME_MAX);
  if (strcmp(path, "/see/cat") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
