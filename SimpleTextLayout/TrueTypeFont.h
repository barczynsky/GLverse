#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

typedef std::shared_ptr<FT_GlyphSlotRec> TrueTypeGlyph;

class TrueTypeFont
{
private:
	FT_Face font_face;
	std::unique_ptr<FT_SizeRec> font_size;
	
	std::string font_name;

private:
	std::unordered_map<wchar_t, TrueTypeGlyph> glyphs;

public:
	// TrueTypeFont(){}
	TrueTypeFont(FT_Face face, std::string name);

	std::string getFontName();

	TrueTypeGlyph getGlyphSlot(wchar_t c);
	GLuint getGlyphTexture(wchar_t c);

	FT_Pos getFontHeight();
	FT_Vector getFontKerning(wchar_t prev, wchar_t next);

};
