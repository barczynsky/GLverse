#pragma once
#include <cmath>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include "saturate_add"

#include "BaseText.h"
#include "FontRepository.h"
#include "TexelVector.h"
#include "TrueTypeFont.h"

template<typename string_t = std::string>
class BaseText
{
public:
	typedef string_t StringType;

private:
	StringType text;
	std::shared_ptr<TrueTypeFont> font;
	struct GLTexture {
		GLuint tex_id{ 0 };
		int tex_w{ 0 };
		int tex_h{ 0 };
	} texture;
	union RGBA_glColor4fv {
		struct {
			float r,g,b,a;
		};
		GLfloat color4fv[4]{ 1.0f, 1.0f, 1.0f, 1.0f };
		operator GLfloat*() { return color4fv; }
	} text_color;

	FT_Pos text_border{ 0 };
	FT_Vector text_origin{ 0, 0 };
	FT_Pos text_baseline{ 0 };
	FT_Pos text_width{ 0 };
	FT_Pos text_height{ 0 };
	// FT_Vector text_advance{ 0, 0 };

	FT_Pos text_size{ 0 };
	FT_Pos text_spacing{ 0 };

	// int line_count{ 1 };
	// int wrap_count{ 0 };

public:
	// BaseText(){}
	BaseText(std::string font_name, int font_size) :
	font { std::move(FontRepository::instance().getFont(font_name, font_size)) }
	{
	}

public:
	virtual StringType getText()
	{
		return text;
	}

	virtual float getBaseline()
	{
		return static_cast<float>(text_baseline / 64.0);
	}

	virtual float getWidth()
	{
		return static_cast<float>(text_width / 64.0);
	}

	virtual float getHeight()
	{
		return static_cast<float>(text_height / 64.0);
	}

	virtual float getSize()
	{
		return static_cast<float>(text_size / 64.0);
	}

	virtual float getSpacing()
	{
		return static_cast<float>(text_spacing / 64.0);
	}

	// int getLineCount() { return line_count; }
	// int getWrapCount() { return wrap_count; }

public:
	virtual void setText(StringType new_text)
	{
		text = std::move(new_text);
	}

	virtual void setSize(int new_size)
	{
		font = std::move(FontRepository::instance().getFont(font->getFontName(), new_size));
	}

	virtual void setSpacing(float sp = 0.0f)
	{
		text_spacing = static_cast<FT_Pos>(std::floor(sp * 64.0));
	}

	void setOpacity(float pct = 100.0f)
	{
		text_color.a = pct / 100.0f;
	}

	void setColor(float r, float g, float b)
	{
		text_color.r = r;
		text_color.g = g;
		text_color.b = b;
	}

	template<typename ColorType>
	void setColor(ColorType color)
	{
		text_color.r = color.r;
		text_color.g = color.g;
		text_color.b = color.b;
	}

public:
	virtual void makeText()
	{
		if (font == nullptr)
			return;
		if (text.empty())
			return;

		text_baseline = 0;
		text_width = 0;
		text_height = 0;
		text_size = font->getFontHeight();

		for (auto c : text)
		{
			auto g = font->getGlyphSlot(c);
			if (g == nullptr)
				continue;

			text_baseline = std::max(text_baseline, g->metrics.horiBearingY);
			text_width += g->advance.x + text_spacing;
			text_height = std::max(text_height, text_baseline - g->metrics.horiBearingY + g->metrics.height);
		}

		text_border = (text_size >> 2) >> 6 << 6;
		texture.tex_w = (text_width + text_border * 4) >> 6;
		texture.tex_h = (text_height + text_border * 2) >> 6;
		texture.tex_w = 2 << (int)std::log2(texture.tex_w);
		texture.tex_h = 2 << (int)std::log2(texture.tex_h);
		text_origin.x = 0 - ((text_border) >> 6 << 6);
		text_origin.y = 0 - ((text_border + text_baseline) >> 6 << 6);
		// fprintf(stderr, "Text: '%s',", text.c_str());
		// fprintf(stderr, "W: %d(%ld), H: %d(%ld)\n", texture.tex_w, text_width, texture.tex_h, text_height);
		// fprintf(stderr, "OrigX: %ld(%ld), OrigY: %ld(%ld)\n", text_origin.x >> 6, text_origin.x, text_origin.y >> 6, text_origin.y);

		TexelVector buffer(texture.tex_w, texture.tex_w, { 0, 0, 0, 0 });
		FT_Pos cursor = 0 - (font->getGlyphSlot(text.front())->metrics.horiBearingX >> 6);
		wchar_t prev_c = 0;
		for (auto c : text)
		{
			auto g = font->getGlyphSlot(c);
			if (g == nullptr)
				continue;

			auto kerning = font->getFontKerning(prev_c, c);
			cursor += kerning.x;
			// if (kerning.x) fprintf(stderr, "kerning for '%lc' after '%lc' is (%ld,%ld)\n", prev_c, c, kerning.x, kerning.y);
#ifdef TARGET_LCD
			int bitmap_width = g->bitmap.width / 3;
#else
			int bitmap_width = g->bitmap.width;
#endif
			int xoff = (cursor + g->metrics.horiBearingX + text_border) >> 6;
			int yoff = (text_baseline - g->metrics.horiBearingY + text_border) >> 6;
			for (int y = 0; y < g->bitmap.rows; y++)
			{
				for (int x = 0; x < bitmap_width; x++)
				{
					auto & texel = buffer.at(xoff + x, yoff + y);
#ifdef TARGET_LCD
					const int idx = g->bitmap.pitch * y + x * 3;
					texel.r = saturate_add(texel.r, g->bitmap.buffer[idx + 0]);
					texel.g = saturate_add(texel.g, g->bitmap.buffer[idx + 1]);
					texel.b = saturate_add(texel.b, g->bitmap.buffer[idx + 2]);
					texel.a = std::max(texel.r, std::max(texel.g, texel.b));
#else
					texel.r = 255;
					texel.g = 255;
					texel.b = 255;
					texel.a = saturate_add(texel.a, g->bitmap.buffer[g->bitmap.pitch * y + x]);
#endif
#ifdef ENABLE_SHADOWS
					texel.a = saturate_add(texel.a, ENABLE_SHADOWS);
#endif
					// fprintf(stderr, "BGRATexel(%d,%d,%d,%d)\n", texel.r, texel.g, texel.b, texel.a);
				}
			}

			cursor += g->advance.x + text_spacing;
			prev_c = c;
		}

		glDeleteTextures(1, &texture.tex_id);
		glGenTextures(1, &texture.tex_id);
		glBindTexture(GL_TEXTURE_2D, texture.tex_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture.tex_w, texture.tex_h, 0, GL_BGRA, GL_UNSIGNED_BYTE, (uint8_t*)buffer.data());
	}

	void drawAll(int x, int y)
	{
		this->drawText(x, y);
		this->drawBounds(x, y);
		this->drawBaseline(x, y);
		this->drawOrigin(x, y);
	}

	virtual void drawText(int x, int y)
	{
		int w = texture.tex_w;
		int h = texture.tex_h;
		x += text_origin.x >> 6;
		y += text_origin.y >> 6;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture.tex_id);
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
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}

	void drawBounds(int x, int y)
	{
		int w = texture.tex_w;
		int h = texture.tex_h;
		x += text_origin.x >> 6;
		y += text_origin.y >> 6;

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glLineWidth(1);
		glBegin(GL_LINE_LOOP);
			glVertex2f(x, y);
			glVertex2f(x + w, y);
			glVertex2f(x + w, y + h);
			glVertex2f(x, y + h);
		glEnd();
	}

	void drawBaseline(int x, int y)
	{
		int w = text_width >> 6;

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glLineWidth(1);
		glBegin(GL_LINES);
			glVertex2f(x, y);
			glVertex2f(x + w, y);
		glEnd();
	}

	void drawOrigin(int x, int y)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glLineWidth(1);
		glBegin(GL_LINES);
			glVertex2f(x - 10, y);
			glVertex2f(x + 10, y);
			glVertex2f(x, y - 10);
			glVertex2f(x, y + 10);
		glEnd();
	}

};
