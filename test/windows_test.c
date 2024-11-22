#include <cpj.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>

int windows_root_empty(void)
{
  size_t size;
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  cpj_path_get_root("", &size);
  if (size != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_root_backslash(void)
{
  size_t size;
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  cpj_path_get_root("\\no_network_path\\hello", &size);
  if (size != 1) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_intersection_case(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (cpj_path_get_intersection("C:\\MYFOLDER\\MYILE.TXT",
        "c:\\myfolder\\myile.txt") != 21) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_get_root_relative(void)
{
  size_t size;
  cpj_path_set_style(CPJ_STYLE_WINDOWS);
  cpj_path_get_root("C:file.txt", &size);

  if (size != 2) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_get_root_separator(void)
{
  size_t size;
  cpj_path_set_style(CPJ_STYLE_WINDOWS);
  cpj_path_get_root("C:/this/is/a/test", &size);

  if (size != 3) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_get_unc_root(void)
{
  size_t size;

  cpj_path_set_style(CPJ_STYLE_WINDOWS);
  cpj_path_get_root("\\\\server\\share\\test.txt", &size);

  if (size != 15) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_get_root(void)
{
  size_t size;

  cpj_path_set_style(CPJ_STYLE_WINDOWS);
  cpj_path_get_root("C:\\test.txt", &size);

  if (size != 3) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_change_style(void)
{
  enum cpj_path_style style;

  style = cpj_path_get_style();
  if (style == CPJ_STYLE_WINDOWS) {
    cpj_path_set_style(CPJ_STYLE_UNIX);
    if (cpj_path_get_style() != CPJ_STYLE_UNIX) {
      return EXIT_FAILURE;
    }
  } else {
    cpj_path_set_style(CPJ_STYLE_WINDOWS);
    if (cpj_path_get_style() != CPJ_STYLE_WINDOWS) {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
