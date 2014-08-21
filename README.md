ft-layout
=========

Yet another efficient text layout interface based on FreeType/GLFW

Features
=========

- static text rendering - rather few changes per second, draw last generated texture
- lazy text rendering - render new texture only when text has been modified before draw
- basic font/glyph metrics support for TrueType/OpenType
- simple kerning with texture drawing stabilisation
- font repository for caching rendered glyph bitmaps and OpenGL textures
- saturated addition needed for memory in-place glyph bitmap blending
- texel vector as texture buffer with random access of 1 or 2 dimensions
- multithreaded processing - main thread for handling events, worker threads for drawing/math
- can use Google Fonts provided in-source, but pretty much can use any TTF/OTF
