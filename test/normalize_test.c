#include "cpj_test.h"
#include <limits.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int normalize_forward_slashes(void)
{
  cpj_size_t count;
  cpj_char_t result[FILENAME_MAX];
  cpj_char_t *input, *expected;

  input = "C:/this/is/a/test/path";
  strcpy(result, input);
  expected = "C:\\this\\is\\a\\test\\path";
  count = cpj_path_normalize_test(CPJ_STYLE_WINDOWS, result, result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int normalize_back_after_root(void)
{
  cpj_size_t count;
  cpj_char_t result[FILENAME_MAX];
  cpj_char_t *input, *expected;

  input = "C:\\..\\this\\is\\a\\test\\path";
  strcpy(result, input);
  expected = "C:\\this\\is\\a\\test\\path";
  count = cpj_path_normalize_test(CPJ_STYLE_WINDOWS, result, result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int normalize_only_separators(void)
{
  cpj_size_t count;
  cpj_char_t result[FILENAME_MAX];
  cpj_char_t *input, *expected;

  input = "////";
  strcpy(result, input);
  expected = "/";
  count = cpj_path_normalize_test(CPJ_STYLE_UNIX, result, result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int normalize_zero_length(void)
{
  cpj_size_t count;
  cpj_char_t result[FILENAME_MAX];
  cpj_char_t *input, *expected;

  input = "";
  strcpy(result, input);
  expected = ".";
  count = cpj_path_normalize_test(CPJ_STYLE_UNIX, input, result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int normalize_empty(void)
{
  cpj_size_t count;
  cpj_char_t result[FILENAME_MAX];
  cpj_char_t *input, *expected;

  input = "test/..";
  strcpy(result, input);
  expected = ".";
  count = cpj_path_normalize_test(CPJ_STYLE_UNIX, result, result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int normalize_overlap(void)
{
  cpj_size_t count;
  cpj_char_t result[FILENAME_MAX];
  cpj_char_t *input, *expected;

  input = "/var/./logs/.//test/..//..//////";
  strcpy(result, input);
  expected = "/var";
  count = cpj_path_normalize_test(CPJ_STYLE_UNIX, result, result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int normalize_mixed(void)
{
  cpj_size_t count;
  cpj_char_t result[FILENAME_MAX];
  cpj_char_t *input, *expected;

  input = "/var/./logs/.//test/..//..//////";
  expected = "/var";
  count = cpj_path_normalize_test(CPJ_STYLE_UNIX, input, result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int normalize_remove_current(void)
{
  cpj_size_t count;
  cpj_char_t result[FILENAME_MAX];
  cpj_char_t *input, *expected;

  input = "/var/././././";
  expected = "/var";
  count = cpj_path_normalize_test(CPJ_STYLE_UNIX, input, result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int normalize_double_separator(void)
{
  cpj_size_t count;
  cpj_char_t result[FILENAME_MAX];
  cpj_char_t *input, *expected;

  input = "/var////logs//test/";
  expected = "/var/logs/test";
  count = cpj_path_normalize_test(CPJ_STYLE_UNIX, input, result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int normalize_terminated(void)
{
  cpj_size_t count;
  cpj_char_t result[FILENAME_MAX];
  cpj_char_t *input, *expected;
  cpj_size_t i, expected_size, n;

  input = "/var/logs/test/../../";
  expected = "/var";
  expected_size = strlen(expected);

  memset(result, 1, sizeof(result));

  for (i = 0; i < 7; ++i) {
    count = cpj_path_normalize_test(CPJ_STYLE_UNIX, input, result, i);

    if (i != 0 && expected_size < i) {
      n = expected_size;
    } else {
      n = i - 1;
    }

    if (count != strlen(expected) ||
        (i > 0 && (strncmp(result, expected, n) != 0 || result[n] != '\0'))) {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

int normalize_relative_too_far(void)
{
  cpj_size_t count;
  cpj_char_t result[FILENAME_MAX];
  cpj_char_t *expected;

  expected = "..";
  count = cpj_path_normalize_test(CPJ_STYLE_UNIX, CPJ_ZSTR_LITERAL("rel/../../"), result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  expected = "C:..\\..";
  count = cpj_path_normalize_test(CPJ_STYLE_WINDOWS, CPJ_ZSTR_LITERAL("C:rel/../../../"), result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  expected = "C:.";
  count = cpj_path_normalize_test(CPJ_STYLE_WINDOWS, CPJ_ZSTR_LITERAL("C:rel/../"), result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  expected = "C:.";
  count = cpj_path_normalize_test(CPJ_STYLE_WINDOWS, CPJ_ZSTR_LITERAL("C:"), result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  expected = "C:\\";
  count = cpj_path_normalize_test(CPJ_STYLE_WINDOWS, CPJ_ZSTR_LITERAL("C:/"), result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int normalize_absolute_too_far(void)
{
  cpj_size_t count;
  cpj_char_t result[FILENAME_MAX];
  cpj_char_t *input, *expected;

  input = "/var/logs/test/../../../../../../";
  expected = "/";
  count = cpj_path_normalize_test(CPJ_STYLE_UNIX, input, result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int normalize_navigate_back(void)
{
  cpj_size_t count;
  cpj_char_t result[FILENAME_MAX];
  cpj_char_t *input, *expected;

  input = "/var/logs/test/../../";
  expected = "/var";
  count = cpj_path_normalize_test(CPJ_STYLE_UNIX, input, result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int normalize_do_nothing(void)
{
  cpj_size_t count;
  cpj_char_t result[FILENAME_MAX];
  cpj_char_t *input, *expected;

  input = "/var";
  expected = "/var";
  count = cpj_path_normalize_test(CPJ_STYLE_UNIX, input, result, sizeof(result));
  if (count != strlen(expected) || strcmp(result, expected) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
