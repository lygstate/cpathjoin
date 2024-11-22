#include <cpj.h>
#include <stdlib.h>

int guess_empty_string(void)
{
  if (cpj_path_guess_style("") != CPJ_STYLE_UNIX) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int guess_unguessable(void)
{
  if (cpj_path_guess_style("myfile") != CPJ_STYLE_UNIX) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int guess_extension(void)
{
  if (cpj_path_guess_style("myfile.txt") != CPJ_STYLE_WINDOWS) {
    return EXIT_FAILURE;
  }

  if (cpj_path_guess_style("/a/directory/myfile.txt") != CPJ_STYLE_UNIX) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int guess_hidden_file(void)
{
  if (cpj_path_guess_style(".my_hidden_file") != CPJ_STYLE_UNIX) {
    return EXIT_FAILURE;
  }

  if (cpj_path_guess_style(".my_hidden_file.txt") != CPJ_STYLE_UNIX) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int guess_unix_separator(void)
{
  if (cpj_path_guess_style("/directory/other") != CPJ_STYLE_UNIX) {
    return EXIT_FAILURE;
  }

  if (cpj_path_guess_style("/directory/other.txt") != CPJ_STYLE_UNIX) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int guess_windows_separator(void)
{
  if (cpj_path_guess_style("\\directory\\other") != CPJ_STYLE_WINDOWS) {
    return EXIT_FAILURE;
  }
  if (cpj_path_guess_style("\\directory\\.other") != CPJ_STYLE_WINDOWS) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int guess_unix_root(void)
{
  if (cpj_path_guess_style("/directory") != CPJ_STYLE_UNIX) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int guess_windows_root(void)
{
  if (cpj_path_guess_style("C:\\test") != CPJ_STYLE_WINDOWS) {
    return EXIT_FAILURE;
  }

  if (cpj_path_guess_style("C:/test") != CPJ_STYLE_WINDOWS) {
    return EXIT_FAILURE;
  }

  if (cpj_path_guess_style("C:test") != CPJ_STYLE_WINDOWS) {
    return EXIT_FAILURE;
  }

  if (cpj_path_guess_style("C:/.test") != CPJ_STYLE_WINDOWS) {
    return EXIT_FAILURE;
  }

  if (cpj_path_guess_style("C:/folder/.test") != CPJ_STYLE_WINDOWS) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
