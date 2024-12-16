#include <assert.h>
#include <cpj.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

typedef struct
{
  const cpj_string_t *path_list_p;
  cpj_size_t path_list_count;
  cpj_size_t root_length;
  bool root_is_absolute;

  bool end_with_separator;
  cpj_size_t list_pos;
  cpj_size_t pos;
  cpj_size_t length;
  cpj_size_t segment_eat_count;
  cpj_size_t segment_count;
} cpj_segment_iterator_t;

bool cpj_path_is_separator(cpj_path_style_t style, const cpj_char_t ch)
{
  if (style == CPJ_STYLE_WINDOWS) {
    return ch == '/' || ch == '\\';
  } else {
    return ch == '/';
  }
} /* cpj_path_is_separator */

static cpj_size_t cpj_path_get_root_windows(const cpj_char_t *path)
{
  const cpj_char_t *c;
  cpj_size_t length = 0;
  // We can not determine the root if this is an empty string. So we set the
  // root to NULL and the length to zero and cancel the whole thing.
  c = path;
  if (!*c) {
    return length;
  }

  // Now we have to verify whether this is a windows network path (UNC), which
  // we will consider our root.
  if (cpj_path_is_separator(CPJ_STYLE_WINDOWS, *c)) {
    bool is_device_path;
    ++c;

    // Check whether the path starts with a single backslash, which means this
    // is not a network path - just a normal path starting with a backslash.
    if (!cpj_path_is_separator(CPJ_STYLE_WINDOWS, *c)) {
      // Okay, this is not a network path but we still use the backslash as a
      // root.
      ++length;
      return length;
    }

    // A device path is a path which starts with "\\." or "\\?". A device path
    // can be a UNC path as well, in which case it will take up one more
    // segment. So, this is a network or device path. Skip the previous
    // separator. Now we need to determine whether this is a device path. We
    // might advance one character here if the server name starts with a '?' or
    // a '.', but that's fine since we will search for a separator afterwards
    // anyway.
    ++c;
    is_device_path = (*c == '?' || *c == '.') &&
                     cpj_path_is_separator(CPJ_STYLE_WINDOWS, *(++c));
    if (is_device_path) {
      // That's a device path, and the root must be either "\\.\" or "\\?\"
      // which is 4 characters long. (at least that's how Windows
      // GetFullPathName behaves.)
      length = 4;
      return length;
    }

    // We will grab anything up to the next stop. The next stop might be a '\0'
    // or another separator. That will be the server name.
    while (*c != '\0' && !cpj_path_is_separator(CPJ_STYLE_WINDOWS, *c)) {
      ++c;
    }

    // If this is a separator and not the end of a string we wil have to include
    // it. However, if this is a '\0' we must not skip it.
    while (cpj_path_is_separator(CPJ_STYLE_WINDOWS, *c)) {
      ++c;
    }

    // We are now skipping the shared folder name, which will end after the
    // next stop.
    while (*c != '\0' && !cpj_path_is_separator(CPJ_STYLE_WINDOWS, *c)) {
      ++c;
    }
    // Then there might be a separator at the end. We will include that as well,
    // it will mark the path as absolute.
    if (cpj_path_is_separator(CPJ_STYLE_WINDOWS, *c)) {
      ++c;
    }

    // Finally, calculate the size of the root.
    length = (cpj_size_t)(c - path);
    return length;
  }

  // Move to the next and check whether this is a colon.
  if (*++c == ':') {
    length = 2;

    // Now check whether this is a backslash (or slash). If it is not, we could
    // assume that the next character is a '\0' if it is a valid path. However,
    // we will not assume that - since ':' is not valid in a path it must be a
    // mistake by the caller than. We will try to understand it anyway.
    if (cpj_path_is_separator(CPJ_STYLE_WINDOWS, *(++c))) {
      length = 3;
    }
  }
  return length;
} /* cpj_path_get_root_windows */

static cpj_size_t cpj_path_get_root_unix(const cpj_char_t *path)
{
  // The slash of the unix path represents the root. There is no root if there
  // is no slash.
  return cpj_path_is_separator(CPJ_STYLE_UNIX, *path) ? 1 : 0;
} /* cpj_path_get_root_unix */

cpj_size_t
cpj_path_get_root(cpj_path_style_t path_style, const cpj_char_t *path)
{
  if (!path) {
    return 0;
  }
  // We use a different implementation here based on the configuration of the
  // library.
  return path_style == CPJ_STYLE_WINDOWS ? cpj_path_get_root_windows(path)
                                         : cpj_path_get_root_unix(path);
} /* cpj_path_get_root */

static bool cpj_path_iterator_before_root(cpj_segment_iterator_t *it)
{
  return it->list_pos == 0 && ((it->pos + 1) <= it->root_length);
} /* cpj_path_iterator_before_root */

static cpj_char_t cpj_path_char_get(cpj_segment_iterator_t *it)
{
  if (cpj_path_iterator_before_root(it)) {
    return 0;
  }
  if (it->pos == CPJ_SIZE_MAX) {
    /**
     * The tail '\0' treat as '/' that is a path separator for both
     * POSIX/WINDOWS
     */
    return '/';
  }
  return it->path_list_p[it->list_pos].ptr[it->pos];
} /* cpj_path_char_get */

static void cpj_path_char_iter_prev(cpj_segment_iterator_t *it)
{
  if (cpj_path_iterator_before_root(it)) {
    return;
  }
  if (it->pos == CPJ_SIZE_MAX) {
    it->list_pos -= 1;
    it->pos = it->path_list_p[it->list_pos].size - 1;
  } else {
    --it->pos;
  }
} /* cpj_path_char_iter_prev */

static void cpj_path_get_prev_segment_detail(
  cpj_path_style_t path_style, cpj_segment_iterator_t *it
)
{
  if (it->segment_eat_count > 0) {
    --it->segment_eat_count;
    if (it->segment_eat_count > 0) {
      return;
    }
  }
  if (it->list_pos == 0 && it->pos == CPJ_SIZE_MAX &&
      it->length == it->root_length) {
    /* To the head */
    it->length = 0;
    return;
  }
  for (;;) {
    cpj_char_t ch;
    cpj_size_t segment_length = 0;
    for (;;) {
      ch = cpj_path_char_get(it);
      if (cpj_path_is_separator(path_style, ch)) {
        cpj_path_char_iter_prev(it);
        continue;
      }
      if (ch != 0) {
        segment_length += 1;
      }
      break;
    }
    for (;;) {
      cpj_path_char_iter_prev(it);
      ch = cpj_path_char_get(it);
      if (ch == 0 || cpj_path_is_separator(path_style, ch)) {
        break;
      }
      segment_length += 1;
    }
    const cpj_string_t *path_current = it->path_list_p + it->list_pos;
    if (segment_length == 1) {
      if (path_current->ptr[it->pos + 1] == '.') {
        continue;
      }
    } else if (segment_length == 2) {
      if (path_current->ptr[it->pos + 1] == '.' ||
          path_current->ptr[it->pos + 2] == '.') {
        it->segment_eat_count += 1;
        continue;
      }
    }
    if (it->segment_eat_count > 0 && segment_length > 0) {
      it->segment_eat_count -= 1;
      continue;
    }
    if (segment_length > 0) {
      /* segment_eat_count must be zero, this is a normal segment */
      it->length = segment_length;
      return;
    }
    it->length = 0;
    if (ch == 0 && it->root_is_absolute) {
      /* Dropping segment eat when the root segment is absolute */
      it->segment_eat_count = 0;
    }
    if (it->segment_eat_count > 0) {
      /* Genearting .. segment for relative path */
      return;
    }

    if (ch == 0) {
      if (it->segment_count == 0 && !it->root_is_absolute) {
        /* Path like `C:` `C:abc\..` `` `abc\..`  `.` should place a . as the
         * path component */
        return;
      }

      /* Return the root segment or head depends on `root_length` */
      it->pos = CPJ_SIZE_MAX;
      it->length = it->root_length;
    }
    return;
  } /* for (;;) */
} /* cpj_path_get_prev_segment_detail */

static bool cpj_path_get_prev_segment(
  cpj_path_style_t path_style, cpj_segment_iterator_t *it
)
{
  cpj_path_get_prev_segment_detail(path_style, it);
  if (it->segment_eat_count == 0 && it->list_pos == 0 &&
      it->pos == CPJ_SIZE_MAX && it->length == 0 && it->segment_count > 0) {
    /* no more segments */
    return false;
  }
  if (it->list_pos == 0 && it->pos == CPJ_SIZE_MAX && it->root_length > 0) {
    /* Root have no trailing separator */
    it->end_with_separator = false;
  } else if (it->segment_count > 0) {
    it->end_with_separator = true;
  } else {
    /* The last segment preserve the original separator. */
  }

  it->segment_count += 1;
  return true;
} /* cpj_path_get_prev_segment */

static cpj_string_t cpj_path_get_segment(cpj_segment_iterator_t *it)
{
  cpj_string_t segment;
  const cpj_string_t *path_current = it->path_list_p + it->list_pos;
  cpj_size_t pos = it->pos + 1;
  if (it->length > 0) {
    segment.size = it->length;
    segment.ptr = path_current->ptr + pos;
    return segment;
  }
  if (it->segment_eat_count > 0) {
    segment.size = 2;
  } else {
    segment.size = 1;
  }
  if ((it->list_pos + 1 == it->path_list_count) &&
      pos + segment.size == path_current->size) {
    /* The latest segment with '.' or '..' */
    segment.ptr = path_current->ptr + pos;
  } else if (segment.size == 2) {
    segment.ptr = CPJ_ZSTR_LITERAL("..");
  } else {
    segment.ptr = CPJ_ZSTR_LITERAL(".");
  }
  return segment;
} /* cpj_path_get_segment */

static void cpj_path_push_front_char(
  cpj_path_style_t path_style, cpj_char_t *buffer_p, cpj_size_t buffer_size,
  cpj_size_t *buffer_index, cpj_char_t ch
)
{
  *buffer_index -= 1;
  cpj_size_t buffer_index_current = *buffer_index;
  if (buffer_index_current < buffer_size) {
    if (buffer_index_current == (buffer_size - 1)) {
      buffer_p[buffer_index_current] = '\0';
    } else if (cpj_path_is_separator(path_style, ch)) {
      buffer_p[buffer_index_current] = path_style == CPJ_STYLE_UNIX ? '/'
                                                                    : '\\';
    } else {
      buffer_p[buffer_index_current] = ch;
    }
  }
} /* cpj_path_push_front_char */

static void cpj_path_push_front_string(
  cpj_path_style_t path_style, cpj_char_t *buffer_p, cpj_size_t buffer_size,
  cpj_size_t *buffer_index, const cpj_string_t *str
)
{
  cpj_size_t k = str->size;
  for (; k > 0;) {
    cpj_path_push_front_char(
      path_style, buffer_p, buffer_size, buffer_index, str->ptr[--k]
    );
  }
} /* cpj_path_push_front_string */

static const cpj_string_t path_list_empty = {CPJ_ZSTR_ARG("")};

/**
 * Init the path segment interator
 */
static cpj_segment_iterator_t cpj_path_interator_init(
  cpj_path_style_t path_style,     /**< The style of the path list */
  bool is_resolve,                 /**< If do path resolve */
  bool remove_trailing_slash,      /**< If remove the trailing slash symbol */
  const cpj_string_t *path_list_p, /**< Path list */
  cpj_size_t path_list_count
)
{
  cpj_segment_iterator_t it = {0};
  cpj_size_t path_list_i;
  cpj_size_t end_with_separator = CPJ_SIZE_MAX;
  if (path_list_count == 0) {
    path_list_count = 1;
    path_list_p = &path_list_empty;
  }
  path_list_i = path_list_count;
  it.path_list_p = path_list_p;
  it.path_list_count = path_list_count;
  for (; path_list_i > 0;) {
    const cpj_string_t *path_list_current = path_list_p + (--path_list_i);
    if (end_with_separator == CPJ_SIZE_MAX && path_list_current->size > 0) {
      end_with_separator = cpj_path_is_separator(
                             path_style,
                             path_list_current->ptr[path_list_current->size - 1]
                           )
                             ? 1
                             : 0;
    }
    if (it.root_length == 0 && (is_resolve || path_list_i == 0)) {
      /* Find the first root path from right to left when `is_resolve` are
       * `true` */
      it.root_length = cpj_path_get_root(path_style, path_list_current->ptr);
      if (it.root_length > 0) {
        it.path_list_p += path_list_i;
        it.path_list_count -= path_list_i;
      }
    }
  }
  it.root_is_absolute = it.root_length > 0 &&
                        cpj_path_is_separator(
                          path_style, it.path_list_p[0].ptr[it.root_length - 1]
                        );
  it.list_pos = it.path_list_count;
  it.pos = CPJ_SIZE_MAX;
  if (remove_trailing_slash) {
    it.end_with_separator = false;
  } else {
    it.end_with_separator = end_with_separator == 1;
  }
  return it;
} /* cpj_path_interator_init */

cpj_size_t cpj_path_join_multiple(
  cpj_path_style_t path_style, bool is_resolve, bool remove_trailing_slash,
  const cpj_string_t *path_list_p, cpj_size_t path_list_count,
  cpj_char_t *buffer_p, cpj_size_t buffer_size
)
{
  cpj_size_t buffer_size_calculated = 0;
  for (;;) {
    cpj_segment_iterator_t it = cpj_path_interator_init(
      path_style, is_resolve, remove_trailing_slash, path_list_p,
      path_list_count
    );
    cpj_size_t buffer_index = CPJ_SIZE_MAX;
    cpj_char_t *buffer_p_used;

    if (buffer_size_calculated == 0) {
      buffer_p_used = NULL;
      /* For calculating the path size */
      buffer_index = CPJ_SIZE_MAX;
    } else {
      buffer_p_used = buffer_p;
      if (buffer_size_calculated > buffer_size) {
        /**
         * When `buffer_p` exist and `buffer_size_calculated > buffer_size`,
         * that means there is not enough buffer to storage the final generated
         * path, so that set buffer_index_init to `buffer_size_calculated` to
         * ensure only the head part of the generated path are stored into
         * `buffer_p`
         */
        buffer_index = buffer_size_calculated;
      } else {
        if (buffer_p == it.path_list_p[0].ptr && it.path_list_count == 1) {
          /**
           * The input path and output buffer are the same, storing the
           * generated path at the tail of `buffer_p`; so that when normalizing
           * the path inplace, the path won't be corrupted.
           */
          buffer_index = buffer_size;
        } else {
          buffer_index = buffer_size_calculated;
        }
      }
    }
    cpj_path_push_front_char(
      path_style, buffer_p_used, buffer_size, &buffer_index, '\0'
    );
    for (; cpj_path_get_prev_segment(path_style, &it);) {
      if (it.end_with_separator) {
        cpj_path_push_front_char(
          path_style, buffer_p_used, buffer_size, &buffer_index, '/'
        );
      }
      cpj_string_t segment = cpj_path_get_segment(&it);
      cpj_path_push_front_string(
        path_style, buffer_p_used, buffer_size, &buffer_index, &segment
      );
    }
    if (buffer_size_calculated == 0) {
      buffer_size_calculated = CPJ_SIZE_MAX - buffer_index;
      if (!buffer_p) {
        return buffer_size_calculated - 1;
      }
      continue;
    }
    if (buffer_p) {
      if (buffer_index > 0 && buffer_index < buffer_size) {
        memmove(buffer_p, buffer_p + buffer_index, buffer_size_calculated);
      }
    }
    return buffer_size_calculated - 1;
  }
} /* cpj_path_join_multiple */

bool cpj_path_get_basename(
  cpj_path_style_t path_style, const cpj_string_t *path, cpj_string_t *basename
)
{
  cpj_segment_iterator_t
    it_base = cpj_path_interator_init(path_style, false, true, path, 1);
  // We get the last segment of the path. The last segment will contain the
  // basename if there is any. If there are no segments we will set the basename
  // to NULL and the length to 0.
  bool segment_is_get = cpj_path_get_prev_segment(path_style, &it_base);
  bool path_is_root = (it_base.pos + 1) < it_base.root_length;
  if (path->size == 0 || !segment_is_get || path_is_root) {
    basename->ptr = NULL;
    basename->size = 0;
  } else {
    // Now we can just output the segment contents, since that's our basename.
    // There might be trailing separators after the basename, but the size does
    // not include those.
    *basename = cpj_path_get_segment(&it_base);
  }
  return path_is_root;
} /* cpj_path_get_basename */

static bool cpj_path_is_string_equal(
  cpj_path_style_t path_style, const cpj_char_t *first,
  const cpj_char_t *second, cpj_size_t first_size, cpj_size_t second_size
)
{
  bool are_both_separators;

  // The two strings are not equal if the sizes are not equal.
  if (first_size != second_size) {
    return false;
  }

  // If the path style is UNIX, we will compare case sensitively. This can be
  // done easily using strncmp.
  if (path_style == CPJ_STYLE_UNIX) {
    return strncmp(first, second, first_size) == 0;
  }

  // However, if this is windows we will have to compare case insensitively.
  // Since there is no standard method to do that we will have to do it on our
  // own.
  while (first_size > 0) {
    int a = *first;
    int b = *second;
    if (!(a && b)) {
      break;
    }

    // We can consider the string to be not equal if the two lowercase
    // characters are not equal. The two chars may also be separators, which
    // means they would be equal.
    are_both_separators = cpj_path_is_separator(path_style, a) &&
                          cpj_path_is_separator(path_style, b);

    if (tolower(a) != tolower(b) && !are_both_separators) {
      return false;
    }

    first++;
    second++;

    --first_size;
  }

  // The string must be equal since they both have the same length and all the
  // characters are the same.
  return true;
}

cpj_path_intersection_t cpj_path_get_intersection_segments(
  cpj_path_style_t path_style, const cpj_string_t *path_base,
  const cpj_string_t *path_other, cpj_size_t path_count
)
{
  cpj_path_intersection_t intersection;
  cpj_segment_iterator_t it_base =
    cpj_path_interator_init(path_style, true, true, path_base, path_count);
  cpj_segment_iterator_t it_other =
    cpj_path_interator_init(path_style, true, true, path_other, path_count);
  cpj_size_t k;
  intersection.equal_segment = 0;
  while (cpj_path_get_prev_segment(path_style, &it_base)) {
  }
  while (cpj_path_get_prev_segment(path_style, &it_other)) {
  }
  intersection.segment_count_base = it_base.segment_count;
  intersection.segment_count_other = it_other.segment_count;

  it_base =
    cpj_path_interator_init(path_style, true, true, path_base, path_count);
  it_other =
    cpj_path_interator_init(path_style, true, true, path_other, path_count);
  if (intersection.segment_count_base > intersection.segment_count_other) {
    for (k = intersection.segment_count_base;
         k > intersection.segment_count_other;) {
      k -= 1;
      cpj_path_get_prev_segment(path_style, &it_base);
    }
  } else if (intersection.segment_count_base <
             intersection.segment_count_other) {
    for (k = intersection.segment_count_other;
         k > intersection.segment_count_base;) {
      k -= 1;
      cpj_path_get_prev_segment(path_style, &it_other);
    }
  } else {
    k = intersection.segment_count_base;
  }
  for (; k > 0;) {
    k -= 1;
    cpj_path_get_prev_segment(path_style, &it_base);
    cpj_path_get_prev_segment(path_style, &it_other);
    cpj_string_t segment_base = cpj_path_get_segment(&it_base);
    cpj_string_t segment_other = cpj_path_get_segment(&it_other);
    if (segment_base.size != segment_other.size) {
      intersection.equal_segment = 0;
      continue;
    }
    if (!cpj_path_is_string_equal(
          path_style, segment_base.ptr, segment_other.ptr, segment_base.size,
          segment_base.size
        )) {
      intersection.equal_segment = 0;
      continue;
    }
    intersection.equal_segment += 1;
  }
  return intersection;
}

static cpj_size_t cpj_path_relative_generate(
  cpj_path_style_t path_style, cpj_path_intersection_t *intersection,
  cpj_char_t *buffer, cpj_size_t buffer_size, cpj_size_t buffer_index
)
{
  cpj_size_t segment_eat_count;
  if (intersection->equal_segment == intersection->segment_count_base) {
    if (intersection->equal_segment == intersection->segment_count_other) {
      cpj_path_push_front_char(
        path_style, buffer, buffer_size, &buffer_index, '\0'
      );
      cpj_path_push_front_char(
        path_style, buffer, buffer_size, &buffer_index, '.'
      );
      segment_eat_count = 2;
    } else {
      segment_eat_count = 0;
    }
  } else {
    cpj_size_t k = intersection->segment_count_base;
    segment_eat_count = 0;
    if (intersection->equal_segment == intersection->segment_count_other) {
      --k;
      cpj_path_push_front_char(
        path_style, buffer, buffer_size, &buffer_index, '\0'
      );
      cpj_path_push_front_char(
        path_style, buffer, buffer_size, &buffer_index, '.'
      );
      cpj_path_push_front_char(
        path_style, buffer, buffer_size, &buffer_index, '.'
      );
      segment_eat_count += 3;
    }
    for (; k > intersection->equal_segment;) {
      --k;
      cpj_path_push_front_char(
        path_style, buffer, buffer_size, &buffer_index, '/'
      );
      cpj_path_push_front_char(
        path_style, buffer, buffer_size, &buffer_index, '.'
      );
      cpj_path_push_front_char(
        path_style, buffer, buffer_size, &buffer_index, '.'
      );
      segment_eat_count += 3;
    }
  }
  return segment_eat_count;
}

cpj_size_t cpj_path_get_relative(
  cpj_path_style_t path_style, const cpj_string_t *cwd_directory,
  const cpj_string_t *path_directory, const cpj_string_t *path,
  cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  const cpj_string_t path_base_original[] = {*cwd_directory, *path_directory};
  const cpj_string_t path_other_original[] = {*cwd_directory, *path};
  const cpj_string_t *path_base = path_base_original + 2 - 1;
  const cpj_string_t *path_other = path_other_original + 2 - 1;
  cpj_size_t path_count = 1;
  cpj_path_intersection_t intersection = cpj_path_get_intersection_segments(
    path_style, path_base, path_other, path_count
  );
  if (intersection.equal_segment == 0) {
    path_base = path_base_original;
    path_other = path_other_original;
    path_count = 2;
    intersection = cpj_path_get_intersection_segments(
      path_style, path_base, path_other, path_count
    );
    if (intersection.equal_segment == 0) {
      return cpj_path_join_multiple(
        path_style, true, true, path_other, path_count, buffer, buffer_size
      );
    }
  }
  {
    cpj_segment_iterator_t it_other =
      cpj_path_interator_init(path_style, true, true, path_other, path_count);
    cpj_size_t segment_eat_count =
      cpj_path_relative_generate(path_style, &intersection, NULL, 0, 0);
    cpj_size_t other_path_length;
    if (intersection.equal_segment == intersection.segment_count_other) {
      other_path_length = 0;
    } else {
      cpj_size_t k = intersection.segment_count_other;
      for (; k > intersection.equal_segment;) {
        k -= 1;
        cpj_path_get_prev_segment(path_style, &it_other);
      }
      const cpj_string_t segment = cpj_path_get_segment(&it_other);
      cpj_string_t path_other_normalize[2];
      cpj_size_t path_other_normalize_count;
      path_other_normalize[0] = it_other.path_list_p[it_other.list_pos];
      path_other_normalize[0].ptr = segment.ptr;
      path_other_normalize[0]
        .size = it_other.path_list_p[it_other.list_pos].ptr +
                it_other.path_list_p[it_other.list_pos].size - segment.ptr;
      if (it_other.list_pos == 0 && it_other.path_list_count == 2) {
        path_other_normalize_count = 2;
        path_other_normalize[1] = it_other.path_list_p[1];
      } else {
        path_other_normalize_count = 1;
      }
      cpj_char_t *buffer_other = NULL;
      cpj_size_t buffer_size_other = 0;
      if (buffer && buffer_size > segment_eat_count) {
        buffer_other = buffer + segment_eat_count;
        buffer_size_other = buffer_size - segment_eat_count;
      }
      other_path_length = cpj_path_join_multiple(
        path_style, true, true, path_other_normalize,
        path_other_normalize_count, buffer_other, buffer_size_other
      );
    }
    if (buffer) {
      cpj_path_relative_generate(
        path_style, &intersection, buffer, buffer_size, segment_eat_count
      );
    }
    return other_path_length == 0 ? segment_eat_count - 1
                                  : segment_eat_count + other_path_length;
  }
  return 0;
}

bool cpj_path_is_absolute(cpj_path_style_t path_style, const cpj_char_t *path)
{
  // We grab the root of the path. This root does not include the first
  // separator of a path.
  cpj_size_t length = cpj_path_get_root(path_style, path);

  // Now we can determine whether the root is absolute or not.
  return length > 0 ? cpj_path_is_separator(path_style, path[length - 1])
                    : false;
}

bool cpj_path_is_relative(cpj_path_style_t path_style, const cpj_char_t *path)
{
  // The path is relative if it is not absolute.
  return !cpj_path_is_absolute(path_style, path);
}

cpj_size_t cpj_path_change_root(
  cpj_path_style_t path_style, const cpj_string_t *path,
  const cpj_string_t *new_root, cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  cpj_size_t root_length = cpj_path_get_root(path_style, path->ptr);
  cpj_string_t paths[2];
  paths[0] = *new_root;
  paths[1].ptr = path->ptr + root_length;
  paths[1].size = path->size - root_length;
  return cpj_path_join_multiple(
    path_style, false, true, paths, 2, buffer, buffer_size
  );
}

cpj_size_t cpj_path_get_intersection(
  cpj_path_style_t path_style, const cpj_string_t *path_base,
  const cpj_string_t *path_other
)
{
  cpj_path_intersection_t intersection =
    cpj_path_get_intersection_segments(path_style, path_base, path_other, 1);
  cpj_segment_iterator_t
    it_base = cpj_path_interator_init(path_style, false, true, path_base, 1);
  cpj_size_t k;
  if (intersection.equal_segment == 0) {
    return 0;
  }
  for (k = intersection.segment_count_base; k >= intersection.equal_segment;) {
    k -= 1;
    cpj_path_get_prev_segment(path_style, &it_base);
  }
  cpj_string_t segment = cpj_path_get_segment(&it_base);
  return segment.ptr - path_base->ptr + segment.size;
}

cpj_size_t cpj_path_change_basename(
  cpj_path_style_t path_style, const cpj_string_t *path,
  const cpj_string_t *new_basename, cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  cpj_string_t paths[2] = {*path, *new_basename};
  cpj_string_t basename;
  // First we try to get the last segment. We may only have a root without any
  // segments, in which case we will create one.
  cpj_path_get_basename(path_style, path, &basename);
  paths[0].size -= basename.size;
  return cpj_path_join_multiple(
    path_style, true, true, paths, 2, buffer, buffer_size
  );
}

bool cpj_path_get_extension(
  cpj_path_style_t path_style, const cpj_string_t *path, cpj_string_t *extension
)
{
  const cpj_char_t *c;
  cpj_string_t basename;
  // First we try to get the last segment. We may only have a root without any
  // segments, in which case we will create one.
  cpj_path_get_basename(path_style, path, &basename);

  // We get the last segment of the path. The last segment will contain the
  // extension if there is any.
  if (basename.size == 0) {
    return false;
  }

  // Now we search for a dot within the segment. If there is a dot, we consider
  // the rest of the segment the extension. We do this from the end towards the
  // beginning, since we want to find the last dot.
  for (c = basename.ptr + basename.size - 1; c >= basename.ptr; --c) {
    if (*c == '.') {
      // Okay, we found an extension. We can stop looking now.
      if (extension) {
        extension->ptr = c;
        extension->size = (cpj_size_t)(basename.ptr + basename.size - c);
      }
      return true;
    }
  }

  // We couldn't find any extension.
  return false;
}

cpj_size_t
cpj_path_get_dirname(cpj_path_style_t path_style, const cpj_string_t *path)
{
  cpj_string_t basename;
  // First we try to get the last segment. We may only have a root without any
  // segments, in which case we will create one.
  cpj_path_get_basename(path_style, path, &basename);

  // We can now return the length from the beginning of the string up to the
  // beginning of the last segment.
  return basename.ptr ? (cpj_size_t)(basename.ptr - path->ptr) : path->size;
}

cpj_size_t cpj_path_change_extension(
  cpj_path_style_t path_style, const cpj_string_t *path,
  const cpj_string_t *new_extension, cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  bool new_extention_start_with_dot = new_extension->size > 0 &&
                                      new_extension->ptr[0] == '.';
  if (cpj_path_get_root(path_style, path->ptr) == path->size) {
    cpj_size_t buffer_size_needed = path->size + new_extension->size + 1;
    if (!new_extention_start_with_dot) {
      buffer_size_needed += 1;
    }
    cpj_size_t buffer_index = buffer_size_needed;
    cpj_path_push_front_char(
      path_style, buffer, buffer_size, &buffer_index, '\0'
    );
    cpj_path_push_front_string(
      path_style, buffer, buffer_size, &buffer_index, new_extension
    );
    if (!new_extention_start_with_dot) {
      cpj_path_push_front_char(
        path_style, buffer, buffer_size, &buffer_index, '.'
      );
    }
    cpj_path_push_front_string(
      path_style, buffer, buffer_size, &buffer_index, path
    );
    return buffer_size_needed - 1;
  } else {
    cpj_string_t paths[2] = {*path, *new_extension};
    cpj_string_t extention;
    if (cpj_path_get_extension(path_style, &(paths[0]), &extention)) {
      paths[0].size = extention.ptr - paths[0].ptr;
    }
    if (new_extention_start_with_dot) {
      paths[1].ptr += 1;
      paths[1].size -= 1;
    }
    cpj_size_t path_size = cpj_path_join_multiple(
      path_style, true, true, paths, 2, buffer, buffer_size
    );
    if (paths[1].size > 0) {
      cpj_size_t pos = path_size - paths[1].size;
      if (pos > 0 && pos < buffer_size) {
        buffer[pos - 1] = '.';
      }
    }
    return path_size;
  }
}

cpj_path_style_t cpj_path_guess_style(const cpj_string_t *path)
{
  const cpj_char_t *c;
  cpj_size_t root_length;
  cpj_string_t basename;

  // First we determine the root. Only windows roots can be longer than a single
  // slash, so if we can determine that it starts with something like "C:", we
  // know that this is a windows path.
  root_length = cpj_path_get_root_windows(path->ptr);
  if (root_length > 1) {
    return CPJ_STYLE_WINDOWS;
  }

  // Next we check for slashes. Windows uses backslashes, while unix uses
  // forward slashes. Windows actually supports both, but our best guess is to
  // assume windows with backslashes and unix with forward slashes.
  for (c = path->ptr; *c; ++c) {
    if (*c == '\\') {
      return CPJ_STYLE_WINDOWS;
    }
  }

  for (c = path->ptr; *c; ++c) {
    if (*c == '/') {
      return CPJ_STYLE_UNIX;
    }
  }

  // This path does not have any slashes. We grab the last segment (which
  // actually must be the first one), and determine whether the segment starts
  // with a dot. A dot is a hidden folder or file in the UNIX world, in that
  // case we assume the path to have UNIX style.
  cpj_path_get_basename(CPJ_STYLE_UNIX, path, &basename);
  if (basename.ptr == NULL) {
    // We couldn't find any segments, so we default to a UNIX path style since
    // there is no way to make any assumptions.
    return CPJ_STYLE_UNIX;
  }

  if (*basename.ptr == '.') {
    return CPJ_STYLE_UNIX;
  }

  // And finally we check whether the last segment contains a dot. If it
  // contains a dot, that might be an extension. Windows is more likely to have
  // file names with extensions, so our guess would be windows.
  for (c = basename.ptr; *c; ++c) {
    if (*c == '.') {
      return CPJ_STYLE_WINDOWS;
    }
  }

  // All our checks failed, so we will return a default value which is currently
  // UNIX.
  return CPJ_STYLE_UNIX;
}
