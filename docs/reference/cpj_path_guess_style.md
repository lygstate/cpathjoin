---
title: cpj_path_guess_style
description: Guesses the path style.
---

_(since v1.2.0)_
Guesses the path style.

## Description

```c
cpj_path_style_t cpj_path_guess_style(const char *path);
```

This function guesses the path style based on a submitted path-string. The guessing will look at the root and the type of slashes contained in the path and return the style which is more likely used in the path.
The algorithm checks the following:

 1. If the root is longer than 1 character -> ``CPJ_STYLE_WINDOWS``
 1. If the first separator is a backslash -> ``CPJ_STYLE_WINDOWS``
 1. If the first separator is a slash -> ``CPJ_STYLE_UNIX``
 1. If the last segment starts with a dot -> ``CPJ_STYLE_UNIX``
 1. If the last segment contains a dot -> ``CPJ_STYLE_WINDOWS``
 1. If nothing was found to determine the style -> ``CPJ_STYLE_UNIX``

## Parameters

* **path**: The path which will be inspected.

## Return Value

Returns the style which is most likely used for the path.

## Outcomes

| Path                        | Result                |
|-----------------------------|-----------------------|
| ``C:\\test``                | ``CPJ_STYLE_WINDOWS`` |
| ``C:/test``                 | ``CPJ_STYLE_WINDOWS`` |
| ``C:test``                  | ``CPJ_STYLE_WINDOWS`` |
| ``C:/.test``                | ``CPJ_STYLE_WINDOWS`` |
| ``C:/folder/.test``         | ``CPJ_STYLE_WINDOWS`` |
| ``\directory\other``        | ``CPJ_STYLE_WINDOWS`` |
| ``\directory\.other``       | ``CPJ_STYLE_WINDOWS`` |
| ``myfile.txt``              | ``CPJ_STYLE_WINDOWS`` |
| ``/directory``              | ``CPJ_STYLE_UNIX``    |
| ``/directory/other``        | ``CPJ_STYLE_UNIX``    |
| ``/directory/other.txt``    | ``CPJ_STYLE_UNIX``    |
| ``.my_hidden_file``         | ``CPJ_STYLE_UNIX``    |
| ``.my_hidden_file.txt``     | ``CPJ_STYLE_UNIX``    |
| ``/a/directory/myfile.txt`` | ``CPJ_STYLE_UNIX``    |
| ``myfile``                  | ``CPJ_STYLE_UNIX``    |

## Example

```c
#include <cpj.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  if (cpj_path_guess_style("C:\\myfile") == CPJ_STYLE_UNIX) {
    printf("UNIX.");
  } else {
    printf("WINDOWS.");
  }

  return EXIT_SUCCESS;
}
```

Ouput:

```txt
WINDOWS.
```

## Changelog

| Version    | Description                                            |
|------------|--------------------------------------------------------|
| **v1.2.1** | Fixed crash on call with empty string.                 |
| **v1.2.0** | The function is introduced.                            |
