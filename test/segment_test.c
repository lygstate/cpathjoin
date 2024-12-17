#include "cpj_test.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int segment_change_overlap(void)
{
  cpj_char_t buffer[FILENAME_MAX] = "C:\\this\\cool\\path\\";
  struct cpj_segment segment;

  if (!cpj_path_get_first_segment(CPJ_STYLE_WINDOWS, buffer, &segment)) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_next_segment(CPJ_STYLE_WINDOWS, &segment)) {
    return EXIT_FAILURE;
  }

  cpj_path_change_segment(CPJ_STYLE_WINDOWS, &segment, "longer_segment", buffer, sizeof(buffer));

  if (strcmp(buffer, "C:\\this\\longer_segment\\path\\") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_change_with_separator(void)
{
  const cpj_char_t *path;
  cpj_char_t buffer[FILENAME_MAX];
  struct cpj_segment segment;

  path = "C:\\this\\cool\\path\\";
  if (!cpj_path_get_first_segment(CPJ_STYLE_WINDOWS, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_next_segment(CPJ_STYLE_WINDOWS, &segment)) {
    return EXIT_FAILURE;
  }

  cpj_path_change_segment(CPJ_STYLE_WINDOWS, &segment, "other\\fancy", buffer, sizeof(buffer));

  if (strcmp(buffer, "C:\\this\\other\\fancy\\path\\") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_change_empty(void)
{
  const cpj_char_t *path;
  cpj_char_t buffer[FILENAME_MAX];
  struct cpj_segment segment;

  path = "C:\\this\\cool\\path\\";
  if (!cpj_path_get_first_segment(CPJ_STYLE_WINDOWS, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_next_segment(CPJ_STYLE_WINDOWS, &segment)) {
    return EXIT_FAILURE;
  }

  cpj_path_change_segment(CPJ_STYLE_WINDOWS, &segment, "", buffer, sizeof(buffer));

  if (strcmp(buffer, "C:\\this\\\\path\\") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_change_trim(void)
{
  const cpj_char_t *path;
  cpj_char_t buffer[FILENAME_MAX];
  struct cpj_segment segment;

  path = "C:\\this\\cool\\path\\";
  if (!cpj_path_get_first_segment(CPJ_STYLE_WINDOWS, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_next_segment(CPJ_STYLE_WINDOWS, &segment)) {
    return EXIT_FAILURE;
  }

  cpj_path_change_segment(CPJ_STYLE_WINDOWS, &segment, "//other/\\\\", buffer, sizeof(buffer));

  if (strcmp(buffer, "C:\\this\\other\\path\\") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_change_last(void)
{
  const cpj_char_t *path;
  cpj_char_t buffer[FILENAME_MAX];
  struct cpj_segment segment;

  path = "C:\\this\\cool\\path\\";
  if (!cpj_path_get_last_segment(CPJ_STYLE_WINDOWS, path, &segment)) {
    return EXIT_FAILURE;
  }

  cpj_path_change_segment(CPJ_STYLE_WINDOWS, &segment, "other", buffer, sizeof(buffer));

  if (strcmp(buffer, "C:\\this\\cool\\other\\") != 0) {
    return EXIT_FAILURE;
  }

  path = "C:\\this\\cool\\path";
  if (!cpj_path_get_last_segment(CPJ_STYLE_WINDOWS, path, &segment)) {
    return EXIT_FAILURE;
  }

  cpj_path_change_segment(CPJ_STYLE_WINDOWS, &segment, "other", buffer, sizeof(buffer));

  if (strcmp(buffer, "C:\\this\\cool\\other") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_change_first(void)
{
  const cpj_char_t *path;
  cpj_char_t buffer[FILENAME_MAX];
  struct cpj_segment segment;

  path = "C:\\this\\cool\\path\\";
  if (!cpj_path_get_first_segment(CPJ_STYLE_WINDOWS, path, &segment)) {
    return EXIT_FAILURE;
  }

  cpj_path_change_segment(CPJ_STYLE_WINDOWS, &segment, "other", buffer, sizeof(buffer));

  if (strcmp(buffer, "C:\\other\\cool\\path\\") != 0) {
    return EXIT_FAILURE;
  }

  path = "this\\cool\\path\\";
  if (!cpj_path_get_first_segment(CPJ_STYLE_WINDOWS, path, &segment)) {
    return EXIT_FAILURE;
  }

  cpj_path_change_segment(CPJ_STYLE_WINDOWS, &segment, "other", buffer, sizeof(buffer));

  if (strcmp(buffer, "other\\cool\\path\\") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_change_simple(void)
{
  const cpj_char_t *path;
  cpj_char_t buffer[FILENAME_MAX];
  struct cpj_segment segment;

  path = "C:\\this\\cool\\path\\";
  if (!cpj_path_get_first_segment(CPJ_STYLE_WINDOWS, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_next_segment(CPJ_STYLE_WINDOWS, &segment)) {
    return EXIT_FAILURE;
  }

  cpj_path_change_segment(CPJ_STYLE_WINDOWS, &segment, "other", buffer, sizeof(buffer));

  if (strcmp(buffer, "C:\\this\\other\\path\\") != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_back_with_root(void)
{
  const cpj_char_t *path;
  struct cpj_segment segment;

  path = "C:\\this\\path";
  if (!cpj_path_get_last_segment(CPJ_STYLE_WINDOWS, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "path", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_WINDOWS, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "this", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_previous_segment(CPJ_STYLE_WINDOWS, &segment)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_type(void)
{
  const cpj_char_t *path;
  struct cpj_segment segment;

  path = "/a/./../.folder/..folder";

  if (!cpj_path_get_first_segment(CPJ_STYLE_UNIX, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_segment_type(&segment) != CPJ_NORMAL) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_next_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_segment_type(&segment) != CPJ_CURRENT) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_next_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_segment_type(&segment) != CPJ_BACK) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_next_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_segment_type(&segment) != CPJ_NORMAL) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_next_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_segment_type(&segment) != CPJ_NORMAL) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_previous_too_far_root(void)
{
  const cpj_char_t *path;
  struct cpj_segment segment;

  path = "//now/hello_world/abc/";

  if (!cpj_path_get_last_segment(CPJ_STYLE_UNIX, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_previous_too_far(void)
{
  const cpj_char_t *path;
  struct cpj_segment segment;

  path = "//now/hello_world/abc/";

  if (!cpj_path_get_last_segment(CPJ_STYLE_UNIX, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_previous_relative(void)
{
  const cpj_char_t *path;
  struct cpj_segment segment;

  path = "now/hello_world/abc/";

  if (!cpj_path_get_last_segment(CPJ_STYLE_UNIX, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "abc", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "hello_world", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "now", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_previous_absolute(void)
{
  const cpj_char_t *path;
  struct cpj_segment segment;

  path = "/now/hello_world/abc/";

  if (!cpj_path_get_last_segment(CPJ_STYLE_UNIX, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "abc", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "hello_world", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "now", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_previous_relative_one_char_first(void)
{
  const cpj_char_t *path;
  struct cpj_segment segment;

  path = "n/hello_world/abc/";

  if (!cpj_path_get_last_segment(CPJ_STYLE_UNIX, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "abc", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "hello_world", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "n", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }
  
  path = "t\\cool\\path\\";

  if (!cpj_path_get_last_segment(CPJ_STYLE_WINDOWS, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "path", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_WINDOWS, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "cool", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_WINDOWS, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "t", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_previous_segment(CPJ_STYLE_WINDOWS, &segment)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_previous_absolute_one_char_first(void)
{
  const cpj_char_t *path;
  struct cpj_segment segment;

  path = "/n/hello_world/abc/";

  if (!cpj_path_get_last_segment(CPJ_STYLE_UNIX, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "abc", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "hello_world", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "n", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_previous_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }
  
  path = "C:\\t\\cool\\path\\";

  if (!cpj_path_get_last_segment(CPJ_STYLE_WINDOWS, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "path", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_WINDOWS, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "cool", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_previous_segment(CPJ_STYLE_WINDOWS, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "t", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_previous_segment(CPJ_STYLE_WINDOWS, &segment)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_next_too_far(void)
{
  const cpj_char_t *path;
  struct cpj_segment segment;

  path = "/hello_world/abc/";

  if (!cpj_path_get_first_segment(CPJ_STYLE_UNIX, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_next_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_next_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_next_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_next(void)
{
  const cpj_char_t *path;
  struct cpj_segment segment;

  path = "/hello_world/abc/";

  if (!cpj_path_get_first_segment(CPJ_STYLE_UNIX, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "hello_world", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (!cpj_path_get_next_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "abc", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  if (cpj_path_get_next_segment(CPJ_STYLE_UNIX, &segment)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_last(void)
{
  const cpj_char_t *path;
  struct cpj_segment segment;

  path = "/hello_world/abc";

  if (!cpj_path_get_last_segment(CPJ_STYLE_UNIX, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "abc", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  path = "hello_world/abc";

  if (!cpj_path_get_last_segment(CPJ_STYLE_UNIX, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "abc", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int segment_first(void)
{
  const cpj_char_t *path;
  struct cpj_segment segment;

  path = "/hello_world/abc";

  if (!cpj_path_get_first_segment(CPJ_STYLE_UNIX, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "hello_world", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  path = "hello_world/abc";

  if (!cpj_path_get_first_segment(CPJ_STYLE_UNIX, path, &segment)) {
    return EXIT_FAILURE;
  }

  if (strncmp(segment.begin, "hello_world", segment.size) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
