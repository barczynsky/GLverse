# GLverse

## What's that?

GLverse is small and simple, but also aims to be an efficient **raster** text (font) rendering library with backend based on FreeType/OpenGL and a demo app based on GLFW.

Current version: 0.5

## Compiling

GLverse should compile (**compile**, not necessarily **link**) well on either Linux/Windows/macOS, preferably using LLVM Clang. Minimum supported language version is C++14 due to various handy constructs being used (make_unique, decltype(auto), generic and improved lambdas, etc.).

NOTE: You may encounter linkage problems on platforms other than Linux (Debian/Arch).

Usual compilation procedure on Linux using **ninja**:
```bash
$ git clone https://github.com/barczynsky/GLverse.git
$ cd GLverse
$ mkdir build
$ cd build
$ cmake -GNinja ..
$ ninja
$ ./demo.GLverse
```

With this you will build a static GLverse library and a simple visual demo, usually presenting recently added features.

## Dependencies

- CMake 3.1 (build only)
- OpenGL 3.3 (demo still uses compatibility context)
- GLEW 2.0 (will be replaced by gl3w, after core context transition)
- FreeType 2.6
- GLFW 3.2
- X11 (Linux only)
- stuff, that I don't remember.

## Features

- Lazy Text Rendering (create new texture only when the text was modified)
- Font [kerning](http://en.wikipedia.org/wiki/Kerning) (from kern tables)
- Font and glyph metrics (for TrueType and OpenType faces)
- Saturated addition math (saturate_add) needed for in-place glyph bitmap blending
- Text layout control, such as text wrap or alignment
- Texel container serving as either one or two dimensional texture buffer
- Font repository, also used for caching rendered glyphs
- Ready for multithreaded pipeline by extensive use of mutexes
- Demo code is now using [Noto Fonts](https://www.google.com/get/noto)

## TODO

- Restrict texture drawing area, probably by using OpenGL scissor test
