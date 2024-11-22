#include <cpj.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int root_change_without_root(void)
{
  size_t length;
  char buffer[FILENAME_MAX] = "hello\\world.txt";

  length = cpj_path_change_root(CPJ_STYLE_WINDOWS, buffer, "D:\\", buffer,
    sizeof(buffer));

  if (length != 18) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "D:\\hello\\world.txt") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_change_overlapping(void)
{
  size_t length;
  char buffer[FILENAME_MAX] = "C:\\hello\\world.txt";

  length = cpj_path_change_root(CPJ_STYLE_WINDOWS, buffer, "D:\\path\\", buffer,
    sizeof(buffer));

  if (length != 23) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "D:\\path\\hello\\world.txt") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_change_separators(void)
{
  size_t length;
  char buffer[FILENAME_MAX];

  length = cpj_path_change_root(CPJ_STYLE_WINDOWS, "C:\\hello\\world.txt", "D:\\path\\", buffer,
    sizeof(buffer));

  if (length != 23) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "D:\\path\\hello\\world.txt") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_change_empty(void)
{
  size_t length;
  char buffer[FILENAME_MAX];

  length = cpj_path_change_root(CPJ_STYLE_WINDOWS, "C:\\hello\\world.txt", "", buffer,
    sizeof(buffer));

  if (length != 15) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "hello\\world.txt") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_change_simple(void)
{
  size_t length;
  char buffer[FILENAME_MAX];

  length = cpj_path_change_root(CPJ_STYLE_WINDOWS, "C:\\hello\\world.txt", "D:\\", buffer,
    sizeof(buffer));

  if (length != 18) {
    return EXIT_FAILURE;
  }

  if (strcmp(buffer, "D:\\hello\\world.txt") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_relative_windows(void)
{
  size_t length;

  cpj_path_get_root(CPJ_STYLE_WINDOWS, "..\\hello\\world.txt", &length);

  if (length != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_relative_drive(void)
{
  size_t length;

  cpj_path_get_root(CPJ_STYLE_WINDOWS, "C:test.txt", &length);

  if (length != 2) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_device_question_mark(void)
{
  size_t length;

  cpj_path_get_root(CPJ_STYLE_WINDOWS, "\\\\?\\mydevice\\test", &length);

  if (length != 4) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_device_dot(void)
{
  size_t length;

  cpj_path_get_root(CPJ_STYLE_WINDOWS, "\\\\.\\mydevice\\test", &length);

  if (length != 4) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_device_unc(void)
{
  size_t length;

  cpj_path_get_root(CPJ_STYLE_WINDOWS, "\\\\.\\UNC\\LOCALHOST\\c$\\temp\\test-file.txt", &length);

  if (length != 4) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_unc(void)
{
  size_t length;

  cpj_path_get_root(CPJ_STYLE_WINDOWS, "\\\\server\\folder\\data", &length);

  if (length != 16) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_absolute_drive(void)
{
  size_t length;

  cpj_path_get_root(CPJ_STYLE_WINDOWS, "C:\\test.txt", &length);

  if (length != 3) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_unix_drive(void)
{
  size_t length;

  cpj_path_get_root(CPJ_STYLE_UNIX, "C:\\test.txt", &length);

  if (length != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_unix_backslash(void)
{
  size_t length;

  cpj_path_get_root(CPJ_STYLE_UNIX, "\\folder\\", &length);

  if (length != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_windows_slash(void)
{
  size_t length;

  cpj_path_get_root(CPJ_STYLE_WINDOWS, "/test.txt", &length);

  if (length != 1) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_windows_backslash(void)
{
  size_t length;

  cpj_path_get_root(CPJ_STYLE_WINDOWS, "\\test.txt", &length);

  if (length != 1) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_relative(void)
{
  size_t length;

  cpj_path_get_root(CPJ_STYLE_UNIX, "test.txt", &length);

  if (length != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_absolute(void)
{
  size_t length;

  cpj_path_get_root(CPJ_STYLE_UNIX, "/test.txt", &length);

  if (length != 1) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
