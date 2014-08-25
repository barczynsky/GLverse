ft-layout
=========

Yet another efficient text layout interface based on FreeType/GLFW

Features
=========

- Static Text Rendering - rather few changes per second, draw previously generated texture
- Lazy Text Rendering - render new texture only when text has been modified before drawing
- basic font/glyph metrics support for TrueType/OpenType
- TrueType kerning with drawing stabilisation and subpixel rendering
- Font Repository for caching rendered glyph bitmaps and OpenGL textures (w/o subpixel)
- saturated addition math needed for in-place glyph bitmap blending (in memory)
- Texel Vector as 2D texture buffer with random access in two dimensions
- suited for multithreaded processing - main thread for handling events, worker drawing threads
- using Google Fonts provided in-source now, but pretty much can use any TTF/OTF
