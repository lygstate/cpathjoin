---
title: C/C++ Path Library - Cross-Platform
description: cpj is a lightweight C/C++ Path Library which focuses on file path manipulations. Using cpj you can easily do things like combine paths, resolve relative paths, create relative paths based on absolute paths, canonicalize paths or read extension information from paths.
---

## What is cpj?

**cpj** is a lightweight C/C++ library which focuses on file path manipulations. Using **cpj** you can easily do things like combine paths, resolve relative paths, create relative paths based on absolute paths, canonicalize paths or read extension information from paths.

## Example

Here is a simple example of one thing (normalization) cpj can do for you. However, there are more features available.

```c
#include <cpj.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  char result[FILENAME_MAX];

  // The following function cleans up the input path and writes it
  // to the result buffer.
  cpj_path_normalize( "/var/log/weird/////path/.././..///", result,
    sizeof(result));

  printf("%s\n", result);
  return EXIT_SUCCESS;
}
```

Output:

```txt
/var/log
```
