#include "cpj_test.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

int relative_root_forward_slashes(void)
{
  cpj_char_t result[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_relative_test(CPJ_STYLE_WINDOWS, "C:\\foo\\bar\\baz\\", "C:/foo/bar/file.txt",
    result, sizeof(result));
  if (length != 11) {
    return EXIT_FAILURE;
  }

  if (strcmp(result, "..\\file.txt") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int relative_root_path_windows(void)
{
  cpj_char_t result[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_relative_test(CPJ_STYLE_WINDOWS, "C:\\this\\is\\path_one", "C:\\", result,
    sizeof(result));
  if (length != 8) {
    return EXIT_FAILURE;
  }

  if (strcmp(result, "..\\..\\..") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int relative_root_path_unix(void)
{
  cpj_char_t result[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_relative_test(CPJ_STYLE_UNIX, "/this/is/path_one", "/", result,
    sizeof(result));
  if (length != 8) {
    return EXIT_FAILURE;
  }

  if (strcmp(result, "../../..") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int relative_check(void)
{
  const cpj_char_t *relative_paths[] = {"..", "test", "test/test", "../another_test",
    "./simple", ".././simple"};
  const cpj_char_t *absolute_paths[] = {"/", "/test", "/../test/", "/../another_test",
    "/./simple", "/.././simple"};
  cpj_size_t i;

  for (i = 0; i < ARRAY_SIZE(relative_paths); ++i) {
    if (!cpj_path_is_relative(CPJ_STYLE_UNIX, relative_paths[i])) {
      return EXIT_FAILURE;
    }
  }

  for (i = 0; i < ARRAY_SIZE(absolute_paths); ++i) {
    if (cpj_path_is_relative(CPJ_STYLE_UNIX, absolute_paths[i])) {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

int relative_relative_and_absolute(void)
{
  cpj_char_t result[FILENAME_MAX];
  cpj_size_t length;

  *result = 1;

  length = cpj_path_get_relative_test(CPJ_STYLE_UNIX, "./foo", "/bar", result, sizeof(result));

  if (length != 0) {
    return EXIT_FAILURE;
  }

  if (*result != '\0') {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int relative_different_roots(void)
{
  cpj_char_t result[FILENAME_MAX];
  cpj_size_t length;

  *result = 1;

  length = cpj_path_get_relative_test(CPJ_STYLE_WINDOWS, "C:/path/same", "D:/path/same", result,
    sizeof(result));

  if (length != 0) {
    return EXIT_FAILURE;
  }

  if (*result != '\0') {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int relative_skip_all(void)
{
  cpj_char_t result[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_relative_test(CPJ_STYLE_UNIX, "/../../", "/../../", result, sizeof(result));
  if (length != 1) {
    return EXIT_FAILURE;
  }

  if (strcmp(result, ".") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int relative_target_div_skipped_end(void)
{
  cpj_char_t result[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_relative_test(CPJ_STYLE_UNIX, "/path/same", "/path/not_same/ho/..", result,
    sizeof(result));
  if (length != 11) {
    return EXIT_FAILURE;
  }

  if (strcmp(result, "../not_same") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int relative_base_div_skipped_end(void)
{
  cpj_char_t result[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_relative_test(CPJ_STYLE_UNIX, "/path/not_same/ho/..", "/path/same", result,
    sizeof(result));
  if (length != 7) {
    return EXIT_FAILURE;
  }

  if (strcmp(result, "../same") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int relative_target_skipped_end(void)
{
  cpj_char_t result[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_relative_test(CPJ_STYLE_UNIX, "/path/same", "/path/same/ho/..", result,
    sizeof(result));
  if (length != 1) {
    return EXIT_FAILURE;
  }

  if (strcmp(result, ".") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int relative_base_skipped_end(void)
{
  cpj_char_t result[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_relative_test(CPJ_STYLE_UNIX, "/path/same/ho/..", "/path/same", result,
    sizeof(result));
  if (length != 1) {
    return EXIT_FAILURE;
  }

  if (strcmp(result, ".") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int relative_equal(void)
{
  cpj_char_t result[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_relative_test(CPJ_STYLE_UNIX, "/path/same", "/path/same", result,
    sizeof(result));
  if (length != 1) {
    return EXIT_FAILURE;
  }

  if (strcmp(result, ".") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int relative_same_base(void)
{
  cpj_char_t result[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_relative_test(CPJ_STYLE_UNIX, "/dir/dir", "/dir/dir3/file", result,
    sizeof(result));
  if (length != 12) {
    return EXIT_FAILURE;
  }

  if (strcmp(result, "../dir3/file") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int relative_long_target(void)
{
  cpj_char_t result[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_relative_test(CPJ_STYLE_UNIX, "/path/long/one", "/path/long/one/two", result,
    sizeof(result));
  if (length != 3) {
    return EXIT_FAILURE;
  }

  if (strcmp(result, "two") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int relative_long_base(void)
{
  cpj_char_t result[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_relative_test(CPJ_STYLE_UNIX, "/path/long/one/two", "/path/long/one", result,
    sizeof(result));
  if (length != 2) {
    return EXIT_FAILURE;
  }

  if (strcmp(result, "..") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int relative_relative(void)
{
  cpj_char_t result[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_relative_test(CPJ_STYLE_UNIX, "./this/is/path_one", "./this/is/path_two",
    result, sizeof(result));
  if (length != 11) {
    return EXIT_FAILURE;
  }

  if (strcmp(result, "../path_two") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int relative_simple(void)
{
  cpj_char_t result[FILENAME_MAX];
  cpj_size_t length;

  length = cpj_path_get_relative_test(CPJ_STYLE_UNIX, "/this/is/path_one", "/this/is/path_two",
    result, sizeof(result));
  if (length != 11) {
    return EXIT_FAILURE;
  }

  if (strcmp(result, "../path_two") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
