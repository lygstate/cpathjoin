#include <cpj.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int join_multiple(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  const cpj_char_t *paths[3];
  cpj_size_t length;

  paths[0] = "hello/there";
  paths[1] = "../world";
  paths[2] = NULL;

  length = cpj_path_join_multiple(CPJ_STYLE_UNIX, paths, buffer, sizeof(buffer));

  if (length != 11) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "hello/world") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int join_relative_back_after_root(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_join(CPJ_STYLE_WINDOWS, "this\\", "C:\\..\\..\\is\\a\\test\\", buffer,
    sizeof(buffer));

  if (length != 9) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "is\\a\\test") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int join_back_after_root(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_join(CPJ_STYLE_WINDOWS, "C:\\this\\path", "C:\\..\\is\\a\\test\\", buffer,
    sizeof(buffer));

  if (length != 22) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "C:\\this\\path\\is\\a\\test") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int join_with_two_roots(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_join(CPJ_STYLE_WINDOWS, "C:\\this\\path", "C:\\is\\a\\test\\", buffer,
    sizeof(buffer));

  if (length != 25) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "C:\\this\\path\\C:\\is\\a\\test") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int join_two_unc(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_join(CPJ_STYLE_WINDOWS, "\\\\server\\unc\\path", "\\\\server2\\unc\\path",
    buffer, sizeof(buffer));

  if (length != 34) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "\\\\server\\unc\\path\\server2\\unc\\path") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int join_two_absolute(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_join(CPJ_STYLE_UNIX, "/first", "/second", buffer, sizeof(buffer));

  if (length != 13) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "/first/second") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int join_empty(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_join(CPJ_STYLE_UNIX, "hello", "..", buffer, sizeof(buffer));

  if (length != 1) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, ".") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int join_navigate_back(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_join(CPJ_STYLE_UNIX, "hello/there", "..", buffer, sizeof(buffer));

  if (length != 5) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "hello") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int join_simple(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_join(CPJ_STYLE_UNIX, "hello", "there", buffer, sizeof(buffer));

  if (length != 11) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "hello/there") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
