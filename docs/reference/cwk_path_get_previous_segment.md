---
title: cpj_path_get_previous_segment
description: Moves to the previous segment.
---

_(since v1.0.0)_  
Moves to the previous segment.

## Description
```c
bool cpj_path_get_previous_segment(struct cpj_segment *segment);
```

This function moves the current segment to the previous segment. If the current segment is the first one, the submitted segment structure will stay unchanged and false is returned. The segment should be either initialized using **[cpj_path_get_first_segment]({{ site.baseurl }}{% link reference/cpj_path_get_first_segment.md %})** or **[cpj_path_get_last_segment]({{ site.baseurl }}{% link reference/cpj_path_get_last_segment.md %})**.

## Parameters
 * **segment**: The current segment which will be moved to the previous one.

## Return Value
Returns ``true`` if there is a segment before this one or ``false`` otherwise.

## Example
```c
#include <cpj.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  struct cpj_segment segment;

  if(!cpj_path_get_last_segment("/my/funny/test/path.txt", &segment)) {
    printf("Path doesn't have any segments.\n");
    return EXIT_FAILURE;
  }
  
  do {
    printf("Current segment is '%.*s'.\n", (int)segment.size, segment.begin);
  } while(cpj_path_get_previous_segment(&segment));
  
  return EXIT_SUCCESS;
}
```

Ouput:
```
Current segment is 'path.txt'.
Current segment is 'test'.
Current segment is 'funny'.
Current segment is 'my'.
```

## Changelog

| Version    | Description                                            |
|------------|--------------------------------------------------------|
| **v1.2.4** | Bugfix for single char segments.                       |
| **v1.0.0** | The function is introduced.                            |
