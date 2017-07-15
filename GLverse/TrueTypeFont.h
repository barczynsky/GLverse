#pragma once
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include "OpenGL.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
// #define GLYPH_SHADOWS 31


struct GLtexture
{
	GLuint tex_id{ 0 };
	int tex_w{ 0 };
	int tex_h{ 0 };
};
// struct GlyphSlotRecEx
// {
// 	GLtexture texture;
// };
// typedef std::shared_ptr<GlyphSlotRecEx> TrueTypeGlyphEx;
typedef std::shared_ptr<FT_GlyphSlotRec> TrueTypeGlyph;


class TrueTypeFont
{
private:
	std::mutex font_mutex;

private:
	std::string font_name;

private:
	FT_Face font_face;
	std::unique_ptr<FT_SizeRec> font_size;

private:
	std::unordered_map<char32_t, TrueTypeGlyph> glyphs;
	//std::unordered_map<char32_t, TrueTypeGlyphEx> glyphs_ex;

public:
	// TrueTypeFont(){}
	TrueTypeFont(FT_Face face, std::string name);
	TrueTypeFont(const TrueTypeFont& other) = delete;
	TrueTypeFont(TrueTypeFont&& other) = delete;

public:
	TrueTypeGlyph getGlyphSlot(char32_t c);
	//TrueTypeGlyphEx getGlyphSlotEx(char32_t c);
	//GLuint getGlyphTexture(char32_t c);
	FT_Outline* getGlyphOutline(char32_t c);

public:
	std::string getFontName();
	FT_Pos getFontHeight();
	FT_Pos getXHeight();

	FT_Vector getFontKerning(char32_t prev, char32_t next);

};
