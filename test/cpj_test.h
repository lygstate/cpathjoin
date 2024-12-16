#pragma once

#include <cpj.h>

#include <string.h>

static inline cpj_size_t cpj_strlen(const cpj_char_t *str)
{
  return (cpj_size_t)strlen((const char *)str);
}

static inline cpj_string_t
cpj_string_create(const cpj_char_t *ptr, cpj_size_t size)
{
  cpj_string_t str = {ptr, size};
  return str;
}

static inline cpj_size_t cpj_path_normalize_test(
  cpj_path_style_t path_style, const cpj_char_t *path, cpj_char_t *buffer,
  cpj_size_t buffer_size
)
{
  cpj_string_t paths[1];

  // Now we initialize the paths which we will normalize. Since this function
  // only supports submitting a single path, we will only add that one.
  paths[0] = cpj_string_create(path, cpj_strlen(path));

  return cpj_path_join_multiple(
    path_style, false, true, paths, 1, buffer, buffer_size
  );
}

static inline cpj_size_t cpj_path_change_extension_test(
  cpj_path_style_t path_style, const cpj_char_t *path,
  const cpj_char_t *new_extension, cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  cpj_string_t path_str = cpj_string_create(path, cpj_strlen(path));
  cpj_string_t new_extension_str =
    cpj_string_create(new_extension, cpj_strlen(new_extension));
  return cpj_path_change_extension(
    path_style, &path_str, &new_extension_str, buffer, buffer_size
  );
}

static inline cpj_size_t cpj_path_get_intersection_test(
  cpj_path_style_t path_style, const cpj_char_t *path_base,
  const cpj_char_t *path_other
)
{
  cpj_string_t
    path_base_str = cpj_string_create(path_base, cpj_strlen(path_base));
  cpj_string_t
    path_other_str = cpj_string_create(path_other, cpj_strlen(path_other));
  return cpj_path_get_intersection(path_style, &path_base_str, &path_other_str);
}

static inline bool cpj_path_get_extension_test(
  cpj_path_style_t path_style, const cpj_char_t *path,
  const cpj_char_t **extension, cpj_size_t *length
)
{
  cpj_string_t path_str = cpj_string_create(path, cpj_strlen(path));
  cpj_string_t extension_str;
  bool has_extension =
    cpj_path_get_extension(path_style, &path_str, &extension_str);
  if (extension) {
    *extension = extension_str.ptr;
  }
  if (length) {
    *length = extension_str.size;
  }
  return has_extension;
}

static inline bool
cpj_path_has_extension_test(cpj_path_style_t path_style, const cpj_char_t *path)
{
  cpj_string_t path_str = cpj_string_create(path, cpj_strlen(path));
  return cpj_path_get_extension(path_style, &path_str, NULL);
}

static inline void cpj_path_get_dirname_test(
  cpj_path_style_t path_style, const cpj_char_t *path, cpj_size_t *length
)
{
  cpj_string_t path_str = cpj_string_create(path, cpj_strlen(path));
  *length = cpj_path_get_dirname(path_style, &path_str);
}

/**
 * @brief Joins two paths together.
 *
 * This function generates a new path by combining the two submitted paths. It
 * will remove double separators, and unlike cpj_path_get_absolute_test it
 * permits the use of two relative paths to combine. The result will be written
 * to a buffer, which might be truncated if the buffer is not large enough to
 * hold the full path. However, the truncated result will always be
 * null-terminated. The returned value is the amount of characters which the
 * resulting path would take if it was not truncated (excluding the
 * null-terminating character).
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path_a The first path which comes first.
 * @param path_b The second path which comes after the first.
 * @param buffer The buffer where the result will be written to.
 * @param buffer_size The size of the result buffer.
 * @return Returns the total amount of characters of the full, combined path.
 */
static inline cpj_size_t cpj_path_join_test(
  cpj_path_style_t path_style, const cpj_char_t *path_a,
  const cpj_char_t *path_b, cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  cpj_string_t paths[2];

  // This is simple. We will just create an array with the two paths which we
  // wish to join.
  paths[0] = cpj_string_create(path_a, cpj_strlen(path_a));
  paths[1] = cpj_string_create(path_b, cpj_strlen(path_b));

  // And then call the join and normalize function which will do the hard work
  // for us.
  return cpj_path_join_multiple(
    path_style, false, true, paths, 2, buffer, buffer_size
  );
}

/**
 * @brief Joins two paths together.
 *
 * This function generates a new path by combining the two submitted paths. It
 * will remove double separators, and unlike cpj_path_get_absolute_test it
 * permits the use of two relative paths to combine. The result will be written
 * to a buffer, which might be truncated if the buffer is not large enough to
 * hold the full path. However, the truncated result will always be
 * null-terminated. The returned value is the amount of characters which the
 * resulting path would take if it was not truncated (excluding the
 * null-terminating character).
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path_a The first path which comes first.
 * @param path_b The second path which comes after the first.
 * @param buffer The buffer where the result will be written to.
 * @param buffer_size The size of the result buffer.
 * @return Returns the total amount of characters of the full, combined path.
 */
static inline cpj_size_t cpj_path_join_module(
  cpj_path_style_t path_style, const cpj_char_t *path_a,
  const cpj_char_t *path_b, cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  cpj_string_t path_list[2] = {
    {(const cpj_char_t *)path_a, (cpj_size_t)cpj_strlen(path_a)},
    {(const cpj_char_t *)path_b, (cpj_size_t)cpj_strlen(path_b)},
  };
  return cpj_path_join_multiple(
    path_style, true, true, path_list, sizeof(path_list) / sizeof(path_list[0]),
    buffer, buffer_size
  );
}

/**
 * @brief Generates an absolute path based on a base.
 *
 * This function generates an absolute path based on a base path and another
 * path. It is guaranteed to return an absolute path. If the second submitted
 * path is absolute, it will override the base path. The result will be
 * written to a buffer, which might be truncated if the buffer is not large
 * enough to hold the full path. However, the truncated result will always be
 * null-terminated. The returned value is the amount of characters which the
 * resulting path would take if it was not truncated (excluding the
 * null-terminating character).
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param base The absolute base path on which the relative path will be
 * applied.
 * @param path The relative path which will be applied on the base path.
 * @param buffer The buffer where the result will be written to.
 * @param buffer_size The size of the result buffer.
 * @return Returns the total amount of characters of the new absolute path.
 */
static inline cpj_size_t cpj_path_get_absolute_test(
  cpj_path_style_t path_style, const cpj_char_t *base, const cpj_char_t *path,
  cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  cpj_string_t paths[3];

  // The basename should be an absolute path if the caller is using the API
  // correctly. However, he might not and in that case we will append a fake
  // root at the beginning.
  paths[0] = cpj_string_create(CPJ_ZSTR_ARG("/"));
  paths[1] = cpj_string_create(base, cpj_strlen(base));
  paths[2] = cpj_string_create(path, cpj_strlen(path));

  // Finally join everything together and normalize it.
  return cpj_path_join_multiple(
    path_style, true, true, paths, 3, buffer, buffer_size
  );
}

static inline cpj_size_t cpj_path_change_basename_test(
  cpj_path_style_t path_style, const cpj_char_t *path,
  const cpj_char_t *new_basename, cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  cpj_string_t path_str = cpj_string_create(path, cpj_strlen(path));
  cpj_string_t new_basename_str =
    cpj_string_create(new_basename, cpj_strlen(new_basename));
  return cpj_path_change_basename(
    path_style, &path_str, &new_basename_str, buffer, buffer_size
  );
}

static inline cpj_path_style_t cpj_path_guess_style_test(const cpj_char_t *path)
{
  cpj_string_t path_str = cpj_string_create(path, cpj_strlen(path));
  return cpj_path_guess_style(&path_str);
}

static inline void cpj_path_get_basename_test(
  cpj_path_style_t path_style, const cpj_char_t *path,
  const cpj_char_t **basename, cpj_size_t *length
)
{
  cpj_string_t path_str = cpj_string_create(path, cpj_strlen(path));
  cpj_string_t basename_str;
  cpj_path_get_basename(path_style, &path_str, &basename_str);
  *basename = basename_str.ptr;
  if (length) {
    *length = basename_str.size;
  }
}

static inline cpj_size_t cpj_path_change_root_test(
  cpj_path_style_t path_style, const cpj_char_t *path,
  const cpj_char_t *new_root, cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  cpj_string_t path_str = cpj_string_create(path, cpj_strlen(path));
  cpj_string_t new_root_str = cpj_string_create(new_root, cpj_strlen(new_root));
  return cpj_path_change_root(
    path_style, &path_str, &new_root_str, buffer, buffer_size
  );
}

static inline cpj_size_t cpj_path_get_relative_test(
  cpj_path_style_t path_style, const cpj_char_t *base_directory,
  const cpj_char_t *path, cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  cpj_string_t base_directory_str =
    cpj_string_create(base_directory, cpj_strlen(base_directory));
  cpj_string_t path_str = cpj_string_create(path, cpj_strlen(path));
  cpj_string_t path_cwd = {CPJ_ZSTR_ARG("/")};
  return cpj_path_get_relative(
    path_style, &path_cwd, &base_directory_str, &path_str, buffer, buffer_size
  );
}
