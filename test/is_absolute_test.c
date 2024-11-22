#include <cpj.h>
#include <stdlib.h>

int is_absolute_relative_windows(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (cpj_path_is_absolute("..\\hello\\world.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_relative_drive(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (cpj_path_is_absolute("C:test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_device_question_mark(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (!cpj_path_is_absolute("\\\\?\\mydevice\\test")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_device_dot(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);
  cpj_path_is_absolute("\\\\.\\mydevice\\test");

  if (!cpj_path_is_absolute("\\\\.\\mydevice\\test")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_device_unc(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (!cpj_path_is_absolute("\\\\.\\UNC\\LOCALHOST\\c$\\temp\\test-file.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_unc(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (!cpj_path_is_absolute("\\\\server\\folder\\data")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_absolute_drive(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (!cpj_path_is_absolute("C:\\test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_unix_drive(void)
{
  cpj_path_set_style(CPJ_STYLE_UNIX);

  if (cpj_path_is_absolute("C:\\test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_unix_backslash(void)
{
  cpj_path_set_style(CPJ_STYLE_UNIX);

  if (cpj_path_is_absolute("\\folder\\")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_windows_slash(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (!cpj_path_is_absolute("/test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_windows_backslash(void)
{
  cpj_path_set_style(CPJ_STYLE_WINDOWS);

  if (!cpj_path_is_absolute("\\test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_relative(void)
{
  cpj_path_set_style(CPJ_STYLE_UNIX);

  if (cpj_path_is_absolute("test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_absolute(void)
{
  cpj_path_set_style(CPJ_STYLE_UNIX);
  if (!cpj_path_is_absolute("/test.txt")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_root(void)
{
  cpj_path_set_style(CPJ_STYLE_UNIX);
  if (!cpj_path_is_absolute("/")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int is_absolute_dir(void)
{
  cpj_path_set_style(CPJ_STYLE_UNIX);
  if (!cpj_path_is_absolute("/dir")) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
