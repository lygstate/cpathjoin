#include "cpj_test.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int extension_change_with_trailing_slash(void)
{
  cpj_char_t buffer[FILENAME_MAX] = "/folder/file.txt/";
  cpj_size_t n;

  n = cpj_path_change_extension_test(CPJ_STYLE_UNIX, buffer, ".md", buffer, sizeof(buffer));
  if (n != 15) {
    return EXIT_FAILURE;
  }

  if (strcmp("/folder/file.md", buffer) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int extension_change_hidden_file(void)
{
  cpj_char_t buffer[FILENAME_MAX] = "/folder/.hidden_file.txt";
  cpj_size_t n;

  n = cpj_path_change_extension_test(CPJ_STYLE_UNIX, buffer, ".md", buffer, sizeof(buffer));
  if (n != 23) {
    return EXIT_FAILURE;
  }

  if (strcmp("/folder/.hidden_file.md", buffer) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int extension_change_overlap_long(void)
{
  cpj_char_t buffer[FILENAME_MAX] = "/test.txt/";
  cpj_size_t n;

  n = cpj_path_change_extension_test(CPJ_STYLE_UNIX, buffer, ".text", buffer, sizeof(buffer));
  if (n != 10) {
    return EXIT_FAILURE;
  }

  if (strcmp("/test.text", buffer) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int extension_change_overlap(void)
{
  cpj_char_t buffer[FILENAME_MAX] = "/test.txt/";
  cpj_size_t n;

  n = cpj_path_change_extension_test(CPJ_STYLE_UNIX, buffer, ".md", buffer, sizeof(buffer));
  if (n != 8) {
    return EXIT_FAILURE;
  }

  if (strcmp("/test.md", buffer) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int extension_change_with_dot(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t n;

  n = cpj_path_change_extension_test(CPJ_STYLE_UNIX, "/test.txt", ".md", buffer, sizeof(buffer));
  if (n != 8) {
    return EXIT_FAILURE;
  }

  if (strcmp("/test.md", buffer) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int extension_change_remove(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t n;

  n = cpj_path_change_extension_test(CPJ_STYLE_UNIX, "/test.txt", "", buffer, sizeof(buffer));
  if (n != 5) {
    return EXIT_FAILURE;
  }

  if (strcmp("/test", buffer) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int extension_change_no_extension(void)
{
  cpj_char_t buffer[FILENAME_MAX] = "C:\\file";
  cpj_size_t n;

  n = cpj_path_change_extension_test(CPJ_STYLE_WINDOWS, buffer, ".md", buffer, sizeof(buffer));
  if (n != 10) {
    return EXIT_FAILURE;
  }

  if (strcmp("C:\\file.md", buffer) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int extension_change_no_basename(void)
{
  cpj_char_t buffer[FILENAME_MAX] = "C:\\";
  cpj_size_t n;

  n = cpj_path_change_extension_test(CPJ_STYLE_WINDOWS, buffer, ".md", buffer, sizeof(buffer));
  if (n != 6) {
    return EXIT_FAILURE;
  }

  if (strcmp("C:\\.md", buffer) != 0) {
    return EXIT_FAILURE;
  }

  strcpy(buffer, "C:");
  n = cpj_path_change_extension_test(CPJ_STYLE_WINDOWS, buffer, ".md", buffer, sizeof(buffer));
  if (n != 5) {
    return EXIT_FAILURE;
  }

  if (strcmp("C:.md", buffer) != 0) {
    return EXIT_FAILURE;
  }

  strcpy(buffer, "C:");
  n = cpj_path_change_extension_test(CPJ_STYLE_WINDOWS, buffer, "md", buffer, sizeof(buffer));
  if (n != 5) {
    return EXIT_FAILURE;
  }

  if (strcmp("C:.md", buffer) != 0) {
    return EXIT_FAILURE;
  }

  strcpy(buffer, "C:\\");
  n = cpj_path_change_extension_test(CPJ_STYLE_WINDOWS, buffer, "md", buffer, sizeof(buffer));
  if (n != 6) {
    return EXIT_FAILURE;
  }

  if (strcmp("C:\\.md", buffer) != 0) {
    return EXIT_FAILURE;
  }

  strcpy(buffer, "C:/");
  n = cpj_path_change_extension_test(CPJ_STYLE_WINDOWS, buffer, "md", buffer, sizeof(buffer));
  if (n != 6) {
    return EXIT_FAILURE;
  }

  if (strcmp("C:\\.md", buffer) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int extension_change_simple(void)
{
  cpj_char_t buffer[FILENAME_MAX];
  cpj_size_t n;

  n = cpj_path_change_extension_test(CPJ_STYLE_UNIX, "/test.txt", "md", buffer, sizeof(buffer));
  if (n != 8) {
    return EXIT_FAILURE;
  }

  if (strcmp("/test.md", buffer) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int extension_check_without(void)
{
  if (cpj_path_has_extension_test(CPJ_STYLE_UNIX, "/my/path")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int extension_check_empty(void)
{
  if (cpj_path_has_extension_test(CPJ_STYLE_UNIX, "")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int extension_check_simple(void)
{
  if (!cpj_path_has_extension_test(CPJ_STYLE_UNIX, "/my/path.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int extension_get_multiple(void)
{
  const cpj_char_t *path, *extension;
  cpj_size_t length;

  path = "/my/path.abc.txt.tests";
  if (!cpj_path_get_extension_test(CPJ_STYLE_UNIX, path, &extension, &length)) {
    return EXIT_FAILURE;
  }

  if (length != 6) {
    return EXIT_FAILURE;
  }

  if (strncmp(extension, ".tests", length) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int extension_get_last(void)
{
  const cpj_char_t *path, *extension;
  cpj_size_t length;

  path = "/my/path.";
  if (!cpj_path_get_extension_test(CPJ_STYLE_UNIX, path, &extension, &length)) {
    return EXIT_FAILURE;
  }

  if (length != 1) {
    return EXIT_FAILURE;
  }

  if (strncmp(extension, ".", length) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int extension_get_first(void)
{
  const cpj_char_t *path, *extension;
  cpj_size_t length;

  path = "/my/.path";
  if (!cpj_path_get_extension_test(CPJ_STYLE_UNIX, path, &extension, &length)) {
    return EXIT_FAILURE;
  }

  if (length != 5) {
    return EXIT_FAILURE;
  }

  if (strncmp(extension, ".path", length) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int extension_get_without(void)
{
  const cpj_char_t *path, *extension;
  cpj_size_t length;

  path = "/my/path";
  if (cpj_path_get_extension_test(CPJ_STYLE_UNIX, path, &extension, &length)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int extension_get_simple(void)
{
  const cpj_char_t *path, *extension;
  cpj_size_t length;

  path = "/my/path.txt";
  if (!cpj_path_get_extension_test(CPJ_STYLE_UNIX, path, &extension, &length)) {
    return EXIT_FAILURE;
  }

  if (length != 4) {
    return EXIT_FAILURE;
  }

  if (strncmp(extension, ".txt", length) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
