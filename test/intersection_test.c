#include "cpj_test.h"
#include <stdlib.h>

int intersection_skipped_end(void)
{
  if (cpj_path_get_intersection_test(CPJ_STYLE_UNIX, "/test/foo/har/../", "/test/foo/ho") != 9) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int intersection_relative_other(void)
{
  if (cpj_path_get_intersection_test(CPJ_STYLE_UNIX, "/test/foo/har", "/test/abc/../foo/bar") != 9) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int intersection_relative_base(void)
{
  if (cpj_path_get_intersection_test(CPJ_STYLE_UNIX, "/test/abc/../foo/bar", "/test/foo/har") !=
      16) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int intersection_one_root_only(void)
{
  if (cpj_path_get_intersection_test(CPJ_STYLE_WINDOWS, "C:/abc/test.txt", "C:/") != 3) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int intersection_same_roots(void)
{
  if (cpj_path_get_intersection_test(CPJ_STYLE_WINDOWS, "C:/abc/test.txt", "C:/def/test.txt") != 3) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int intersection_relative_absolute_mix(void)
{
  if (cpj_path_get_intersection_test(CPJ_STYLE_UNIX, "/test/abc.txt", "test/abc.txt") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int intersection_unequal_roots(void)
{
  if (cpj_path_get_intersection_test(CPJ_STYLE_WINDOWS, "C:/test/abc.txt", "D:/test/abc.txt") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int intersection_empty(void)
{
  if (cpj_path_get_intersection_test(CPJ_STYLE_UNIX, "/", "") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int intersection_double_separator(void)
{
  if (cpj_path_get_intersection_test(CPJ_STYLE_UNIX, "/this///is/a//test", "/this//is/a///file") !=
      12) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int intersection_trailing_separator(void)
{
  if (cpj_path_get_intersection_test(CPJ_STYLE_UNIX, "/this/is/a/test", "/this/is/a/") != 10) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int intersection_truncated(void)
{
  if (cpj_path_get_intersection_test(CPJ_STYLE_UNIX, "/this/is/a/test", "/this/is/a") != 10) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int intersection_simple(void)
{
  if (cpj_path_get_intersection_test(CPJ_STYLE_UNIX, "/this/is/a/test", "/this/is/a/string") != 10) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
