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

shared_ptr<FT_GlyphSlotRec> TrueTypeFont::getGlyphSlot(wchar_t c)
{
	if (glyphs.count(c))
	{
		return glyphs[c];
	}

	if (FT_Load_Char(font_face, c, FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT))
		return nullptr;

	auto g = font_face->glyph;
	glyphs[c] = make_shared<FT_GlyphSlotRec>();
	glyphs[c]->metrics = g->metrics;
	glyphs[c]->advance = g->advance;
	glyphs[c]->bitmap = g->bitmap;
	glyphs[c]->bitmap_left = g->bitmap_left;
	glyphs[c]->bitmap_top = g->bitmap_top;
	glyphs[c]->lsb_delta = g->lsb_delta;
	glyphs[c]->rsb_delta = g->rsb_delta;

	size_t buffer_length = g->bitmap.width * g->bitmap.rows;
	glyphs[c]->bitmap.buffer = new uint8_t[buffer_length];
	memcpy(glyphs[c]->bitmap.buffer, g->bitmap.buffer, buffer_length);

	TexelVector buffer(g->bitmap.width, g->bitmap.rows, { 255, 255, 255, 0 });
	for (size_t i = 0; i < buffer.size(); i++)
	{
		auto & texel = buffer.at(i);
		texel.a = saturate_add(texel.a, g->bitmap.buffer[i]);
	}

	GLuint tex;
	glDeleteTextures(1, &tex);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, glyphs[c]->bitmap.width, glyphs[c]->bitmap.rows, 0, GL_BGRA, GL_UNSIGNED_BYTE, (uint8_t*)buffer.data());
	glyphs[c]->generic.data = (void*)tex;

	return glyphs[c];
}

GLuint TrueTypeFont::getGlyphTexture(wchar_t c)
{
	return (GLuint)getGlyphSlot(c)->generic.data;
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
