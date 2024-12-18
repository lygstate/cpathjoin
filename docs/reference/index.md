---
title: Reference
description: A complete reference of the cpj path library for C/C++.
---

## Basics

These are some basic, helpful functions available in the library. The basename is the last portion of the path which determines the name of the file or folder which is being pointed to. For instance, the path ``/var/log/test.txt`` would have the basename ``test.txt``. The dirname is the opposite - the path up to the basename. In that example the dirname would be ``/var/log``.

### Functions

* **[cpj_path_get_basename](cpj_path_get_basename.md)**
Gets the basename of a file path.

* **[cpj_path_change_basename](cpj_path_change_basename.md)**
Changes the basename of a file path.

* **[cpj_path_get_dirname](cpj_path_get_dirname.md)**
Gets the dirname of a file path.

* **[cpj_path_get_root](cpj_path_get_root.md)**
Determines the root of a path.

* **[cpj_path_change_root](cpj_path_change_root.md)**
Changes the root of a path.

* **[cpj_path_is_absolute](cpj_path_is_absolute.md)**
Determine whether the path is absolute or not.

* **[cpj_path_is_relative](cpj_path_is_relative.md)**
Determine whether the path is relative or not.

* **[cpj_path_join_multiple](cpj_path_join_multiple.md)**
Joins multiple paths together.

* **[cpj_path_get_intersection](cpj_path_get_intersection.md)**
Finds common portions in two paths.

## Navigation

One might specify paths containing relative components ``../``. These functions help to resolve or create relative paths based on a base path.

### Functions

* **[cpj_path_get_absolute](cpj_path_get_absolute.md)**
Generates an absolute path based on a base.

* **[cpj_path_get_relative](cpj_path_get_relative.md)**
Generates a relative path based on a base.

## Extensions

Extensions are the portion of a path which come after a `.`. For instance, the file extension of the ``/var/log/test.txt`` would be ``.txt`` - which indicates that the content is text.

### Functions

* **[cpj_path_get_extension](cpj_path_get_extension.md)**
Gets the extension of a file path.

* **[cpj_path_has_extension](cpj_path_has_extension.md)**
Determines whether the file path has an extension.

* **[cpj_path_change_extension](cpj_path_change_extension.md)**
Changes the extension of a file path.

## Style

The path style describes how paths are generated and parsed. **cpj** currently supports two path styles, ``CPJ_STYLE_WINDOWS`` and ``CPJ_STYLE_UNIX``.

### Functions

* **[cpj_path_guess_style](cpj_path_guess_style.md)**
Guesses the path style.
