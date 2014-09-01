# ft-layout

## What's that?

ft-layout is a very efficient yet very simple text layout library with interface based on FreeType and demo code based on GLFW.

Current version: 0.2

## Compiling

ft-layout should compile under either Linux/Windows/Mac, given using gcc or clang (and other C++ compilers).

Recommended C++ standard used when compiling is C++14. Some minor elements of it are in use, but it still will fail in case the compiler doesn't support high enough standard extensions.

## Dependencies

- OpenGL 3.2 with support for extensions (GLEW, etc.)
- FreeType 2.5+ (compiled with LCD filtering if necessary)
- GLFW 3.0 (or even 3.1)
- stuff, that I don't remember.

## Features

- Lazy Text Rendering (render new text only when text was modified prior to the next frame)
- easy Font/Glyph metrics access (support for TrueType/OpenType)
- TrueType [kerning](http://en.wikipedia.org/wiki/Kerning) (from kern tables)
- LCD Subpixel rendering (disabled by default)
- origin drawing stabilisation
- Font Repository for caching rendered glyph bitmaps and OpenGL textures
- Saturated Addition (saturated_add) math needed for in-place glyph blending during text layout
- Texel Vector container serving as one or two dimensional buffer (random access)
- well suited for multithreaded processing (main thread for handling events, worker drawing threads, etc.)
- demo code is using [DejaVu Fonts](http://dejavu-fonts.org) now (previously using faces from [Google Fonts](https://www.google.com/fonts))

## TODO

- Hyper Text Rendering (renders glyphs separately, measuring every change)
- automatic text line breaking for set width
- selective drawing similar to OpenGL's viewport
