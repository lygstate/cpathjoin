---
title: cpj_path_get_segment_type
description: Gets the type of the submitted path segment.
---

_(since v1.0.0)_  
Gets the type of the submitted path segment.

## Description
```c
enum cpj_segment_type cpj_path_get_segment_type(const struct cpj_segment *segment);
```

This function inspects the contents of the segment and determines the type of it. Currently, there are three types ``CPJ_NORMAL``, ``CPJ_CURRENT`` and ``CPJ_BACK``. A ``CPJ_NORMAL`` segment is a normal folder or file entry. A ``CPJ_CURRENT`` is a "./" and a ``CPJ_BACK`` a "../" segment.

## Parameters
 * **segment**: The segment which will be inspected.

## Return Value
Returns the type of the segment.

## Example
```c
#include <cpj.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  struct cpj_segment segment;

  if(!cpj_path_get_first_segment("/../funny/test/path.txt", &segment)) {
    printf("Path doesn't have any segments.");
    return EXIT_FAILURE;
  }
  
  if(cpj_path_get_segment_type(&segment) == CPJ_BACK) {
    printf("The segment type is CPJ_BACK.");
  }
  
  return EXIT_SUCCESS;
}
```

Ouput:
```
The segment type is CPJ_BACK.
```

## Changelog

| Version    | Description                                            |
|------------|--------------------------------------------------------|
| **v1.0.0** | The function is introduced.                            |