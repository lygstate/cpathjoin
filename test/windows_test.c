#include <cwalk.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>

int windows_get_root_relative()
{
  size_t size;
  cwk_path_set_style(CWK_STYLE_WINDOWS);
  cwk_path_get_root("C:file.txt", &size);

  if (size != 2) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_get_root_separator()
{
  size_t size;
  cwk_path_set_style(CWK_STYLE_WINDOWS);
  cwk_path_get_root("C:/this/is/a/test", &size);

  if (size != 3) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_get_unc_root()
{
  size_t size;

  cwk_path_set_style(CWK_STYLE_WINDOWS);
  cwk_path_get_root("\\\\server\\share\\test.txt", &size);

  if (size != 9) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_get_root()
{
  size_t size;

  cwk_path_set_style(CWK_STYLE_WINDOWS);
  cwk_path_get_root("C:\\test.txt", &size);

  if (size != 3) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int windows_change_style()
{
  enum cwk_path_style style;

  style = cwk_path_get_style();
  if (style == CWK_STYLE_WINDOWS) {
    cwk_path_set_style(CWK_STYLE_UNIX);
    if (cwk_path_get_style() != CWK_STYLE_UNIX) {
      return EXIT_FAILURE;
    }
  } else {
    cwk_path_set_style(CWK_STYLE_WINDOWS);
    if (cwk_path_get_style() != CWK_STYLE_WINDOWS) {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}