#include "cpj_test.h"
#include <memory.h>
#include <stdlib.h>
#include <string.h>

int windows_root_empty(void)
{
  cpj_size_t size;
  size = cpj_path_get_root(CPJ_STYLE_WINDOWS, "");
  if (size != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_root_backslash(void)
{
  cpj_size_t size;
  size = cpj_path_get_root(CPJ_STYLE_WINDOWS, "\\no_network_path\\hello");
  if (size != 1) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_intersection_case(void)
{
  if (cpj_path_get_intersection_test(CPJ_STYLE_WINDOWS, "C:\\MYFOLDER\\MYILE.TXT",
        "c:\\myfolder\\myile.txt") != 21) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_get_root_relative(void)
{
  cpj_size_t size;
  size = cpj_path_get_root(CPJ_STYLE_WINDOWS, "C:file.txt");

  if (size != 2) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_get_root_separator(void)
{
  cpj_size_t size;
  size = cpj_path_get_root(CPJ_STYLE_WINDOWS, "C:/this/is/a/test");

  if (size != 3) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_get_unc_root(void)
{
  cpj_size_t size;

  size = cpj_path_get_root(CPJ_STYLE_WINDOWS, "\\\\server\\share\\test.txt");

  if (size != 15) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_get_root(void)
{
  cpj_size_t size;

  size = cpj_path_get_root(CPJ_STYLE_WINDOWS, "C:\\test.txt");

  if (size != 3) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
