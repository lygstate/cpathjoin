#pragma once

#ifndef CPJ_LIBRARY_H
#define CPJ_LIBRARY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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

typedef char cpj_char_t;
typedef size_t cpj_size_t;
#define CPJ_SIZE_MAX SIZE_MAX

/**
 * Description of a string for arguments passing
 */
typedef struct
{
  /**< pointer to the zero-terminated ASCII/UTF-8/CESU-8 string */
  const cpj_char_t *ptr;
  cpj_size_t size; /**< size of the string, excluding '\0' terminator */
} cpj_string_t;

typedef struct
{
  cpj_size_t segment_count_base;
  cpj_size_t segment_count_other;
  cpj_size_t equal_segment;
} cpj_path_intersection_t;

/**
 * @brief Determines the style which is used for the path parsing and
 * generation.
 */
typedef enum
{
  CPJ_STYLE_WINDOWS,
  CPJ_STYLE_UNIX
} cpj_path_style_t;

/**
 * Helper to generate a string literal with type const cpj_char_t *
 */
#define CPJ_ZSTR_LITERAL(str) ((const cpj_char_t *)(str ""))

/**
 * Helper to expand string literal to [string pointer, string size] argument
 * pair.
 */
#define CPJ_ZSTR_ARG(str) CPJ_ZSTR_LITERAL(str), ((cpj_size_t)(sizeof(str) - 1))

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
 * @param cwd_directory The current directory for prefix the relative paths when
 * needed
 * @param base_directory The base path from which the relative path will
 * start.
 * @param path The target path where the relative path will point to.
 * @param buffer The buffer where the result will be written to.
 * @param buffer_size The size of the result buffer.
 * @return Returns the total amount of characters of the full path.
 */
CPJ_PUBLIC cpj_size_t cpj_path_get_relative(
  cpj_path_style_t path_style, const cpj_string_t *cwd_directory,
  const cpj_string_t *path_directory, const cpj_string_t *path,
  cpj_char_t *buffer, cpj_size_t buffer_size
);

/**
 * @brief Joins multiple paths together.
 *
 * This function generates a new path by joining multiple paths together. It
 * will remove double separators, and unlike cpj_path_get_absolute_test it
 * permits the use of multiple relative paths to combine. The last path of the
 * submitted string array must be set to NULL. The result will be written to a
 * buffer, which might be truncated if the buffer is not large enough to hold
 * the full path. However, the truncated result will always be
 * null-terminated. The returned value is the amount of characters which the
 * resulting path would take if it was not truncated (excluding the
 * null-terminating character).
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param is_resolve If true, then the given sequence of paths is processed from
 * right to left, with each subsequent path prepended until an absolute path is
 * constructed. For instance, given the sequence of path segments: /foo, /bar,
 *   baz, calling path.resolve('/foo', '/bar', 'baz') would return /bar/baz
 * because 'baz' is not an absolute path but
 *   '/bar' + '/' + 'baz' is.
 * Otherwise all paths are joined.
 * @param remove_trailing_slash If true then remove the trailing slash of a
 * directory path otherwise preserved.
 * @param path_list_p An array of paths which will be joined.
 * @param path_list_count The count of array of paths.
 * @param buffer_p The buffer where the result will be written to.
 * @param buffer_size The size of the result buffer.
 * @return Returns the size of the joined path, excluding the '\0' teminiator
 */
CPJ_PUBLIC
cpj_size_t cpj_path_join_multiple(
  cpj_path_style_t path_style, bool is_resolve, bool remove_trailing_slash,
  const cpj_string_t *path_list_p, cpj_size_t path_list_count,
  cpj_char_t *buffer_p, cpj_size_t buffer_size
);

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
 * @return The inspected root length of the path.
 */
CPJ_PUBLIC cpj_size_t
cpj_path_get_root(cpj_path_style_t path_style, const cpj_char_t *path);

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
CPJ_PUBLIC cpj_size_t cpj_path_change_root(
  cpj_path_style_t path_style, const cpj_string_t *path,
  const cpj_string_t *new_root, cpj_char_t *buffer, cpj_size_t buffer_size
);

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
CPJ_PUBLIC bool
cpj_path_is_absolute(cpj_path_style_t path_style, const cpj_char_t *path);

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
CPJ_PUBLIC bool
cpj_path_is_relative(cpj_path_style_t path_style, const cpj_char_t *path);

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
 * @return Returns true if the path a root
 */
CPJ_PUBLIC bool cpj_path_get_basename(
  cpj_path_style_t path_style, const cpj_string_t *path, cpj_string_t *basename
);

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
CPJ_PUBLIC cpj_size_t cpj_path_change_basename(
  cpj_path_style_t path_style, const cpj_string_t *path,
  const cpj_string_t *new_basename, cpj_char_t *buffer, cpj_size_t buffer_size
);

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
 * @return Returns the length of the dirname.
 */
CPJ_PUBLIC cpj_size_t
cpj_path_get_dirname(cpj_path_style_t path_style, const cpj_string_t *path);

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
 * @param extension The output of the extension.
 * @return Returns true if an extension is found or false otherwise.
 */
CPJ_PUBLIC bool cpj_path_get_extension(
  cpj_path_style_t path_style, const cpj_string_t *path, cpj_string_t *extension
);

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
cpj_size_t cpj_path_change_extension(
  cpj_path_style_t path_style, const cpj_string_t *path,
  const cpj_string_t *new_extension, cpj_char_t *buffer, cpj_size_t buffer_size
);

/**
 * @brief Finds common segments in two paths.
 *
 * This function finds common segments in two paths and returns the number
 * segments that are equal from the beginning of the base path which are equal
 * to the other path.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param path_base The base path which will be compared with the other path.
 * @param path_other The other path which will compared with the base path.
 * @return Returns the common segments information
 */
CPJ_PUBLIC
cpj_path_intersection_t cpj_path_get_intersection_segments(
  cpj_path_style_t path_style, const cpj_string_t *path_base,
  const cpj_string_t *path_other, cpj_size_t path_count
);

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
CPJ_PUBLIC cpj_size_t cpj_path_get_intersection(
  cpj_path_style_t path_style, const cpj_string_t *path_base,
  const cpj_string_t *path_other
);

/**
 * @brief Checks whether the submitted character is a separator.
 *
 * @param path_style Style depending on the operating system. So this should
 * detect whether we should use windows or unix paths.
 * @param ch A character
 * @return Returns true if it is a separator, or false otherwise.
 */
CPJ_PUBLIC bool
cpj_path_is_separator(cpj_path_style_t path_style, const cpj_char_t ch);

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
CPJ_PUBLIC cpj_path_style_t cpj_path_guess_style(const cpj_string_t *path);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
