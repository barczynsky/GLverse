#pragma once
#include <algorithm>
#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include "saturate_add"

#include "BaseText.h"
#include "FontRepository.h"
#include "TexelVector.h"
#include "TrueTypeFont.h"

template<typename text_t = std::string>
class BaseText
{
public:
	typedef text_t StringType;

	union TextColor {
		struct {
			float r,g,b,a;
		};
		operator GLfloat*() { return color4fv; }
		GLfloat color4fv[4];
	};
	union TextOrigin {
		struct {
			float x,y;
		};
		operator GLfloat*() { return origin2fv; }
		GLfloat origin2fv[2];
	};
	enum class TextAlign {
		Left = 0,
		Center = 1,
		Right = 2
	};

protected:
	std::shared_ptr<TrueTypeFont> font;

protected:
	std::vector<StringType> text_lines;
	std::vector<FT_Pos> text_lines_w;

protected:
	StringType text;
	TextColor text_color{ { 1.0f, 1.0f, 1.0f, 1.0f } };
	TextOrigin text_origin{ { 0.0f, 0.0f } };
	TextAlign text_align{ TextAlign::Left };

protected:
	GLtexture texture{};
	FT_Vector text_border{ 0, 0 };
	FT_Vector text_offset{ 0, 0 };

	FT_Pos text_size{ 0 };
	FT_Pos x_height{ 0 };

	FT_Pos text_baseline{ 0 };
	FT_Pos text_width{ 0 };
	FT_Pos text_height{ 0 };
	FT_Pos text_spacing{ 0 };
	// FT_Pos text_interline{ 0 };

public:
	// BaseText(){}
	BaseText(std::string font_name, int font_size)
	{
		setFont(font_name, font_size);
	}

	BaseText(std::shared_ptr<TrueTypeFont> font_ptr)
	{
		setFont(font_ptr);
	}

	virtual ~BaseText()
	{
		glDeleteTextures(1, &texture.tex_id);
	}

public:
	std::shared_ptr<TrueTypeFont> getFont()
	{
		return font;
	}

	StringType getText()
	{
		return text;
	}

	TextOrigin getOrigin()
	{
		return text_origin;
	}

	TextAlign getAlign()
	{
		return text_align;
	}

	TextColor getColor()
	{
		return text_color;
	}

	float getOpacity()
	{
		return text_color.a * 100.0f;
	}

	float getBaseline()
	{
		return static_cast<float>(text_baseline / 64.0);
	}

	float getWidth()
	{
		return static_cast<float>(text_width / 64.0);
	}

	float getHeight()
	{
		return static_cast<float>(text_height / 64.0);
	}

	float getSize()
	{
		return static_cast<float>(text_size / 64.0);
	}

	float getSpacing()
	{
		return static_cast<float>(text_spacing / 64.0);
	}

	size_t getLineCount()
	{
		return text_lines.size();
	}

	size_t getWrapCount()
	{
		return text_lines.size() - 1;
	}

public:
	virtual void setFont(std::shared_ptr<TrueTypeFont> new_font)
	{
		font = std::move(new_font);
		text_size = font->getFontHeight();
		x_height = font->getXHeight();
	}

	virtual void setFont(std::string font_name, int font_size)
	{
		font = std::move(FontRepository::instance().getFont(font_name, font_size));
		text_size = font->getFontHeight();
		x_height = font->getXHeight();
	}

	virtual void setFontSize(int font_size)
	{
		setFont(font->getFontName(), font_size);
	}

	virtual void setText(StringType new_text)
	{
		text = std::move(new_text);
	}

	virtual void setSpacing(float sp = 0.0f)
	{
		text_spacing = static_cast<FT_Pos>(std::floor(sp * 64.0));
	}

	void setOrigin(float x, float y)
	{
		text_origin.x = x;
		text_origin.y = y;
	}

	void setAlign(TextAlign align)
	{
		text_align = align;
	}

	void setOpacity(float pct = 100.0f)
	{
		text_color.a = pct / 100.0f;
	}

	void setOpacity(int a)
	{
		text_color.a = a / 255.0f;
	}

	void setColor(float r, float g, float b)
	{
		text_color.r = r;
		text_color.g = g;
		text_color.b = b;
	}

	void setColor(int r, int g, int b)
	{
		text_color.r = r / 255.0f;
		text_color.g = g / 255.0f;
		text_color.b = b / 255.0f;
	}

	template<typename ColorType>
	void setColor(ColorType color)
	{
		text_color.r = color.r;
		text_color.g = color.g;
		text_color.b = color.b;
	}

public:
	float measureString(StringType string)
	{
		FT_Pos string_width = 0;

		typename StringType::value_type prev_c = 0;
		for (auto c : string)
		{
			auto g = font->getGlyphSlot(c);
			if (g == nullptr)
				continue;

			auto kerning = font->getFontKerning(prev_c, c);
			string_width += g->advance.x + kerning.x + text_spacing;
		}

		return static_cast<float>(string_width / 64.0);
	}

	float coneFunc(float x, float xoff, float yoff)
	{
		return std::abs(x - xoff) + yoff;
	}

	void transformOrigin(int & x, int & y)
	{
		x += (text_width >> 6) * -text_origin.x;
		y += (text_size >> 6) * (text_lines.size() - 1) * -text_origin.y;
		y += (x_height >> 6) * -coneFunc(text_origin.y, 0.5f, -0.5f);
	}

public:
	virtual void splitText()
	{
		text_lines.clear();
		std::istringstream ss(text);

		while(!ss.eof())
		{
			text_lines.emplace_back();
			std::getline(ss, text_lines.back());
		}
		text_lines_w.resize(text_lines.size());
	}

	virtual void measureText()
	{
		text_baseline = 0;
		text_width = 0;
		text_height = 0;

		for (int i = 0; i < 1; i++)
		{
			FT_Pos line_width = 0;
			FT_Pos max_ascent = 0;
			typename StringType::value_type prev_c = 0;
			for (auto c : text_lines[i])
			{
				auto g = font->getGlyphSlot(c);
				if (g == nullptr)
					continue;

				auto kerning = font->getFontKerning(prev_c, c);
				line_width += g->advance.x + kerning.x + text_spacing;
				max_ascent = std::max(max_ascent, g->metrics.horiBearingY);
			}
			text_baseline = max_ascent;
			text_width = std::max(text_width, line_width);
			text_height += max_ascent;
			text_lines_w[i] = line_width;
		}
		for (int i = 1; i < text_lines.size() - 1; i++)
		{
			FT_Pos line_width = 0;
			typename StringType::value_type prev_c = 0;
			for (auto c : text_lines[i])
			{
				auto g = font->getGlyphSlot(c);
				if (g == nullptr)
					continue;

				auto kerning = font->getFontKerning(prev_c, c);
				line_width += g->advance.x + kerning.x + text_spacing;
			}
			text_width = std::max(text_width, line_width);
			text_lines_w[i] = line_width;
		}
		text_height += text_size * (text_lines.size() - 1);
		for (int i = text_lines.size() - 1; i < text_lines.size(); i++)
		{
			FT_Pos line_width = 0;
			FT_Pos max_descent = 0;
			typename StringType::value_type prev_c = 0;
			for (auto c : text_lines[i])
			{
				auto g = font->getGlyphSlot(c);
				if (g == nullptr)
					continue;

				auto kerning = font->getFontKerning(prev_c, c);
				line_width += g->advance.x + kerning.x + text_spacing;
				max_descent = std::max(max_descent, g->metrics.height - g->metrics.horiBearingY);
			}
			text_width = std::max(text_width, line_width);
			text_height += max_descent;
			text_lines_w[i] = line_width;
		}
	}

	virtual void makeText()
	{
		if (font == nullptr)
			return;

		splitText();
		measureText();

		text_border.x = std::max(2L, (text_size >> 2) >> 6 << 6);
		text_border.y = text_border.x;
		texture.tex_w = (text_width + text_border.x * 3) >> 6;
		texture.tex_h = (text_height + text_border.y * 2) >> 6;
		texture.tex_w = 2 << (int)std::log2(texture.tex_w);
		texture.tex_h = 2 << (int)std::log2(texture.tex_h);
		text_offset.x = 0 - ((text_border.x) >> 6 << 6);
		text_offset.y = 0 - ((text_border.y + text_baseline) >> 6 << 6);
		// fprintf(stderr, "Text: '%s'\n", text.c_str());
		// fprintf(stderr, "W: %d(%ld), H: %d(%ld)\n", texture.tex_w, text_width, texture.tex_h, text_height);
		// fprintf(stderr, "OrigX: %ld(%ld), OrigY: %ld(%ld)\n", text_offset.x >> 6, text_offset.x, text_offset.y >> 6, text_offset.y);

		TexelVector buffer(texture.tex_w, texture.tex_h, { 0, 0, 0, 0 });
		FT_Pos current_baseline = text_baseline;
		for (int i = 0; i < text_lines.size(); i++)
		{
			auto & line = text_lines[i];

			FT_Pos cursor = 0 - (font->getGlyphSlot(line.front())->metrics.horiBearingX >> 6);
			typename StringType::value_type prev_c = 0;
			for (auto c : line)
			{
				auto g = font->getGlyphSlot(c);
				if (g == nullptr)
					continue;

				auto kerning = font->getFontKerning(prev_c, c);
				cursor += kerning.x;
				// if (kerning.x || kerning.y) fprintf(stderr, "kerning for '%lc' after '%lc' is (%ld,%ld)\n", prev_c, c, kerning.x, kerning.y);
			#ifdef TARGET_LCD
				int bitmap_width = g->bitmap.width / 3;
			#else
				int bitmap_width = g->bitmap.width;
			#endif
				int xoff = (cursor + g->metrics.horiBearingX + text_border.x) >> 6;
				xoff += ((text_width - text_lines_w[i]) >> 6) * (float)text_align / 2.0f;
				int yoff = (current_baseline - g->metrics.horiBearingY + text_border.y) >> 6;
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
						texel.a = std::max({ texel.r, texel.g, texel.b });
					#else
						texel.r = 255;
						texel.g = 255;
						texel.b = 255;
						texel.a = saturate_add(texel.a, g->bitmap.buffer[g->bitmap.pitch * y + x]);
					#ifdef GLYPH_SHADOWS
						texel.a = saturate_add(texel.a, GLYPH_SHADOWS);
					#endif
					#endif
					}
				}
				cursor += g->advance.x + text_spacing;
				prev_c = c;
			}
			current_baseline += text_size;
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

public:
	virtual void drawAll(int x, int y)
	{
		drawBounds(x, y);
		drawBaseline(x, y);
		drawOrigin(x, y);
		BaseText::drawText(x, y);
	}

	virtual void drawText(int x, int y)
	{
		int w = texture.tex_w;
		int h = texture.tex_h;
		x += text_offset.x >> 6;
		y += text_offset.y >> 6;

		transformOrigin(x, y);

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
		x += text_offset.x >> 6;
		y += text_offset.y >> 6;

		transformOrigin(x, y);

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
		transformOrigin(x, y);

		for (int i = 0; i < text_lines.size(); ++i)
		{
			int s = text_size >> 6;
			int w = text_width >> 6;

			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			glLineWidth(1);
			glBegin(GL_LINES);
				glVertex2f(x, y + s * i);
				glVertex2f(x + w, y + s * i);
			glEnd();
		}
	}

	void drawOrigin(int x, int y)
	{
		int r = 10;

		glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
		glLineWidth(1);
		glBegin(GL_LINES);
			glVertex2f(x - r, y);
			glVertex2f(x + r, y);
			glVertex2f(x, y - r);
			glVertex2f(x, y + r);
		glEnd();
	}

};
