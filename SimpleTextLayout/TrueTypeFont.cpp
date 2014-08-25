#include "TrueTypeFont.h"
#include "TexelVector.h"
#include "saturate_add"

using namespace std;

TrueTypeFont::TrueTypeFont(FT_Face face, string name)
{
	font_face = face;
	font_size = make_unique<FT_SizeRec>();
	font_size->metrics = face->size->metrics;
	font_name = name;
}

TrueTypeGlyph TrueTypeFont::getGlyphSlot(wchar_t c)
{
	if (glyphs.count(c))
	{
		return glyphs[c];
	}

#ifdef TARGET_LCD
	if (FT_Load_Char(font_face, c, FT_LOAD_NO_BITMAP | FT_LOAD_RENDER | FT_LOAD_TARGET_LCD))
		return nullptr;
#else
	if (FT_Load_Char(font_face, c, FT_LOAD_NO_BITMAP | FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT))
		return nullptr;
#endif

	auto g = font_face->glyph;
	glyphs[c] = make_shared<FT_GlyphSlotRec>();
	glyphs[c]->metrics = g->metrics;
	glyphs[c]->advance = g->advance;
	glyphs[c]->bitmap = g->bitmap;
	glyphs[c]->bitmap_left = g->bitmap_left;
	glyphs[c]->bitmap_top = g->bitmap_top;
	glyphs[c]->lsb_delta = g->lsb_delta;
	glyphs[c]->rsb_delta = g->rsb_delta;

	int tex_w = g->bitmap.width;
	int tex_h = g->bitmap.rows;
	size_t buffer_size = g->bitmap.rows * std::abs(g->bitmap.pitch);
	glyphs[c]->bitmap.buffer = new uint8_t[buffer_size];
	memcpy(glyphs[c]->bitmap.buffer, g->bitmap.buffer, buffer_size);
	TexelVector buffer(tex_w, tex_h, { 255, 255, 255, 255 });
#ifdef TARGET_LCD
	// TODO: solve this somehow
#else
	for (size_t i = 0; i < buffer.size(); i++)
	{
		auto & texel = buffer.at(i);
		texel.a = g->bitmap.buffer[i];
	}
#endif

	GLuint tex_id;
	glDeleteTextures(1, &tex_id);
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_w, tex_h, 0, GL_BGRA, GL_UNSIGNED_BYTE, (uint8_t*)buffer.data());

	glyphs_ex[c] = make_shared<GlyphSlotRecEx>();
	glyphs_ex[c]->texture.tex_id = tex_id;
	glyphs_ex[c]->texture.tex_w = tex_w;
	glyphs_ex[c]->texture.tex_h = tex_h;

	return glyphs[c];
}

TrueTypeGlyphEx TrueTypeFont::getGlyphSlotEx(wchar_t c)
{
	if (glyphs.count(c))
	{
		return glyphs_ex[c];
	}
	return nullptr;
}

GLuint TrueTypeFont::getGlyphTexture(wchar_t c)
{
	return getGlyphSlotEx(c)->texture.tex_id;
}

string TrueTypeFont::getFontName()
{
	return font_name;
}

FT_Pos TrueTypeFont::getFontHeight()
{
	return font_size->metrics.height;
}

FT_Vector TrueTypeFont::getFontKerning(wchar_t left, wchar_t right)
{
	auto prev = FT_Get_Char_Index(font_face, left);
	auto next = FT_Get_Char_Index(font_face, right);

	FT_Vector kerning;
	FT_Get_Kerning(font_face, prev, next, FT_KERNING_DEFAULT, &kerning);

	return kerning;
}
