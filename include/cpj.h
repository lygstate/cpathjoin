#pragma once

#ifndef CPJ_LIBRARY_H
#define CPJ_LIBRARY_H

#include <stdbool.h>
#include <stddef.h>

#if defined(_WIN32) || defined(__CYGWIN__)
#define CPJ_EXPORT __declspec(dllexport)
#define CPJ_IMPORT __declspec(dllimport)
#elif __GNUC__ >= 4
#define CPJ_EXPORT __attribute__((visibility("default")))
#define CPJ_IMPORT __attribute__((visibility("default")))
#else
#define CPJ_EXPORT
#define CPJ_IMPORT
#endif

#if defined(CPJ_SHARED)
#if defined(CPJ_EXPORTS)
#define CPJ_PUBLIC CPJ_EXPORT
#else
#define CPJ_PUBLIC CPJ_IMPORT
#endif
#else
#define CPJ_PUBLIC
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * A segment represents a single component of a path. For instance, on linux a
 * path might look like this "/var/log/", which consists of two segments "var"
 * and "log".
 */
struct cpj_segment
{
  const char *path;
  const char *segments;
  const char *begin;
  const char *end;
  size_t size;
};

/**
 * The segment type can be used to identify whether a segment is a special
 * segment or not.
 *
 * CPJ_NORMAL - normal folder or file segment
 * CPJ_CURRENT - "./" current folder segment
 * CPJ_BACK - "../" relative back navigation segment
 */
enum cpj_segment_type
{
  CPJ_NORMAL,
  CPJ_CURRENT,
  CPJ_BACK
};

/**
 * @brief Determines the style which is used for the path parsing and
 * generation.
 */
enum cpj_path_style
{
  CPJ_STYLE_WINDOWS,
  CPJ_STYLE_UNIX
};

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
CPJ_PUBLIC size_t cpj_path_get_absolute(enum cpj_path_style path_style,
  const char *base, const char *path, char *buffer, size_t buffer_size);

/**
 * @brief Generates a relative path based on a base.
 *
 * This function generates a relative path based on a base path and another
 * path. It determines how to get to the submitted path, starting from the
 * base directory. The result will be written to a buffer, which might be
 * truncated if the buffer is not large enough to hold the full path. However,
 * the truncated result will always be null-terminated. The returned value is
 * the amount of characters which the resulting path would take if it was not
 * truncated (excluding the null-terminating character).
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param base_directory The base path from which the relative path will
 * start.
 * @param path The target path where the relative path will point to.
 * @param buffer The buffer where the result will be written to.
 * @param buffer_size The size of the result buffer.
 * @return Returns the total amount of characters of the full path.
 */
CPJ_PUBLIC size_t cpj_path_get_relative(enum cpj_path_style path_style,
  const char *base_directory, const char *path, char *buffer,
  size_t buffer_size);

/**
 * @brief Joins two paths together.
 *
 * This function generates a new path by combining the two submitted paths. It
 * will remove double separators, and unlike cpj_path_get_absolute it permits
 * the use of two relative paths to combine. The result will be written to a
 * buffer, which might be truncated if the buffer is not large enough to hold
 * the full path. However, the truncated result will always be
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
CPJ_PUBLIC size_t cpj_path_join(enum cpj_path_style path_style,
  const char *path_a, const char *path_b, char *buffer, size_t buffer_size);

/**
 * @brief Joins multiple paths together.
 *
 * This function generates a new path by joining multiple paths together. It
 * will remove double separators, and unlike cpj_path_get_absolute it permits
 * the use of multiple relative paths to combine. The last path of the
 * submitted string array must be set to NULL. The result will be written to a
 * buffer, which might be truncated if the buffer is not large enough to hold
 * the full path. However, the truncated result will always be
 * null-terminated. The returned value is the amount of characters which the
 * resulting path would take if it was not truncated (excluding the
 * null-terminating character).
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param paths An array of paths which will be joined.
 * @param buffer The buffer where the result will be written to.
 * @param buffer_size The size of the result buffer.
 * @return Returns the total amount of characters of the full, combined path.
 */
CPJ_PUBLIC size_t cpj_path_join_multiple(enum cpj_path_style path_style,
  const char **paths, char *buffer, size_t buffer_size);

/**
 * @brief Determines the root of a path.
 *
 * This function determines the root of a path by finding its length. The
 * root always starts at the submitted path. If the path has no root, the
 * length will be set to zero.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path The path which will be inspected.
 * @param length The output of the root length.
 */
CPJ_PUBLIC void cpj_path_get_root(enum cpj_path_style path_style,
  const char *path, size_t *length);

/**
 * @brief Changes the root of a path.
 *
 * This function changes the root of a path. It does not normalize the result.
 * The result will be written to a buffer, which might be truncated if the
 * buffer is not large enough to hold the full path. However, the truncated
 * result will always be null-terminated. The returned value is the amount of
 * characters which the resulting path would take if it was not truncated
 * (excluding the null-terminating character).
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path The original path which will get a new root.
 * @param new_root The new root which will be placed in the path.
 * @param buffer The output buffer where the result is written to.
 * @param buffer_size The size of the output buffer where the result is
 * written to.
 * @return Returns the total amount of characters of the new path.
 */
CPJ_PUBLIC size_t cpj_path_change_root(enum cpj_path_style path_style,
  const char *path, const char *new_root, char *buffer, size_t buffer_size);

/**
 * @brief Determine whether the path is absolute or not.
 *
 * This function checks whether the path is an absolute path or not. A path is
 * considered to be absolute if the root ends with a separator.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path The path which will be checked.
 * @return Returns true if the path is absolute or false otherwise.
 */
CPJ_PUBLIC bool cpj_path_is_absolute(enum cpj_path_style path_style,
  const char *path);

/**
 * @brief Determine whether the path is relative or not.
 *
 * This function checks whether the path is a relative path or not. A path is
 * considered to be relative if the root does not end with a separator.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path The path which will be checked.
 * @return Returns true if the path is relative or false otherwise.
 */
CPJ_PUBLIC bool cpj_path_is_relative(enum cpj_path_style path_style,
  const char *path);

/**
 * @brief Gets the basename of a file path.
 *
 * This function gets the basename of a file path. A pointer to the beginning
 * of the basename will be returned through the basename parameter. This
 * pointer will be positioned on the first letter after the separator. The
 * length of the file path will be returned through the length parameter. The
 * length will be set to zero and the basename to NULL if there is no basename
 * available.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path The path which will be inspected.
 * @param basename The output of the basename pointer.
 * @param length The output of the length of the basename. This may be
 * null if not required.
 */
CPJ_PUBLIC void cpj_path_get_basename(enum cpj_path_style path_style,
  const char *path, const char **basename, size_t *length);

/**
 * @brief Changes the basename of a file path.
 *
 * This function changes the basename of a file path. This function will not
 * write out more than the specified buffer can contain. However, the
 * generated string is always null-terminated - even if not the whole path is
 * written out. The function returns the total number of characters the
 * complete buffer would have, even if it was not written out completely. The
 * path may be the same memory address as the buffer.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path The original path which will be used for the modified path.
 * @param new_basename The new basename which will replace the old one.
 * @param buffer The buffer where the changed path will be written to.
 * @param buffer_size The size of the result buffer where the changed path is
 * written to.
 * @return Returns the size which the complete new path would have if it was
 * not truncated.
 */
CPJ_PUBLIC size_t cpj_path_change_basename(enum cpj_path_style path_style,
  const char *path, const char *new_basename, char *buffer, size_t buffer_size);

/**
 * @brief Gets the dirname of a file path.
 *
 * This function determines the dirname of a file path and returns the length
 * up to which character is considered to be part of it. If no dirname is
 * found, the length will be set to zero. The beginning of the dirname is
 * always equal to the submitted path pointer.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path The path which will be inspected.
 * @param length The length of the dirname.
 */
CPJ_PUBLIC void cpj_path_get_dirname(enum cpj_path_style path_style,
  const char *path, size_t *length);

/**
 * @brief Gets the extension of a file path.
 *
 * This function extracts the extension portion of a file path. A pointer to
 * the beginning of the extension will be returned through the extension
 * parameter if an extension is found and true is returned. This pointer will
 * be positioned on the dot. The length of the extension name will be returned
 * through the length parameter. If no extension is found both parameters
 * won't be touched and false will be returned.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path The path which will be inspected.
 * @param extension The output of the extension pointer.
 * @param length The output of the length of the extension.
 * @return Returns true if an extension is found or false otherwise.
 */
CPJ_PUBLIC bool cpj_path_get_extension(enum cpj_path_style path_style,
  const char *path, const char **extension, size_t *length);

/**
 * @brief Determines whether the file path has an extension.
 *
 * This function determines whether the submitted file path has an extension.
 * This will evaluate to true if the last segment of the path contains a dot.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path The path which will be inspected.
 * @return Returns true if the path has an extension or false otherwise.
 */
CPJ_PUBLIC bool cpj_path_has_extension(enum cpj_path_style path_style,
  const char *path);

/**
 * @brief Changes the extension of a file path.
 *
 * This function changes the extension of a file name. The function will
 * append an extension if the basename does not have an extension, or use the
 * extension as a basename if the path does not have a basename. This function
 * will not write out more than the specified buffer can contain. However, the
 * generated string is always null-terminated - even if not the whole path is
 * written out. The function returns the total number of characters the
 * complete buffer would have, even if it was not written out completely. The
 * path may be the same memory address as the buffer.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path The path which will be used to make the change.
 * @param new_extension The extension which will be placed within the new
 * path.
 * @param buffer The output buffer where the result will be written to.
 * @param buffer_size The size of the output buffer where the result will be
 * written to.
 * @return Returns the total size which the output would have if it was not
 * truncated.
 */
CPJ_PUBLIC size_t cpj_path_change_extension(enum cpj_path_style path_style,
  const char *path, const char *new_extension, char *buffer,
  size_t buffer_size);

/**
 * @brief Creates a normalized version of the path.
 *
 * This function creates a normalized version of the path within the specified
 * buffer. This function will not write out more than the specified buffer can
 * contain. However, the generated string is always null-terminated - even if
 * not the whole path is written out. The returned value is the amount of
 * characters which the resulting path would take if it was not truncated
 * (excluding the null-terminating character). The path may be the same memory
 * address as the buffer.
 *
 * The following will be true for the normalized path:
 * 1) "../" will be resolved.
 * 2) "./" will be removed.
 * 3) double separators will be fixed with a single separator.
 * 4) separator suffixes will be removed.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path The path which will be normalized.
 * @param buffer The buffer where the new path is written to.
 * @param buffer_size The size of the buffer.
 * @return The size which the complete normalized path has if it was not
 * truncated.
 */
CPJ_PUBLIC size_t cpj_path_normalize(enum cpj_path_style path_style,
  const char *path, char *buffer, size_t buffer_size);

/**
 * @brief Finds common portions in two paths.
 *
 * This function finds common portions in two paths and returns the number
 * characters from the beginning of the base path which are equal to the other
 * path.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path_base The base path which will be compared with the other path.
 * @param path_other The other path which will compared with the base path.
 * @return Returns the number of characters which are common in the base path.
 */
CPJ_PUBLIC size_t cpj_path_get_intersection(enum cpj_path_style path_style,
  const char *path_base, const char *path_other);

/**
 * @brief Gets the first segment of a path.
 *
 * This function finds the first segment of a path. The position of the
 * segment is set to the first character after the separator, and the length
 * counts all characters until the next separator (excluding the separator).
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path The path which will be inspected.
 * @param segment The segment which will be extracted.
 * @return Returns true if there is a segment or false if there is none.
 */
CPJ_PUBLIC bool cpj_path_get_first_segment(enum cpj_path_style path_style,
  const char *path, struct cpj_segment *segment);

/**
 * @brief Gets the last segment of the path.
 *
 * This function gets the last segment of a path. This function may return
 * false if the path doesn't contain any segments, in which case the submitted
 * segment parameter is not modified. The position of the segment is set to
 * the first character after the separator, and the length counts all
 * characters until the end of the path (excluding the separator).
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path The path which will be inspected.
 * @param segment The segment which will be extracted.
 * @return Returns true if there is a segment or false if there is none.
 */
CPJ_PUBLIC bool cpj_path_get_last_segment(enum cpj_path_style path_style,
  const char *path, struct cpj_segment *segment);

/**
 * @brief Advances to the next segment.
 *
 * This function advances the current segment to the next segment. If there
 * are no more segments left, the submitted segment structure will stay
 * unchanged and false is returned.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param segment The current segment which will be advanced to the next one.
 * @return Returns true if another segment was found or false otherwise.
 */
CPJ_PUBLIC bool cpj_path_get_next_segment(enum cpj_path_style path_style,
  struct cpj_segment *segment);

/**
 * @brief Moves to the previous segment.
 *
 * This function moves the current segment to the previous segment. If the
 * current segment is the first one, the submitted segment structure will stay
 * unchanged and false is returned.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param segment The current segment which will be moved to the previous one.
 * @return Returns true if there is a segment before this one or false
 * otherwise.
 */
CPJ_PUBLIC bool cpj_path_get_previous_segment(enum cpj_path_style path_style,
  struct cpj_segment *segment);

/**
 * @brief Gets the type of the submitted path segment.
 *
 * This function inspects the contents of the segment and determines the type
 * of it. Currently, there are three types CPJ_NORMAL, CPJ_CURRENT and
 * CPJ_BACK. A CPJ_NORMAL segment is a normal folder or file entry. A
 * CPJ_CURRENT is a "./" and a CPJ_BACK a "../" segment.
 *
 * @param segment The segment which will be inspected.
 * @return Returns the type of the segment.
 */
CPJ_PUBLIC enum cpj_segment_type cpj_path_get_segment_type(
  const struct cpj_segment *segment);

/**
 * @brief Changes the content of a segment.
 *
 * This function overrides the content of a segment to the submitted value and
 * outputs the whole new path to the submitted buffer. The result might
 * require less or more space than before if the new value length differs from
 * the original length. The output is truncated if the new path is larger than
 * the submitted buffer size, but it is always null-terminated. The source of
 * the segment and the submitted buffer may be the same.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param segment The segment which will be modifier.
 * @param value The new content of the segment.
 * @param buffer The buffer where the modified path will be written to.
 * @param buffer_size The size of the output buffer.
 * @return Returns the total size which would have been written if the output
 * was not truncated.
 */
CPJ_PUBLIC size_t cpj_path_change_segment(enum cpj_path_style path_style,
  struct cpj_segment *segment, const char *value, char *buffer,
  size_t buffer_size);

/**
 * @brief Checks whether the submitted pointer points to a separator.
 *
 * This function simply checks whether the submitted pointer points to a
 * separator, which has to be null-terminated (but not necessarily after the
 * separator). The function will return true if it is a separator, or false
 * otherwise.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param str A pointer to a string.
 * @return Returns true if it is a separator, or false otherwise.
 */
CPJ_PUBLIC bool cpj_path_is_separator(enum cpj_path_style path_style,
  const char *str);

/**
 * @brief Guesses the path style.
 *
 * This function guesses the path style based on a submitted path-string. The
 * guessing will look at the root and the type of slashes contained in the
 * path and return the style which is more likely used in the path.
 *
 * @param path The path which will be inspected.
 * @return Returns the style which is most likely used for the path.
 */
CPJ_PUBLIC enum cpj_path_style cpj_path_guess_style(const char *path);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
