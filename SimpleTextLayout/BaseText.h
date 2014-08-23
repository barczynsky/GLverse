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
	struct FontTexture {
		GLuint id{ 0 };
		FT_Pos b{ 0 };
		int w{ 0 };
		int h{ 0 };
		int xoff{ 0 };
		int yoff{ 0 };
	} tex;

	FT_Pos text_origin{ 0 };
	FT_Pos text_width{ 0 };
	FT_Pos text_height{ 0 };
	FT_Pos text_size{ 0 };
	FT_Pos text_spacing{ 0 };

	GLfloat text_color[4]{ 1.0f, 1.0f, 1.0f, 1.0f };

	// int line_count{ 1 };
	// int wrap_count{ 0 };

public:
	// BaseText(){}
	BaseText(std::string font_name, int font_size)
	: font { std::move(FontRepository::instance().getFont(font_name, font_size)) }
	{
	}

public:
	virtual StringType getText()
	{
		return text;
	}

	virtual float getOrigin()
	{
		return static_cast<float>(text_origin / 64.0);
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
		text_color[3] = pct / 100.0f;
	}

	void setColor(float r, float g, float b)
	{
		text_color[0] = r;
		text_color[1] = g;
		text_color[2] = b;
	}

	template<typename ColorType>
	void setColor(ColorType color)
	{
		text_color[0] = color.r;
		text_color[1] = color.g;
		text_color[2] = color.b;
	}

public:
	virtual void makeText()
	{
		if (font == nullptr)
			return;
		if (text.empty())
			return;

		text_origin = 0;
		text_width = 0;
		text_height = 0;
		text_size = font->getFontHeight();

		for (auto c : text)
		{
			auto g = font->getGlyphSlot(c);
			if (g == nullptr)
				continue;

			text_origin = std::max(text_origin, g->metrics.horiBearingY);
			text_width += g->advance.x;
			text_height = std::max(text_height, text_origin - g->metrics.horiBearingY + g->metrics.height);
		}

		tex.b = text_size >> 3;
		tex.w = (text_width + tex.b * 4) >> 6;
		tex.h = (text_height + tex.b * 2) >> 6;
		tex.xoff = 0 - (tex.b >> 6);
		tex.yoff = 0 - ((text_origin + tex.b) >> 6);
		// fprintf(stderr, "Text: '%s', ", text.c_str());
		// fprintf(stderr, "W: %d(%ld), H: %d(%ld)\n", tex.w, text_width, tex.h, text_height);

		TexelVector buffer(tex.w, tex.h, { 255, 255, 255, 0 });
		FT_Pos cursor = 0 - (font->getGlyphSlot(text[0])->metrics.horiBearingX >> 6); //TODO
		wchar_t prev_c = 0;

		for (auto c : text)
		{
			auto g = font->getGlyphSlot(c);
			if (g == nullptr)
				continue;

			// auto kerning = font->getFontKerning(prev_c, c);
			// cursor += kerning.x; //TODO
			// if (kerning.x) fprintf(stderr, "kerning of %lc and %lc is (%ld,%ld)\n", prev_c, c, kerning.x, kerning.y);

			int xoff = (cursor + g->metrics.horiBearingX + tex.b) >> 6;
			int yoff = (text_origin - g->metrics.horiBearingY + tex.b) >> 6;

			for (int y = 0; y < g->bitmap.rows; y++)
			{
				for (int x = 0; x < g->bitmap.width; x++)
				{
					auto & texel = buffer.at(xoff + x, yoff + y);
					texel.a = saturate_add(texel.a, g->bitmap.buffer[g->bitmap.width * y + x]);
					texel.a = saturate_add(texel.a, 31);
				}
			}

			// cursor += g->advance.x + text_spacing; //TODO
			cursor += g->advance.x;
			prev_c = c;
		}

		glDeleteTextures(1, &tex.id);
		glGenTextures(1, &tex.id);
		glBindTexture(GL_TEXTURE_2D, tex.id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.w, tex.h, 0, GL_BGRA, GL_UNSIGNED_BYTE, (uint8_t*)buffer.data());
	}

	virtual void drawText(int x, int y)
	{
		if (font == nullptr)
			return;

		int w = tex.w;
		int h = tex.h;
		x += tex.xoff;
		y += tex.yoff;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex.id);
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
		int w = tex.w;
		int h = tex.h;
		x += tex.xoff;
		y += tex.yoff;

		glColor3f(0.0f, 1.0f, 0.0f);
		glLineWidth(1);
		glBegin(GL_LINE_LOOP);
			glVertex2f(x, y);
			glVertex2f(x + w, y);
			glVertex2f(x + w, y + h);
			glVertex2f(x, y + h);
		glEnd();
	}

	void drawOrigin(int x, int y)
	{
		int len = text_width >> 6;
		y += text_size >> 14;

		glColor3f(1.0f, 0.0f, 0.0f);
		glLineWidth(1);
		glBegin(GL_LINES);
			glVertex2f(x, y);
			glVertex2f(x + len, y);
		glEnd();
	}

	void drawCross(int x, int y)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glLineWidth(1);
		glBegin(GL_LINES);
			glVertex2f(x - 10, y);
			glVertex2f(x + 10, y);
			glVertex2f(x, y - 10);
			glVertex2f(x, y + 10);
		glEnd();
	}

	void drawAll(int x, int y)
	{
		this->drawText(x, y);
		this->drawBounds(x, y);
		this->drawOrigin(x, y);
		this->drawCross(x, y);
	}

};
