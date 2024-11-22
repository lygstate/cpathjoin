#include <cpj.h>
#include <stdlib.h>

int is_relative_relative_windows(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (!cpj_path_is_relative("..\\hello\\world.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_relative_relative_drive(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (!cpj_path_is_relative("C:test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_relative_device_question_mark(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (cpj_path_is_relative("\\\\?\\mydevice\\test")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_relative_device_dot(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);
  cpj_path_is_absolute("\\\\.\\mydevice\\test");

  if (cpj_path_is_relative("\\\\.\\mydevice\\test")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_relative_device_unc(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (cpj_path_is_relative("\\\\.\\UNC\\LOCALHOST\\c$\\temp\\test-file.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_relative_unc(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (cpj_path_is_relative("\\\\server\\folder\\data")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_relative_absolute_drive(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (cpj_path_is_relative("C:\\test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_relative_unix_drive(void)
{
  cpj_path_set_style(CPJ_STYLE_UNIX);

  if (!cpj_path_is_relative("C:\\test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_relative_unix_backslash(void)
{
  cpj_path_set_style(CPJ_STYLE_UNIX);

  if (!cpj_path_is_relative("\\folder\\")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_relative_windows_slash(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (cpj_path_is_relative("/test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_relative_windows_backslash(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (cpj_path_is_relative("\\test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_relative_relative(void)
{
  cpj_path_set_style(CPJ_STYLE_UNIX);

  if (!cpj_path_is_relative("test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_relative_absolute(void)
{
  cpj_path_set_style(CPJ_STYLE_UNIX);
  if (cpj_path_is_relative("/test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
