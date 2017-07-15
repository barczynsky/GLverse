#include "TrueTypeFont.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "TexelVector.h"


TrueTypeFont::TrueTypeFont(FT_Face face, std::string name)
{
	font_face = face;
	font_size = std::make_unique<FT_SizeRec>();
	font_size->metrics = face->size->metrics;
	font_name = name;
}

TrueTypeGlyph TrueTypeFont::getGlyphSlot(char32_t c)
{
	std::lock_guard<std::mutex> lck(font_mutex);
	if (glyphs.count(c))
	{
		return glyphs[c];
	}

	if (FT_Load_Char(font_face, c, FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT))
	{
		return nullptr;
	}

	auto g = font_face->glyph;
	glyphs[c] = std::make_shared<FT_GlyphSlotRec>();
	glyphs[c]->metrics = g->metrics;
	glyphs[c]->advance = g->advance;
	glyphs[c]->bitmap = g->bitmap;
	glyphs[c]->bitmap_left = g->bitmap_left;
	glyphs[c]->bitmap_top = g->bitmap_top;
	glyphs[c]->outline = g->outline;
	glyphs[c]->lsb_delta = g->lsb_delta;
	glyphs[c]->rsb_delta = g->rsb_delta;

	//deepcopy(glyphs[c]->bitmap)
	size_t bitmap_buffer_size = g->bitmap.rows * std::abs(g->bitmap.pitch);
	glyphs[c]->bitmap.buffer = new uint8_t[bitmap_buffer_size];
	std::memcpy(glyphs[c]->bitmap.buffer, g->bitmap.buffer, bitmap_buffer_size);

	//deepcopy(glyphs[c]->outline)
	size_t outline_points_size = g->outline.n_points;
	glyphs[c]->outline.points = new FT_Vector[outline_points_size];
	std::memcpy(glyphs[c]->outline.points, g->outline.points, outline_points_size * sizeof(FT_Vector));
	size_t outline_tags_size = g->outline.n_points;
	glyphs[c]->outline.tags = new char[outline_tags_size];
	std::memcpy(glyphs[c]->outline.tags, g->outline.tags, outline_tags_size * sizeof(char));
	size_t outline_contours_size = g->outline.n_contours;
	glyphs[c]->outline.contours = new short[outline_contours_size];
	std::memcpy(glyphs[c]->outline.contours, g->outline.contours, outline_contours_size * sizeof(short));

	// int tex_w = g->bitmap.width;
	// int tex_h = g->bitmap.rows;

	// TexelVector buffer(tex_w, tex_h, { 255, 255, 255, 255 });
	// for (size_t i = 0; i < buffer.size(); i++)
	// {
	// 	auto&& texel = buffer.at(i);
	// 	texel.a = g->bitmap.buffer[i];
	// }

	// GLuint tex_id;
	// glDeleteTextures(1, &tex_id);
	// glGenTextures(1, &tex_id);
	// glBindTexture(GL_TEXTURE_2D, tex_id);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_w, tex_h, 0, GL_BGRA, GL_UNSIGNED_BYTE, (uint8_t*)buffer.data());

	// glyphs_ex[c] = std::make_shared<GlyphSlotRecEx>();
	// glyphs_ex[c]->texture.tex_id = tex_id;
	// glyphs_ex[c]->texture.tex_w = tex_w;
	// glyphs_ex[c]->texture.tex_h = tex_h;

	return glyphs[c];
}

// TrueTypeGlyphEx TrueTypeFont::getGlyphSlotEx(char32_t c)
// {
// 	std::lock_guard<std::mutex> lck(font_mutex);
// 	if (glyphs.count(c))
// 	{
// 		return glyphs_ex[c];
// 	}
// 	return nullptr;
// }

// GLuint TrueTypeFont::getGlyphTexture(char32_t c)
// {
// 	return getGlyphSlotEx(c)->texture.tex_id;
// }

FT_Outline* TrueTypeFont::getGlyphOutline(char32_t c)
{
	return &getGlyphSlot(c)->outline;
}

std::string TrueTypeFont::getFontName()
{
	std::lock_guard<std::mutex> lck(font_mutex);
	return font_name;
}

FT_Pos TrueTypeFont::getFontHeight()
{
	std::lock_guard<std::mutex> lck(font_mutex);
	return font_size->metrics.height;
}

FT_Pos TrueTypeFont::getXHeight()
{
	return getGlyphSlot('x')->metrics.height;
}

FT_Vector TrueTypeFont::getFontKerning(char32_t left, char32_t right)
{
	std::lock_guard<std::mutex> lck(font_mutex);
	auto prev = FT_Get_Char_Index(font_face, left);
	auto next = FT_Get_Char_Index(font_face, right);

	FT_Vector kerning;
	FT_Get_Kerning(font_face, prev, next, FT_KERNING_DEFAULT, &kerning);

	return kerning;
}
