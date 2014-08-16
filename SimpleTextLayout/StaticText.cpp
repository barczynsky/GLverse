#include <vector>
#include <GLFW/glfw3.h>

#include "FontRepository.h"
#include "StaticText.h"
#include "TexelVector.h"

#include "saturate_add"

using namespace std;

StaticText::StaticText(string font_name, int font_size, wstring stext) :
font { move(FontRepository::instance().getFont(font_name, font_size)) }
{
	if (!stext.empty())
		setText(move(stext));
}

wstring StaticText::getText()
{
	return text;
}

void StaticText::setText(wstring stext, int max_width)
{
	text_max_width = max_width;
	text = move(stext);
	refreshText();
}

float StaticText::getWidth()
{
	return static_cast<float>(text_width / 64.0);
}

float StaticText::getHeight()
{
	return static_cast<float>(text_height / 64.0);
}

float StaticText::getOrigin()
{
	return static_cast<float>(text_origin / 64.0);
}

void StaticText::setColor(float r, float g, float b)
{
	text_color[0] = r;
	text_color[1] = g;
	text_color[2] = b;
}

void StaticText::setOpacity(float pct)
{
	text_color[3] = pct / 100.0f;
}

void StaticText::setSpacing(float sp)
{
	text_spacing = static_cast<FT_Pos>(sp * 64.0f);
	refreshText();
}

void StaticText::refreshText()
{
	if (text.empty())
		return;

	text_origin = 0;
	text_width = 0;
	text_height = 0;

	for (auto c : text)
	{
		auto g = font.getGlyphSlot(c);
		if (g == nullptr)
			continue;

		text_origin = std::max(text_origin, g->metrics.horiBearingY);
		text_width += g->advance.x;
		text_height = std::max(text_height, text_origin - g->metrics.horiBearingY + g->metrics.height);
	}

	int border = font.getFontHeight() >> 3;
	text_width += border * 2;
	text_height += border * 2;
	// printf("W: %f(%ld), H: %f(%ld)\n", text_width / 64.0f, text_width, text_height / 64.0f, text_height);

	TexelVector buffer(text_width >> 6, text_height >> 6, { 255, 255, 255, 0 });
	wchar_t prev_c = 0;
	FT_Pos cursor = 0;

	for (auto c : text)
	{
		auto g = font.getGlyphSlot(c);
		if (g == nullptr)
			continue;

		auto kerning = font.getFontKerning(prev_c, c);
		cursor += kerning.x;
		// if (kerning.x) printf("kerning of %lc and %lc is (%ld,%ld)\n", prev_c, c, kerning.x, kerning.y);

		int xoff = (cursor + g->metrics.horiBearingX + border) >> 6;
		int yoff = (text_origin - g->metrics.horiBearingY + border) >> 6;
		for (int y = 0; y < g->bitmap.rows; y++)
		{
			for (int x = 0; x < g->bitmap.width; x++)
			{
				auto & texel = buffer.at(xoff + x, yoff + y);
				texel.a = saturate_add(texel.a, g->bitmap.buffer[g->bitmap.width * y + x]);
			}
		}
		prev_c = c;
		cursor += g->advance.x + text_spacing;
	}

	glDeleteTextures(1, &font_texture);
	glGenTextures(1, &font_texture);
	glBindTexture(GL_TEXTURE_2D, font_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, text_width >> 6, text_height >> 6, 0, GL_BGRA, GL_UNSIGNED_BYTE, (uint8_t*)buffer.data());
}

void StaticText::drawText(int x, int y)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int w = text_width >> 6;
	int h = text_height >> 6;
	y += (font.getFontHeight() - text_origin) >> 6; //stabilize rendered text origin
	glBindTexture(GL_TEXTURE_2D, font_texture);
	glColor4fv(text_color);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(x, y);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(x + w, y);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(x + w, y + h);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(x, y + h);
	glEnd();
}
