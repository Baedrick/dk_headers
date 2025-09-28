# dk_headers
Various single-file libraries for C and C++, similar to [stb libraries](https://github.com/nothings/stb).

| Library         | Version | Language | Description                                                  |
| --------------- | ------- | -------- | ------------------------------------------------------------ |
| dk_flat_map.hpp | 0.21     | C++      | A template associative ordered container using a sorted vector. Similar interface to `std::map`. |
| dk_pcg32.h      | 0.1     | C/C++    | PCG32 random number generator with added common functions used in real-time applications. |

These libraries are as-is, however, suggestions for improvements or bug fixes are appreciated. Please raise an issue before submitting a PR. Bug fixes are welcomed!

## Motivation

These headers are derived from projects when I find myself saying "I hate implementing this again". They are built to solve specific problems I faced in my own projects.

## FAQ

> How do I use these libraries?

Generally, these headers do not have dependencies and are to be directly included in your source code. Include them like any other header, however, you must select exactly ONE C/C++ source file that actually instantiates the code, preferably a file you're not editing frequently. It would look something like this:

```cpp
// Do ONCE in a c/c++ file.
#define DK_LIBRARY_IMPLEMENTATION
#include "dk_library.h"

// Include like a typical header everywhere else.
#include "dk_library.h"
```

Check each header for specific documentation at the top of the file for what the macro should be, if required.

> What is the license?

Each library contains license information at the end of the file. It is generally a choice between MIT or zlib, or in the case of derived work, the original license.

> Why have the declaration and definition in one file? Why not ship a .h/.hpp with a .c/.cpp?

Shipping a single header file is easier to distribute and integrate because all of the code is in one file. Additionally, build scripts are a pain to work with and usually have varied/insane steps to build and include to your project, from "just glob the sources and add the include path" to "you have to correctly parameterize this 2000 line build script written in X" (u/kalmoc on reddit). These single header libraries can be added to any project without modifying a single build script.

If you strongly prefer to split the implementation into a header and a source file, you are free to do so! All declarations are at the top of the file BEFORE the `#ifdef DK_LIBRARY_IMPLEMENTATION`, so copy those into a header file. Anything after can go into a source file. It would look like this.

```cpp
// Single header library: dk_foo.h
void foo();
#ifdef DK_LIBRARY_IMPLEMENTATION
void foo() {
  printf("fooooo!!");
}

// Header: dk_foo.h
void foo();

// Source: dk_foo.c
void foo() {
  printf("fooooo!!");
}
```

> Are your libraries "sane"?

I try to keep the interface and implementation sane, based off the uses I needed in my projects. Unfortunately, writing a good header library is hard.

