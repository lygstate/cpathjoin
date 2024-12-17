#include "cpj_test.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int root_change_without_root(void)
{
  cpj_size_t length;
  cpj_char_t buffer[FILENAME_MAX] = "hello\\world.txt";

  length = cpj_path_change_root_test(CPJ_STYLE_WINDOWS, buffer, "D:\\", buffer,
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
  cpj_size_t length;
  cpj_char_t buffer[FILENAME_MAX] = "C:\\hello\\world.txt";

  length = cpj_path_change_root_test(CPJ_STYLE_WINDOWS, buffer, "D:\\path\\", buffer,
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
  cpj_size_t length;
  cpj_char_t buffer[FILENAME_MAX];

  length = cpj_path_change_root_test(CPJ_STYLE_WINDOWS, "C:\\hello\\world.txt", "D:\\path\\", buffer,
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
  cpj_size_t length;
  cpj_char_t buffer[FILENAME_MAX];

  length = cpj_path_change_root_test(CPJ_STYLE_WINDOWS, "C:\\hello\\world.txt", "", buffer,
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
  cpj_size_t length;
  cpj_char_t buffer[FILENAME_MAX];

  length = cpj_path_change_root_test(CPJ_STYLE_WINDOWS, "C:\\hello\\world.txt", "D:\\", buffer,
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
  cpj_size_t length;

  length = cpj_path_get_root(CPJ_STYLE_WINDOWS, "..\\hello\\world.txt");

  if (length != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_relative_drive(void)
{
  cpj_size_t length;

  length = cpj_path_get_root(CPJ_STYLE_WINDOWS, "C:test.txt");

  if (length != 2) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_device_question_mark(void)
{
  cpj_size_t length;

  length = cpj_path_get_root(CPJ_STYLE_WINDOWS, "\\\\?\\mydevice\\test");

  if (length != 4) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_device_dot(void)
{
  cpj_size_t length;

  length = cpj_path_get_root(CPJ_STYLE_WINDOWS, "\\\\.\\mydevice\\test");

  if (length != 4) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_device_unc(void)
{
  cpj_size_t length;

  length = cpj_path_get_root(CPJ_STYLE_WINDOWS, "\\\\.\\UNC\\LOCALHOST\\c$\\temp\\test-file.txt");

  if (length != 4) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_unc(void)
{
  cpj_size_t length;

  length = cpj_path_get_root(CPJ_STYLE_WINDOWS, "\\\\server\\folder\\data");

  if (length != 16) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_absolute_drive(void)
{
  cpj_size_t length;

  length = cpj_path_get_root(CPJ_STYLE_WINDOWS, "C:\\test.txt");

  if (length != 3) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_unix_drive(void)
{
  cpj_size_t length;

  length = cpj_path_get_root(CPJ_STYLE_UNIX, "C:\\test.txt");

  if (length != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_unix_backslash(void)
{
  cpj_size_t length;

  length = cpj_path_get_root(CPJ_STYLE_UNIX, "\\folder\\");

  if (length != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_windows_slash(void)
{
  cpj_size_t length;

  length = cpj_path_get_root(CPJ_STYLE_WINDOWS, "/test.txt");

  if (length != 1) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_windows_backslash(void)
{
  cpj_size_t length;

  length = cpj_path_get_root(CPJ_STYLE_WINDOWS, "\\test.txt");

  if (length != 1) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_relative(void)
{
  cpj_size_t length;

  length = cpj_path_get_root(CPJ_STYLE_UNIX, "test.txt");

  if (length != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int root_absolute(void)
{
  cpj_size_t length;

  length = cpj_path_get_root(CPJ_STYLE_UNIX, "/test.txt");

  if (length != 1) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
