#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H
#define TARGET_LCD
// #define ENABLE_SHADOWS 31

struct GlyphSlotRecEx
{
	struct GLTexture {
		GLuint tex_id{ 0 };
		int tex_w{ 0 };
		int tex_h{ 0 };
	} texture;
};
typedef std::shared_ptr<GlyphSlotRecEx> TrueTypeGlyphEx;
typedef std::shared_ptr<FT_GlyphSlotRec> TrueTypeGlyph;

class TrueTypeFont
{
private:
	FT_Face font_face;
	std::unique_ptr<FT_SizeRec> font_size;
	
	std::string font_name;

private:
	std::unordered_map<wchar_t, TrueTypeGlyph> glyphs;
	std::unordered_map<wchar_t, TrueTypeGlyphEx> glyphs_ex;

public:
	// TrueTypeFont(){}
	TrueTypeFont(FT_Face face, std::string name);

	TrueTypeGlyph getGlyphSlot(wchar_t c);
	TrueTypeGlyphEx getGlyphSlotEx(wchar_t c);
	GLuint getGlyphTexture(wchar_t c);

	std::string getFontName();
	FT_Pos getFontHeight();
	FT_Vector getFontKerning(wchar_t prev, wchar_t next);

};
