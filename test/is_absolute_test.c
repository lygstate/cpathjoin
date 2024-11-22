#include <cpj.h>
#include <stdlib.h>

int is_absolute_relative_windows(void)
{
  if (cpj_path_is_absolute(CPJ_STYLE_WINDOWS, "..\\hello\\world.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_relative_drive(void)
{
  if (cpj_path_is_absolute(CPJ_STYLE_WINDOWS, "C:test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_device_question_mark(void)
{
  if (!cpj_path_is_absolute(CPJ_STYLE_WINDOWS, "\\\\?\\mydevice\\test")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_device_dot(void)
{
  cpj_path_is_absolute("\\\\.\\mydevice\\test");

  if (!cpj_path_is_absolute(CPJ_STYLE_WINDOWS, "\\\\.\\mydevice\\test")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_device_unc(void)
{
  if (!cpj_path_is_absolute(CPJ_STYLE_WINDOWS, "\\\\.\\UNC\\LOCALHOST\\c$\\temp\\test-file.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_unc(void)
{
  if (!cpj_path_is_absolute(CPJ_STYLE_WINDOWS, "\\\\server\\folder\\data")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_absolute_drive(void)
{
  if (!cpj_path_is_absolute(CPJ_STYLE_WINDOWS, "C:\\test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_unix_drive(void)
{
  if (cpj_path_is_absolute(CPJ_STYLE_UNIX, "C:\\test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_unix_backslash(void)
{
  if (cpj_path_is_absolute(CPJ_STYLE_UNIX, "\\folder\\")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_windows_slash(void)
{
  if (!cpj_path_is_absolute(CPJ_STYLE_WINDOWS, "/test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_windows_backslash(void)
{
  if (!cpj_path_is_absolute(CPJ_STYLE_WINDOWS, "\\test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_relative(void)
{
  if (cpj_path_is_absolute(CPJ_STYLE_UNIX, "test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_absolute(void)
{
  if (!cpj_path_is_absolute(CPJ_STYLE_UNIX, "/test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_root(void)
{
  if (!cpj_path_is_absolute(CPJ_STYLE_UNIX, "/")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_dir(void)
{
  if (!cpj_path_is_absolute(CPJ_STYLE_UNIX, "/dir")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
