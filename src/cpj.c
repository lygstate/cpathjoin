#include <assert.h>
#include <cpj.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

/**
 * This is a list of separators used in different styles. Windows can read
 * multiple separators, but it generally outputs just a backslash. The output
 * will always use the first character for the output.
 */
static const cpj_char_t *separators[] = {
  "\\/", // CPJ_STYLE_WINDOWS
  "/"    // CPJ_STYLE_UNIX
};

/**
 * A joined path represents multiple path strings which are concatenated, but
 * not (necessarily) stored in contiguous memory. The joined path allows to
 * iterate over the segments as if it was one piece of path.
 */
struct cpj_segment_joined
{
  struct cpj_segment segment;
  const cpj_char_t **paths;
  cpj_size_t path_index;
};

static cpj_size_t cpj_path_output_sized(
  cpj_char_t *buffer, cpj_size_t buffer_size, cpj_size_t position,
  const cpj_char_t *str, cpj_size_t length
)
{
  cpj_size_t amount_written;

  // First we determine the amount which we can write to the buffer. There are
  // three cases. In the first case we have enough to store the whole string in
  // it. In the second one we can only store a part of it, and in the third we
  // have no space left.
  if (buffer_size > position + length) {
    amount_written = length;
  } else if (buffer_size > position) {
    amount_written = buffer_size - position;
  } else {
    amount_written = 0;
  }

  // If we actually want to write out something we will do that here. We will
  // always append a '\0', this way we are guaranteed to have a valid string at
  // all times.
  if (amount_written > 0) {
    memmove(&buffer[position], str, amount_written);
  }

  // Return the theoretical length which would have been written when everything
  // would have fit in the buffer.
  return length;
}

static cpj_size_t cpj_path_output_current(
  cpj_char_t *buffer, cpj_size_t buffer_size, cpj_size_t position
)
{
  // We output a "current" directory, which is a single character. This
  // character is currently not style dependant.
  return cpj_path_output_sized(buffer, buffer_size, position, ".", 1);
}

static cpj_size_t cpj_path_output_back(
  cpj_char_t *buffer, cpj_size_t buffer_size, cpj_size_t position
)
{
  // We output a "back" directory, which ahs two characters. This
  // character is currently not style dependant.
  return cpj_path_output_sized(buffer, buffer_size, position, "..", 2);
}

static cpj_size_t cpj_path_output_separator(
  cpj_path_style_t path_style, cpj_char_t *buffer, cpj_size_t buffer_size,
  cpj_size_t position
)
{
  // We output a separator, which is a single character.
  return cpj_path_output_sized(
    buffer, buffer_size, position, separators[path_style], 1
  );
}

static cpj_size_t cpj_path_output_dot(
  cpj_char_t *buffer, cpj_size_t buffer_size, cpj_size_t position
)
{
  // We output a dot, which is a single character. This is used for extensions.
  return cpj_path_output_sized(buffer, buffer_size, position, ".", 1);
}

static cpj_size_t cpj_path_output(
  cpj_char_t *buffer, cpj_size_t buffer_size, cpj_size_t position,
  const cpj_char_t *str
)
{
  cpj_size_t length;

  // This just does a sized output internally, but first measuring the
  // null-terminated string.
  length = strlen(str);
  return cpj_path_output_sized(buffer, buffer_size, position, str, length);
}

static void cpj_path_terminate_output(
  cpj_char_t *buffer, cpj_size_t buffer_size, cpj_size_t pos
)
{
  if (buffer_size > 0) {
    if (pos >= buffer_size) {
      buffer[buffer_size - 1] = '\0';
    } else {
      buffer[pos] = '\0';
    }
  }
}

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
    are_both_separators = strchr(separators[path_style], a) != NULL &&
                          strchr(separators[path_style], b) != NULL;

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

static const cpj_char_t *
cpj_path_find_next_stop(cpj_path_style_t path_style, const cpj_char_t *c)
{
  // We just move forward until we find a '\0' or a separator, which will be our
  // next "stop".
  while (*c != '\0' && !cpj_path_is_separator(path_style, c)) {
    ++c;
  }

  // Return the pointer of the next stop.
  return c;
}

static const cpj_char_t *cpj_path_find_previous_stop(
  cpj_path_style_t path_style, const cpj_char_t *begin, const cpj_char_t *c
)
{
  // We just move back until we find a separator or reach the beginning of the
  // path, which will be our previous "stop".
  while (c > begin && !cpj_path_is_separator(path_style, c)) {
    --c;
  }

  // Return the pointer to the previous stop. We have to return the first
  // character after the separator, not on the separator itself.
  if (cpj_path_is_separator(path_style, c)) {
    return c + 1;
  } else {
    return c;
  }
}

static bool cpj_path_get_first_segment_without_root(
  cpj_path_style_t path_style, const cpj_char_t *path,
  const cpj_char_t *segments, struct cpj_segment *segment
)
{
  // Let's remember the path. We will move the path pointer afterwards, that's
  // why this has to be done first.
  segment->path = path;
  segment->segments = segments;
  segment->begin = segments;
  segment->end = segments;
  segment->size = 0;

  // Now let's check whether this is an empty string. An empty string has no
  // segment it could use.
  if (*segments == '\0') {
    return false;
  }

  // If the string starts with separators, we will jump over those. If there is
  // only a slash and a '\0' after it, we can't determine the first segment
  // since there is none.
  while (cpj_path_is_separator(path_style, segments)) {
    ++segments;
    if (*segments == '\0') {
      return false;
    }
  }

  // So this is the beginning of our segment.
  segment->begin = segments;

  // Now let's determine the end of the segment, which we do by moving the path
  // pointer further until we find a separator.
  segments = cpj_path_find_next_stop(path_style, segments);

  // And finally, calculate the size of the segment by subtracting the position
  // from the end.
  segment->size = (cpj_size_t)(segments - segment->begin);
  segment->end = segments;

  // Tell the caller that we found a segment.
  return true;
}

static bool cpj_path_get_last_segment_without_root(
  cpj_path_style_t path_style, const cpj_char_t *path,
  struct cpj_segment *segment
)
{
  // Now this is fairly similar to the normal algorithm, however, it will assume
  // that there is no root in the path. So we grab the first segment at this
  // position, assuming there is no root.
  if (!cpj_path_get_first_segment_without_root(
        path_style, path, path, segment
      )) {
    return false;
  }

  // Now we find our last segment. The segment struct of the caller
  // will contain the last segment, since the function we call here will not
  // change the segment struct when it reaches the end.
  while (cpj_path_get_next_segment(path_style, segment)) {
    // We just loop until there is no other segment left.
  }

  return true;
}

static bool cpj_path_get_first_segment_joined(
  cpj_path_style_t path_style, const cpj_char_t **paths,
  struct cpj_segment_joined *sj
)
{
  bool result;

  // Prepare the first segment. We position the joined segment on the first path
  // and assign the path array to the struct.
  sj->path_index = 0;
  sj->paths = paths;

  // We loop through all paths until we find one which has a segment. The result
  // is stored in a variable, so we can let the caller know whether we found one
  // or not.
  result = false;
  while (paths[sj->path_index] != NULL &&
         (result = cpj_path_get_first_segment(
            path_style, paths[sj->path_index], &sj->segment
          )) == false) {
    ++sj->path_index;
  }

  return result;
}

static bool cpj_path_get_next_segment_joined(
  cpj_path_style_t path_style, struct cpj_segment_joined *sj
)
{
  bool result;

  if (sj->paths[sj->path_index] == NULL) {
    // We reached already the end of all paths, so there is no other segment
    // left.
    return false;
  } else if (cpj_path_get_next_segment(path_style, &sj->segment)) {
    // There was another segment on the current path, so we are good to
    // continue.
    return true;
  }

  // We try to move to the next path which has a segment available. We must at
  // least move one further since the current path reached the end.
  result = false;

  do {
    ++sj->path_index;

    // And we obviously have to stop this loop if there are no more paths left.
    if (sj->paths[sj->path_index] == NULL) {
      break;
    }

    // Grab the first segment of the next path and determine whether this path
    // has anything useful in it. There is one more thing we have to consider
    // here - for the first time we do this we want to skip the root, but
    // afterwards we will consider that to be part of the segments.
    result = cpj_path_get_first_segment_without_root(
      path_style, sj->paths[sj->path_index], sj->paths[sj->path_index],
      &sj->segment
    );

  } while (!result);

  // Finally, report the result back to the caller.
  return result;
}

static bool cpj_path_get_previous_segment_joined(
  cpj_path_style_t path_style, struct cpj_segment_joined *sj
)
{
  bool result;

  if (*sj->paths == NULL) {
    // It's possible that there is no initialized segment available in the
    // struct since there are no paths. In that case we can return false, since
    // there is no previous segment.
    return false;
  } else if (cpj_path_get_previous_segment(path_style, &sj->segment)) {
    // Now we try to get the previous segment from the current path. If we can
    // do that successfully, we can let the caller know that we found one.
    return true;
  }

  result = false;

  do {
    // We are done once we reached index 0. In that case there are no more
    // segments left.
    if (sj->path_index == 0) {
      break;
    }

    // There is another path which we have to inspect. So we decrease the path
    // index.
    --sj->path_index;

    // If this is the first path we will have to consider that this path might
    // include a root, otherwise we just treat is as a segment.
    if (sj->path_index == 0) {
      result = cpj_path_get_last_segment(
        path_style, sj->paths[sj->path_index], &sj->segment
      );
    } else {
      result = cpj_path_get_last_segment_without_root(
        path_style, sj->paths[sj->path_index], &sj->segment
      );
    }

  } while (!result);

  return result;
}

static bool cpj_path_segment_back_will_be_removed(
  cpj_path_style_t path_style, struct cpj_segment_joined *sj
)
{
  enum cpj_segment_type type;
  int counter;

  // We are handling back segments here. We must verify how many back segments
  // and how many normal segments come before this one to decide whether we keep
  // or remove it.

  // The counter determines how many normal segments are our current segment,
  // which will popped off before us. If the counter goes above zero it means
  // that our segment will be popped as well.
  counter = 0;

  // We loop over all previous segments until we either reach the beginning,
  // which means our segment will not be dropped or the counter goes above zero.
  while (cpj_path_get_previous_segment_joined(path_style, sj)) {

    // Now grab the type. The type determines whether we will increase or
    // decrease the counter. We don't handle a CPJ_CURRENT frame here since it
    // has no influence.
    type = cpj_path_get_segment_type(&sj->segment);
    if (type == CPJ_NORMAL) {
      // This is a normal segment. The normal segment will increase the counter
      // since it neutralizes one back segment. If we go above zero we can
      // return immediately.
      ++counter;
      if (counter > 0) {
        return true;
      }
    } else if (type == CPJ_BACK) {
      // A CPJ_BACK segment will reduce the counter by one. We can not remove a
      // back segment as long we are not above zero since we don't have the
      // opposite normal segment which we would remove.
      --counter;
    }
  }

  // We never got a count larger than zero, so we will keep this segment alive.
  return false;
}

static bool cpj_path_segment_normal_will_be_removed(
  cpj_path_style_t path_style, struct cpj_segment_joined *sj
)
{
  enum cpj_segment_type type;
  int counter;

  // The counter determines how many segments are above our current segment,
  // which will popped off before us. If the counter goes below zero it means
  // that our segment will be popped as well.
  counter = 0;

  // We loop over all following segments until we either reach the end, which
  // means our segment will not be dropped or the counter goes below zero.
  while (cpj_path_get_next_segment_joined(path_style, sj)) {

    // First, grab the type. The type determines whether we will increase or
    // decrease the counter. We don't handle a CPJ_CURRENT frame here since it
    // has no influence.
    type = cpj_path_get_segment_type(&sj->segment);
    if (type == CPJ_NORMAL) {
      // This is a normal segment. The normal segment will increase the counter
      // since it will be removed by a "../" before us.
      ++counter;
    } else if (type == CPJ_BACK) {
      // A CPJ_BACK segment will reduce the counter by one. If we are below zero
      // we can return immediately.
      --counter;
      if (counter < 0) {
        return true;
      }
    }
  }

  // We never got a negative count, so we will keep this segment alive.
  return false;
}

static bool cpj_path_segment_will_be_removed(
  cpj_path_style_t path_style, const struct cpj_segment_joined *sj,
  bool absolute
)
{
  enum cpj_segment_type type;
  struct cpj_segment_joined sjc;

  // We copy the joined path so we don't need to modify it.
  sjc = *sj;

  // First we check whether this is a CPJ_CURRENT or CPJ_BACK segment, since
  // those will always be dropped.
  type = cpj_path_get_segment_type(&sj->segment);
  if (type == CPJ_CURRENT || (type == CPJ_BACK && absolute)) {
    return true;
  } else if (type == CPJ_BACK) {
    return cpj_path_segment_back_will_be_removed(path_style, &sjc);
  } else {
    return cpj_path_segment_normal_will_be_removed(path_style, &sjc);
  }
}

static bool cpj_path_segment_joined_skip_invisible(
  cpj_path_style_t path_style, struct cpj_segment_joined *sj, bool absolute
)
{
  while (cpj_path_segment_will_be_removed(path_style, sj, absolute)) {
    if (!cpj_path_get_next_segment_joined(path_style, sj)) {
      return false;
    }
  }

  return true;
}

static void
cpj_path_get_root_windows(const cpj_char_t *path, cpj_size_t *length)
{
  const cpj_char_t *c;
  bool is_device_path;

  // We can not determine the root if this is an empty string. So we set the
  // root to NULL and the length to zero and cancel the whole thing.
  c = path;
  *length = 0;
  if (!*c) {
    return;
  }

  // Now we have to verify whether this is a windows network path (UNC), which
  // we will consider our root.
  if (cpj_path_is_separator(CPJ_STYLE_WINDOWS, c)) {
    ++c;

    // Check whether the path starts with a single backslash, which means this
    // is not a network path - just a normal path starting with a backslash.
    if (!cpj_path_is_separator(CPJ_STYLE_WINDOWS, c)) {
      // Okay, this is not a network path but we still use the backslash as a
      // root.
      ++(*length);
      return;
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
                     cpj_path_is_separator(CPJ_STYLE_WINDOWS, ++c);
    if (is_device_path) {
      // That's a device path, and the root must be either "\\.\" or "\\?\"
      // which is 4 characters long. (at least that's how Windows
      // GetFullPathName behaves.)
      *length = 4;
      return;
    }

    // We will grab anything up to the next stop. The next stop might be a '\0'
    // or another separator. That will be the server name.
    c = cpj_path_find_next_stop(CPJ_STYLE_WINDOWS, c);

    // If this is a separator and not the end of a string we wil have to include
    // it. However, if this is a '\0' we must not skip it.
    while (cpj_path_is_separator(CPJ_STYLE_WINDOWS, c)) {
      ++c;
    }

    // We are now skipping the shared folder name, which will end after the
    // next stop.
    c = cpj_path_find_next_stop(CPJ_STYLE_WINDOWS, c);

    // Then there might be a separator at the end. We will include that as well,
    // it will mark the path as absolute.
    if (cpj_path_is_separator(CPJ_STYLE_WINDOWS, c)) {
      ++c;
    }

    // Finally, calculate the size of the root.
    *length = (cpj_size_t)(c - path);
    return;
  }

  // Move to the next and check whether this is a colon.
  if (*++c == ':') {
    *length = 2;

    // Now check whether this is a backslash (or slash). If it is not, we could
    // assume that the next character is a '\0' if it is a valid path. However,
    // we will not assume that - since ':' is not valid in a path it must be a
    // mistake by the caller than. We will try to understand it anyway.
    if (cpj_path_is_separator(CPJ_STYLE_WINDOWS, ++c)) {
      *length = 3;
    }
  }
}

static void cpj_path_get_root_unix(const cpj_char_t *path, cpj_size_t *length)
{
  // The slash of the unix path represents the root. There is no root if there
  // is no slash.
  if (cpj_path_is_separator(CPJ_STYLE_UNIX, path)) {
    *length = 1;
  } else {
    *length = 0;
  }
}

static bool cpj_path_is_root_absolute(
  cpj_path_style_t path_style, const cpj_char_t *path, cpj_size_t length
)
{
  // This is definitely not absolute if there is no root.
  if (length == 0) {
    return false;
  }

  // If there is a separator at the end of the root, we can safely consider this
  // to be an absolute path.
  return cpj_path_is_separator(path_style, &path[length - 1]);
}

static void cpj_path_fix_root(
  cpj_path_style_t path_style, cpj_char_t *buffer, cpj_size_t buffer_size,
  cpj_size_t length
)
{
  cpj_size_t i;

  // This only affects windows.
  if (path_style != CPJ_STYLE_WINDOWS) {
    return;
  }

  // Make sure we are not writing further than we are actually allowed to.
  if (length > buffer_size) {
    length = buffer_size;
  }

  // Replace all forward slashes with backwards slashes. Since this is windows
  // we can't have any forward slashes in the root.
  for (i = 0; i < length; ++i) {
    if (cpj_path_is_separator(CPJ_STYLE_WINDOWS, &buffer[i])) {
      buffer[i] = *separators[CPJ_STYLE_WINDOWS];
    }
  }
}

static cpj_size_t cpj_path_join_and_normalize_multiple(
  cpj_path_style_t path_style, const cpj_char_t **paths, cpj_char_t *buffer,
  cpj_size_t buffer_size
)
{
  cpj_size_t pos;
  bool absolute, has_segment_output;
  struct cpj_segment_joined sj;

  // We initialize the position after the root, which should get us started.
  pos = cpj_path_get_root(path_style, paths[0]);

  // Determine whether the path is absolute or not. We need that to determine
  // later on whether we can remove superfluous "../" or not.
  absolute = cpj_path_is_root_absolute(path_style, paths[0], pos);

  // First copy the root to the output. After copying, we will normalize the
  // root.
  cpj_path_output_sized(buffer, buffer_size, 0, paths[0], pos);
  cpj_path_fix_root(path_style, buffer, buffer_size, pos);

  // So we just grab the first segment. If there is no segment we will always
  // output a "/", since we currently only support absolute paths here.
  if (!cpj_path_get_first_segment_joined(path_style, paths, &sj)) {
    goto done;
  }

  // Let's assume that we don't have any segment output for now. We will toggle
  // this flag once there is some output.
  has_segment_output = false;

  do {
    // Check whether we have to drop this segment because of resolving a
    // relative path or because it is a CPJ_CURRENT segment.
    if (cpj_path_segment_will_be_removed(path_style, &sj, absolute)) {
      continue;
    }

    // We add a separator if we previously wrote a segment. The last segment
    // must not have a trailing separator. This must happen before the segment
    // output, since we would override the null terminating character with
    // reused buffers if this was done afterwards.
    if (has_segment_output) {
      pos += cpj_path_output_separator(path_style, buffer, buffer_size, pos);
    }

    // Remember that we have segment output, so we can handle the trailing slash
    // later on. This is necessary since we might have segments but they are all
    // removed.
    has_segment_output = true;

    // Write out the segment but keep in mind that we need to follow the
    // buffer size limitations. That's why we use the path output functions
    // here.
    pos += cpj_path_output_sized(
      buffer, buffer_size, pos, sj.segment.begin, sj.segment.size
    );
  } while (cpj_path_get_next_segment_joined(path_style, &sj));

  // Remove the trailing slash, but only if we have segment output. We don't
  // want to remove anything from the root.
  if (!has_segment_output && pos == 0) {
    // This may happen if the path is absolute and all segments have been
    // removed. We can not have an empty output - and empty output means we stay
    // in the current directory. So we will output a ".".
    assert(absolute == false);
    pos += cpj_path_output_current(buffer, buffer_size, pos);
  }

  // We must append a '\0' in any case, unless the buffer size is zero. If the
  // buffer size is zero, which means we can not.
done:
  cpj_path_terminate_output(buffer, buffer_size, pos);

  // And finally let our caller know about the total size of the normalized
  // path.
  return pos;
}

cpj_size_t cpj_path_get_absolute_test(
  cpj_path_style_t path_style, const cpj_char_t *base, const cpj_char_t *path,
  cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  cpj_size_t i;
  const cpj_char_t *paths[4];

  // The basename should be an absolute path if the caller is using the API
  // correctly. However, he might not and in that case we will append a fake
  // root at the beginning.
  if (cpj_path_is_absolute(path_style, base)) {
    i = 0;
  } else if (path_style == CPJ_STYLE_WINDOWS) {
    paths[0] = "\\";
    i = 1;
  } else {
    paths[0] = "/";
    i = 1;
  }

  if (cpj_path_is_absolute(path_style, path)) {
    // If the submitted path is not relative the base path becomes irrelevant.
    // We will only normalize the submitted path instead.
    paths[i++] = path;
    paths[i] = NULL;
  } else {
    // Otherwise we append the relative path to the base path and normalize it.
    // The result will be a new absolute path.
    paths[i++] = base;
    paths[i++] = path;
    paths[i] = NULL;
  }

  // Finally join everything together and normalize it.
  return cpj_path_join_and_normalize_multiple(
    path_style, paths, buffer, buffer_size
  );
}

static void cpj_path_skip_segments_until_diverge(
  cpj_path_style_t path_style, struct cpj_segment_joined *bsj,
  struct cpj_segment_joined *osj, bool absolute, bool *base_available,
  bool *other_available
)
{
  // Now looping over all segments until they start to diverge. A path may
  // diverge if two segments are not equal or if one path reaches the end.
  do {

    // Check whether there is anything available after we skip everything which
    // is invisible. We do that for both paths, since we want to let the caller
    // know which path has some trailing segments after they diverge.
    *base_available =
      cpj_path_segment_joined_skip_invisible(path_style, bsj, absolute);
    *other_available =
      cpj_path_segment_joined_skip_invisible(path_style, osj, absolute);

    // We are done if one or both of those paths reached the end. They either
    // diverge or both reached the end - but in both cases we can not continue
    // here.
    if (!*base_available || !*other_available) {
      break;
    }

    // Compare the content of both segments. We are done if they are not equal,
    // since they diverge.
    if (!cpj_path_is_string_equal(
          path_style, bsj->segment.begin, osj->segment.begin, bsj->segment.size,
          osj->segment.size
        )) {
      break;
    }

    // We keep going until one of those segments reached the end. The next
    // segment might be invisible, but we will check for that in the beginning
    // of the loop once again.
    *base_available = cpj_path_get_next_segment_joined(path_style, bsj);
    *other_available = cpj_path_get_next_segment_joined(path_style, osj);
  } while (*base_available && *other_available);
}

cpj_size_t cpj_path_get_relative_test(
  cpj_path_style_t path_style, const cpj_char_t *base_directory,
  const cpj_char_t *path, cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  cpj_size_t pos, base_root_length, path_root_length;
  bool absolute, base_available, other_available, has_output;
  const cpj_char_t *base_paths[2], *other_paths[2];
  struct cpj_segment_joined bsj, osj;

  pos = 0;

  // First we compare the roots of those two paths. If the roots are not equal
  // we can't continue, since there is no way to get a relative path from
  // different roots.
  base_root_length = cpj_path_get_root(path_style, base_directory);
  path_root_length = cpj_path_get_root(path_style, path);
  if (base_root_length != path_root_length ||
      !cpj_path_is_string_equal(
        path_style, base_directory, path, base_root_length, path_root_length
      )) {
    cpj_path_terminate_output(buffer, buffer_size, pos);
    return pos;
  }

  // Verify whether this is an absolute path. We need to know that since we can
  // remove all back-segments if it is.
  absolute =
    cpj_path_is_root_absolute(path_style, base_directory, base_root_length);

  // Initialize our joined segments. This will allow us to use the internal
  // functions to skip until diverge and invisible. We only have one path in
  // them though.
  base_paths[0] = base_directory;
  base_paths[1] = NULL;
  other_paths[0] = path;
  other_paths[1] = NULL;
  cpj_path_get_first_segment_joined(path_style, base_paths, &bsj);
  cpj_path_get_first_segment_joined(path_style, other_paths, &osj);

  // Okay, now we skip until the segments diverge. We don't have anything to do
  // with the segments which are equal.
  cpj_path_skip_segments_until_diverge(
    path_style, &bsj, &osj, absolute, &base_available, &other_available
  );

  // Assume there is no output until we have got some. We will need this
  // information later on to remove trailing slashes or alternatively output a
  // current-segment.
  has_output = false;

  // So if we still have some segments left in the base path we will now output
  // a back segment for all of them.
  if (base_available) {
    do {
      // Skip any invisible segment. We don't care about those and we don't need
      // to navigate back because of them.
      if (!cpj_path_segment_joined_skip_invisible(path_style, &bsj, absolute)) {
        break;
      }

      // Toggle the flag if we have output. We need to remember that, since we
      // want to remove the trailing slash.
      has_output = true;

      // Output the back segment and a separator. No need to worry about the
      // superfluous segment since it will be removed later on.
      pos += cpj_path_output_back(buffer, buffer_size, pos);
      pos += cpj_path_output_separator(path_style, buffer, buffer_size, pos);
    } while (cpj_path_get_next_segment_joined(path_style, &bsj));
  }

  // And if we have some segments available of the target path we will output
  // all of those.
  if (other_available) {
    do {
      // Again, skip any invisible segments since we don't need to navigate into
      // them.
      if (!cpj_path_segment_joined_skip_invisible(path_style, &osj, absolute)) {
        break;
      }

      // Toggle the flag if we have output. We need to remember that, since we
      // want to remove the trailing slash.
      has_output = true;

      // Output the current segment and a separator. No need to worry about the
      // superfluous segment since it will be removed later on.
      pos += cpj_path_output_sized(
        buffer, buffer_size, pos, osj.segment.begin, osj.segment.size
      );
      pos += cpj_path_output_separator(path_style, buffer, buffer_size, pos);
    } while (cpj_path_get_next_segment_joined(path_style, &osj));
  }

  // If we have some output by now we will have to remove the trailing slash. We
  // simply do that by moving back one character. The terminate output function
  // will then place the '\0' on this position. Otherwise, if there is no
  // output, we will have to output a "current directory", since the target path
  // points to the base path.
  if (has_output) {
    --pos;
  } else {
    pos += cpj_path_output_current(buffer, buffer_size, pos);
  }

  // Finally, we can terminate the output - which means we place a '\0' at the
  // current position or at the end of the buffer.
  cpj_path_terminate_output(buffer, buffer_size, pos);

  return pos;
}

cpj_size_t cpj_path_join_test(
  cpj_path_style_t path_style, const cpj_char_t *path_a,
  const cpj_char_t *path_b, cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  const cpj_char_t *paths[3];

  // This is simple. We will just create an array with the two paths which we
  // wish to join.
  paths[0] = path_a;
  paths[1] = path_b;
  paths[2] = NULL;

  // And then call the join and normalize function which will do the hard work
  // for us.
  return cpj_path_join_and_normalize_multiple(
    path_style, paths, buffer, buffer_size
  );
}

cpj_size_t cpj_path_join_multiple_test(
  cpj_path_style_t path_style, const cpj_char_t **paths, cpj_char_t *buffer,
  cpj_size_t buffer_size
)
{
  // We can just call the internal join and normalize function for this one,
  // since it will handle everything.
  return cpj_path_join_and_normalize_multiple(
    path_style, paths, buffer, buffer_size
  );
}

cpj_size_t
cpj_path_get_root(cpj_path_style_t path_style, const cpj_char_t *path)
{
  cpj_size_t length;
  // We use a different implementation here based on the configuration of the
  // library.
  if (path_style == CPJ_STYLE_WINDOWS) {
    cpj_path_get_root_windows(path, &length);
  } else {
    cpj_path_get_root_unix(path, &length);
  }
  return length;
}

cpj_size_t cpj_path_change_root_test(
  cpj_path_style_t path_style, const cpj_char_t *path,
  const cpj_char_t *new_root, cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  const cpj_char_t *tail;
  cpj_size_t root_length, path_length, tail_length, new_root_length,
    new_path_size;

  // First we need to determine the actual size of the root which we will
  // change.
  root_length = cpj_path_get_root(path_style, path);

  // Now we determine the sizes of the new root and the path. We need that to
  // determine the size of the part after the root (the tail).
  new_root_length = strlen(new_root);
  path_length = strlen(path);

  // Okay, now we calculate the position of the tail and the length of it.
  tail = path + root_length;
  tail_length = path_length - root_length;

  // We first output the tail and then the new root, that's because the source
  // path and the buffer may be overlapping. This way the root will not
  // overwrite the tail.
  cpj_path_output_sized(
    buffer, buffer_size, new_root_length, tail, tail_length
  );
  cpj_path_output_sized(buffer, buffer_size, 0, new_root, new_root_length);

  // Finally we calculate the size o the new path and terminate the output with
  // a '\0'.
  new_path_size = tail_length + new_root_length;
  cpj_path_terminate_output(buffer, buffer_size, new_path_size);

  return new_path_size;
}

bool cpj_path_is_absolute(cpj_path_style_t path_style, const cpj_char_t *path)
{
  cpj_size_t length;

  // We grab the root of the path. This root does not include the first
  // separator of a path.
  length = cpj_path_get_root(path_style, path);

  // Now we can determine whether the root is absolute or not.
  return cpj_path_is_root_absolute(path_style, path, length);
}

bool cpj_path_is_relative(cpj_path_style_t path_style, const cpj_char_t *path)
{
  // The path is relative if it is not absolute.
  return !cpj_path_is_absolute(path_style, path);
}

void cpj_path_get_basename_test(
  cpj_path_style_t path_style, const cpj_char_t *path,
  const cpj_char_t **basename, cpj_size_t *length
)
{
  struct cpj_segment segment;

  // We get the last segment of the path. The last segment will contain the
  // basename if there is any. If there are no segments we will set the basename
  // to NULL and the length to 0.
  if (!cpj_path_get_last_segment(path_style, path, &segment)) {
    *basename = NULL;
    if (length) {
      *length = 0;
    }
    return;
  }

  // Now we can just output the segment contents, since that's our basename.
  // There might be trailing separators after the basename, but the size does
  // not include those.
  *basename = segment.begin;
  if (length) {
    *length = segment.size;
  }
}

cpj_size_t cpj_path_change_basename_test(
  cpj_path_style_t path_style, const cpj_char_t *path,
  const cpj_char_t *new_basename, cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  struct cpj_segment segment;
  cpj_size_t pos, root_size, new_basename_size;

  // First we try to get the last segment. We may only have a root without any
  // segments, in which case we will create one.
  if (!cpj_path_get_last_segment(path_style, path, &segment)) {

    // So there is no segment in this path. First we grab the root and output
    // that. We are not going to modify the root in any way.
    root_size = cpj_path_get_root(path_style, path);
    pos = cpj_path_output_sized(buffer, buffer_size, 0, path, root_size);

    // We have to trim the separators from the beginning of the new basename.
    // This is quite easy to do.
    while (cpj_path_is_separator(path_style, new_basename)) {
      ++new_basename;
    }

    // Now we measure the length of the new basename, this is a two step
    // process. First we find the '\0' character at the end of the string.
    new_basename_size = 0;
    while (new_basename[new_basename_size]) {
      ++new_basename_size;
    }

    // And then we trim the separators at the end of the basename until we reach
    // the first valid character.
    while (
      new_basename_size > 0 &&
      cpj_path_is_separator(path_style, &new_basename[new_basename_size - 1])
    ) {
      --new_basename_size;
    }

    // Now we will output the new basename after the root.
    pos += cpj_path_output_sized(
      buffer, buffer_size, pos, new_basename, new_basename_size
    );

    // And finally terminate the output and return the total size of the path.
    cpj_path_terminate_output(buffer, buffer_size, pos);
    return pos;
  }

  // If there is a last segment we can just forward this call, which is fairly
  // easy.
  return cpj_path_change_segment(
    path_style, &segment, new_basename, buffer, buffer_size
  );
}

void cpj_path_get_dirname_test(
  cpj_path_style_t path_style, const cpj_char_t *path, cpj_size_t *length
)
{
  struct cpj_segment segment;

  // We get the last segment of the path. The last segment will contain the
  // basename if there is any. If there are no segments we will set the length
  // to 0.
  if (!cpj_path_get_last_segment(path_style, path, &segment)) {
    *length = 0;
    return;
  }

  // We can now return the length from the beginning of the string up to the
  // beginning of the last segment.
  *length = (cpj_size_t)(segment.begin - path);
}

bool cpj_path_get_extension_test(
  cpj_path_style_t path_style, const cpj_char_t *path,
  const cpj_char_t **extension, cpj_size_t *length
)
{
  struct cpj_segment segment;
  const cpj_char_t *c;

  // We get the last segment of the path. The last segment will contain the
  // extension if there is any.
  if (!cpj_path_get_last_segment(path_style, path, &segment)) {
    return false;
  }

  // Now we search for a dot within the segment. If there is a dot, we consider
  // the rest of the segment the extension. We do this from the end towards the
  // beginning, since we want to find the last dot.
  for (c = segment.end; c >= segment.begin; --c) {
    if (*c == '.') {
      // Okay, we found an extension. We can stop looking now.
      *extension = c;
      *length = (cpj_size_t)(segment.end - c);
      return true;
    }
  }

  // We couldn't find any extension.
  return false;
}

bool cpj_path_has_extension_test(cpj_path_style_t path_style, const cpj_char_t *path)
{
  const cpj_char_t *extension;
  cpj_size_t length;

  // We just wrap the get_extension call which will then do the work for us.
  return cpj_path_get_extension_test(path_style, path, &extension, &length);
}

cpj_size_t cpj_path_change_extension_test(
  cpj_path_style_t path_style, const cpj_char_t *path,
  const cpj_char_t *new_extension, cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  struct cpj_segment segment;
  const cpj_char_t *c, *old_extension;
  cpj_size_t pos, root_size, trail_size, new_extension_size;

  // First we try to get the last segment. We may only have a root without any
  // segments, in which case we will create one.
  if (!cpj_path_get_last_segment(path_style, path, &segment)) {

    // So there is no segment in this path. First we grab the root and output
    // that. We are not going to modify the root in any way. If there is no
    // root, this will end up with a root size 0, and nothing will be written.
    root_size = cpj_path_get_root(path_style, path);
    pos = cpj_path_output_sized(buffer, buffer_size, 0, path, root_size);

    // Add a dot if the submitted value doesn't have any.
    if (*new_extension != '.') {
      pos += cpj_path_output_dot(buffer, buffer_size, pos);
    }

    // And finally terminate the output and return the total size of the path.
    pos += cpj_path_output(buffer, buffer_size, pos, new_extension);
    cpj_path_terminate_output(buffer, buffer_size, pos);
    return pos;
  }

  // Now we seek the old extension in the last segment, which we will replace
  // with the new one. If there is no old extension, it will point to the end of
  // the segment.
  old_extension = segment.end;
  for (c = segment.begin; c < segment.end; ++c) {
    if (*c == '.') {
      old_extension = c;
    }
  }

  pos = cpj_path_output_sized(
    buffer, buffer_size, 0, segment.path,
    (cpj_size_t)(old_extension - segment.path)
  );

  // If the new extension starts with a dot, we will skip that dot. We always
  // output exactly one dot before the extension. If the extension contains
  // multiple dots, we will output those as part of the extension.
  if (*new_extension == '.') {
    ++new_extension;
  }

  // We calculate the size of the new extension, including the dot, in order to
  // output the trail - which is any part of the path coming after the
  // extension. We must output this first, since the buffer may overlap with the
  // submitted path - and it would be overridden by longer extensions.
  new_extension_size = strlen(new_extension) + 1;
  trail_size =
    cpj_path_output(buffer, buffer_size, pos + new_extension_size, segment.end);

  // Finally we output the dot and the new extension. The new extension itself
  // doesn't contain the dot anymore, so we must output that first.
  pos += cpj_path_output_dot(buffer, buffer_size, pos);
  pos += cpj_path_output(buffer, buffer_size, pos, new_extension);

  // Now we terminate the output with a null-terminating character, but before
  // we do that we must add the size of the trail to the position which we
  // output before.
  pos += trail_size;
  cpj_path_terminate_output(buffer, buffer_size, pos);

  // And the position is our output size now.
  return pos;
}

cpj_size_t cpj_path_normalize_test(
  cpj_path_style_t path_style, const cpj_char_t *path, cpj_char_t *buffer,
  cpj_size_t buffer_size
)
{
  const cpj_char_t *paths[2];

  // Now we initialize the paths which we will normalize. Since this function
  // only supports submitting a single path, we will only add that one.
  paths[0] = path;
  paths[1] = NULL;

  return cpj_path_join_and_normalize_multiple(
    path_style, paths, buffer, buffer_size
  );
}

cpj_size_t cpj_path_get_intersection_test(
  cpj_path_style_t path_style, const cpj_char_t *path_base,
  const cpj_char_t *path_other
)
{
  bool absolute;
  cpj_size_t base_root_length, other_root_length;
  const cpj_char_t *end;
  const cpj_char_t *paths_base[2], *paths_other[2];
  struct cpj_segment_joined base, other;

  // We first compare the two roots. We just return zero if they are not equal.
  // This will also happen to return zero if the paths are mixed relative and
  // absolute.
  base_root_length = cpj_path_get_root(path_style, path_base);
  other_root_length = cpj_path_get_root(path_style, path_other);
  if (!cpj_path_is_string_equal(
        path_style, path_base, path_other, base_root_length, other_root_length
      )) {
    return 0;
  }

  // Configure our paths. We just have a single path in here for now.
  paths_base[0] = path_base;
  paths_base[1] = NULL;
  paths_other[0] = path_other;
  paths_other[1] = NULL;

  // So we get the first segment of both paths. If one of those paths don't have
  // any segment, we will return 0.
  if (!cpj_path_get_first_segment_joined(path_style, paths_base, &base) ||
      !cpj_path_get_first_segment_joined(path_style, paths_other, &other)) {
    return base_root_length;
  }

  // We now determine whether the path is absolute or not. This is required
  // because if will ignore removed segments, and this behaves differently if
  // the path is absolute. However, we only need to check the base path because
  // we are guaranteed that both paths are either relative or absolute.
  absolute = cpj_path_is_root_absolute(path_style, path_base, base_root_length);

  // We must keep track of the end of the previous segment. Initially, this is
  // set to the beginning of the path. This means that 0 is returned if the
  // first segment is not equal.
  end = path_base + base_root_length;

  // Now we loop over both segments until one of them reaches the end or their
  // contents are not equal.
  do {
    // We skip all segments which will be removed in each path, since we want to
    // know about the true path.
    if (!cpj_path_segment_joined_skip_invisible(path_style, &base, absolute) ||
        !cpj_path_segment_joined_skip_invisible(path_style, &other, absolute)) {
      break;
    }

    if (!cpj_path_is_string_equal(
          path_style, base.segment.begin, other.segment.begin,
          base.segment.size, other.segment.size
        )) {
      // So the content of those two segments are not equal. We will return the
      // size up to the beginning.
      return (cpj_size_t)(end - path_base);
    }

    // Remember the end of the previous segment before we go to the next one.
    end = base.segment.end;
  } while (cpj_path_get_next_segment_joined(path_style, &base) &&
           cpj_path_get_next_segment_joined(path_style, &other));

  // Now we calculate the length up to the last point where our paths pointed to
  // the same place.
  return (cpj_size_t)(end - path_base);
}

bool cpj_path_get_first_segment(
  cpj_path_style_t path_style, const cpj_char_t *path,
  struct cpj_segment *segment
)
{
  cpj_size_t length;
  const cpj_char_t *segments;

  // We skip the root since that's not part of the first segment. The root is
  // treated as a separate entity.
  length = cpj_path_get_root(path_style, path);
  segments = path + length;

  // Now, after we skipped the root we can continue and find the actual segment
  // content.
  return cpj_path_get_first_segment_without_root(
    path_style, path, segments, segment
  );
}

bool cpj_path_get_last_segment(
  cpj_path_style_t path_style, const cpj_char_t *path,
  struct cpj_segment *segment
)
{
  // We first grab the first segment. This might be our last segment as well,
  // but we don't know yet. There is no last segment if there is no first
  // segment, so we return false in that case.
  if (!cpj_path_get_first_segment(path_style, path, segment)) {
    return false;
  }

  // Now we find our last segment. The segment struct of the caller
  // will contain the last segment, since the function we call here will not
  // change the segment struct when it reaches the end.
  while (cpj_path_get_next_segment(path_style, segment)) {
    // We just loop until there is no other segment left.
  }

  return true;
}

bool cpj_path_get_next_segment(
  cpj_path_style_t path_style, struct cpj_segment *segment
)
{
  const cpj_char_t *c;

  // First we jump to the end of the previous segment. The first character must
  // be either a '\0' or a separator.
  c = segment->begin + segment->size;
  if (*c == '\0') {
    return false;
  }

  // Now we skip all separator until we reach something else. We are not yet
  // guaranteed to have a segment, since the string could just end afterwards.
  assert(cpj_path_is_separator(path_style, c));
  do {
    ++c;
  } while (cpj_path_is_separator(path_style, c));

  // If the string ends here, we can safely assume that there is no other
  // segment after this one.
  if (*c == '\0') {
    return false;
  }

  // Now we are safe to assume there is a segment. We store the beginning of
  // this segment in the segment struct of the caller.
  segment->begin = c;

  // And now determine the size of this segment, and store it in the struct of
  // the caller as well.
  c = cpj_path_find_next_stop(path_style, c);
  segment->end = c;
  segment->size = (cpj_size_t)(c - segment->begin);

  // Tell the caller that we found a segment.
  return true;
}

bool cpj_path_get_previous_segment(
  cpj_path_style_t path_style, struct cpj_segment *segment
)
{
  const cpj_char_t *c;

  // The current position might point to the first character of the path, which
  // means there are no previous segments available.
  c = segment->begin;
  if (c <= segment->segments) {
    return false;
  }

  // We move towards the beginning of the path until we either reached the
  // beginning or the character is no separator anymore.
  do {
    --c;
    if (c < segment->segments) {
      // So we reached the beginning here and there is no segment. So we return
      // false and don't change the segment structure submitted by the caller.
      return false;
    }
  } while (cpj_path_is_separator(path_style, c));

  // We are guaranteed now that there is another segment, since we moved before
  // the previous separator and did not reach the segment path beginning.
  segment->end = c + 1;
  segment
    ->begin = cpj_path_find_previous_stop(path_style, segment->segments, c);
  segment->size = (cpj_size_t)(segment->end - segment->begin);

  return true;
}

enum cpj_segment_type
cpj_path_get_segment_type(const struct cpj_segment *segment)
{
  // We just make a string comparison with the segment contents and return the
  // appropriate type.
  if (strncmp(segment->begin, ".", segment->size) == 0) {
    return CPJ_CURRENT;
  } else if (strncmp(segment->begin, "..", segment->size) == 0) {
    return CPJ_BACK;
  }

  return CPJ_NORMAL;
}

bool cpj_path_is_separator(cpj_path_style_t path_style, const cpj_char_t *str)
{
  const cpj_char_t *c;

  // We loop over all characters in the read symbols.
  c = separators[path_style];
  while (*c) {
    if (*c == *str) {
      return true;
    }

    ++c;
  }

  return false;
}

cpj_size_t cpj_path_change_segment(
  cpj_path_style_t path_style, struct cpj_segment *segment,
  const cpj_char_t *value, cpj_char_t *buffer, cpj_size_t buffer_size
)
{
  cpj_size_t pos, value_size, tail_size;

  // First we have to output the head, which is the whole string up to the
  // beginning of the segment. This part of the path will just stay the same.
  pos = cpj_path_output_sized(
    buffer, buffer_size, 0, segment->path,
    (cpj_size_t)(segment->begin - segment->path)
  );

  // In order to trip the submitted value, we will skip any separator at the
  // beginning of it and behave as if it was never there.
  while (cpj_path_is_separator(path_style, value)) {
    ++value;
  }

  // Now we determine the length of the value. In order to do that we first
  // locate the '\0'.
  value_size = 0;
  while (value[value_size]) {
    ++value_size;
  }

  // Since we trim separators at the beginning and in the end of the value we
  // have to subtract from the size until there are either no more characters
  // left or the last character is no separator.
  while (value_size > 0 &&
         cpj_path_is_separator(path_style, &value[value_size - 1])) {
    --value_size;
  }

  // We also have to determine the tail size, which is the part of the string
  // following the current segment. This part will not change.
  tail_size = strlen(segment->end);

  // Now we output the tail. We have to do that, because if the buffer and the
  // source are overlapping we would override the tail if the value is
  // increasing in length.
  cpj_path_output_sized(
    buffer, buffer_size, pos + value_size, segment->end, tail_size
  );

  // Finally we can output the value in the middle of the head and the tail,
  // where we have enough space to fit the whole trimmed value.
  pos += cpj_path_output_sized(buffer, buffer_size, pos, value, value_size);

  // Now we add the tail size to the current position and terminate the output -
  // basically, ensure that there is a '\0' at the end of the buffer.
  pos += tail_size;
  cpj_path_terminate_output(buffer, buffer_size, pos);

  // And now tell the caller how long the whole path would be.
  return pos;
}

cpj_path_style_t cpj_path_guess_style_test(const cpj_char_t *path)
{
  const cpj_char_t *c;
  cpj_size_t root_length;
  struct cpj_segment segment;

  // First we determine the root. Only windows roots can be longer than a single
  // slash, so if we can determine that it starts with something like "C:", we
  // know that this is a windows path.
  cpj_path_get_root_windows(path, &root_length);
  if (root_length > 1) {
    return CPJ_STYLE_WINDOWS;
  }

  // Next we check for slashes. Windows uses backslashes, while unix uses
  // forward slashes. Windows actually supports both, but our best guess is to
  // assume windows with backslashes and unix with forward slashes.
  for (c = path; *c; ++c) {
    if (*c == *separators[CPJ_STYLE_UNIX]) {
      return CPJ_STYLE_UNIX;
    } else if (*c == *separators[CPJ_STYLE_WINDOWS]) {
      return CPJ_STYLE_WINDOWS;
    }
  }

  // This path does not have any slashes. We grab the last segment (which
  // actually must be the first one), and determine whether the segment starts
  // with a dot. A dot is a hidden folder or file in the UNIX world, in that
  // case we assume the path to have UNIX style.
  if (!cpj_path_get_last_segment(CPJ_STYLE_UNIX, path, &segment)) {
    // We couldn't find any segments, so we default to a UNIX path style since
    // there is no way to make any assumptions.
    return CPJ_STYLE_UNIX;
  }

  if (*segment.begin == '.') {
    return CPJ_STYLE_UNIX;
  }

  // And finally we check whether the last segment contains a dot. If it
  // contains a dot, that might be an extension. Windows is more likely to have
  // file names with extensions, so our guess would be windows.
  for (c = segment.begin; *c; ++c) {
    if (*c == '.') {
      return CPJ_STYLE_WINDOWS;
    }
  }

  // All our checks failed, so we will return a default value which is currently
  // UNIX.
  return CPJ_STYLE_UNIX;
}
