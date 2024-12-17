#include "cpj_test.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int basename_change_trim_only_root(void)
{
  cpj_size_t n;
  cpj_char_t buffer[FILENAME_MAX];

  n = cpj_path_change_basename_test(CPJ_STYLE_UNIX, "/", "///another.txt///", buffer,
    sizeof(buffer));
  if (n != 12) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "/another.txt") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int basename_change_trim(void)
{
  cpj_size_t n;
  cpj_char_t buffer[FILENAME_MAX];

  n = cpj_path_change_basename_test(CPJ_STYLE_UNIX, "/test.txt", "///another.txt///", buffer,
    sizeof(buffer));
  if (n != 12) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "/another.txt") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int basename_change_relative(void)
{
  cpj_size_t n;
  cpj_char_t buffer[FILENAME_MAX];

  n = cpj_path_change_basename_test(CPJ_STYLE_WINDOWS, "../test.txt", "another.txt", buffer,
    sizeof(buffer));
  if (n != 14) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "../another.txt") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int basename_change_empty_basename(void)
{
  cpj_size_t n;
  cpj_char_t buffer[FILENAME_MAX];

  n = cpj_path_change_basename_test(CPJ_STYLE_WINDOWS, "C:\\test.txt", "", buffer, sizeof(buffer));
  if (n != 3) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "C:\\") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int basename_change_only_root(void)
{
  cpj_size_t n;
  cpj_char_t buffer[FILENAME_MAX];

  n = cpj_path_change_basename_test(CPJ_STYLE_WINDOWS, "C:\\", "another.txt", buffer, sizeof(buffer));
  if (n != 14) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "C:\\another.txt") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int basename_change_empty_path(void)
{
  cpj_size_t n;
  cpj_char_t buffer[FILENAME_MAX];

  n = cpj_path_change_basename_test(CPJ_STYLE_WINDOWS, "", "another.txt", buffer, sizeof(buffer));
  if (n != 11) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "another.txt") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int basename_change_simple(void)
{
  cpj_size_t n;
  cpj_char_t buffer[FILENAME_MAX];

  n = cpj_path_change_basename_test(CPJ_STYLE_WINDOWS, "C:\\test.txt", "another.txt", buffer,
    sizeof(buffer));
  if (n != 14) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "C:\\another.txt") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int basename_windows(void)
{
  const cpj_char_t *path, *basename;
  cpj_size_t length;

  path = "C:\\path\\test.txt";
  cpj_path_get_basename_test(CPJ_STYLE_WINDOWS, path, &basename, &length);

  if (length != 8) {
    return EXIT_FAILURE;
  }

  if (strcmp(basename, "test.txt") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int basename_root(void)
{
  const cpj_char_t *path, *basename;
  cpj_size_t length;

  path = "/";
  cpj_path_get_basename_test(CPJ_STYLE_UNIX, path, &basename, &length);

  if (length != 0) {
    return EXIT_FAILURE;
  }

  if (basename != NULL) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int basename_special_directories(void)
{
  const cpj_char_t *path, *basename;
  cpj_size_t length;

  path = "..";
  cpj_path_get_basename_test(CPJ_STYLE_UNIX, path, &basename, &length);

  if (length != 2) {
    return EXIT_FAILURE;
  }

  if (strncmp(basename, "..", length) != 0) {
    return EXIT_FAILURE;
  }

  path = ".";
  cpj_path_get_basename_test(CPJ_STYLE_UNIX, path, &basename, &length);

  if (length != 1) {
    return EXIT_FAILURE;
  }

  if (strncmp(basename, ".", length) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int basename_no_separators(void)
{
  const cpj_char_t *path, *basename;
  cpj_size_t length;

  path = "file_name";
  cpj_path_get_basename_test(CPJ_STYLE_UNIX, path, &basename, &length);

  if (length != 9) {
    return EXIT_FAILURE;
  }

  if (strncmp(basename, "file_name", length) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int basename_trailing_separators(void)
{
  const cpj_char_t *path, *basename;
  cpj_size_t length;

  path = "/my/path.txt////";
  cpj_path_get_basename_test(CPJ_STYLE_UNIX, path, &basename, &length);

  if (length != 8) {
    return EXIT_FAILURE;
  }

  if (strncmp(basename, "path.txt", length) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int basename_trailing_separator(void)
{
  const cpj_char_t *path, *basename;
  cpj_size_t length;

  path = "/my/path.txt/";
  cpj_path_get_basename_test(CPJ_STYLE_UNIX, path, &basename, &length);

  if (length != 8) {
    return EXIT_FAILURE;
  }

  if (strncmp(basename, "path.txt", length) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int basename_empty(void)
{
  const cpj_char_t *path, *basename;
  cpj_size_t length;

  path = "";
  cpj_path_get_basename_test(CPJ_STYLE_UNIX, path, &basename, &length);

  if (length != 0) {
    return EXIT_FAILURE;
  }

  if (basename != NULL) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int basename_simple(void)
{
  const cpj_char_t *path, *basename;
  cpj_size_t length;

  path = "/my/path.txt";
  cpj_path_get_basename_test(CPJ_STYLE_UNIX, path, &basename, &length);

  if (length != 8) {
    return EXIT_FAILURE;
  }

  if (strncmp(basename, "path.txt", length) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
