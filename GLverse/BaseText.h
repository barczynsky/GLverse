#pragma once
#include <cmath>
#include <algorithm>
#include <codecvt>
#include <locale>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>
#include "saturate_add"

#include "FontRepository.h"
#include "TexelVector.h"
#include "TrueTypeFont.h"


template <typename string_type = std::string>
class BaseText
{
private:
	std::recursive_mutex base_mutex;

public:
	typedef string_type StringType;
	typedef typename string_type::value_type StringValueType;
	typedef std::basic_stringstream<typename string_type::value_type> StringStreamType;

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
		Right = 2,
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

protected:
	FT_Vector text_border{ 0, 0 };
	FT_Vector text_offset{ 0, 0 };

	FT_Pos text_size{ 0 };
	FT_Pos x_height{ 0 };

	FT_Pos text_baseline{ 0 };
	FT_Pos text_width{ 0 };
	FT_Pos text_height{ 0 };
	FT_Pos text_spacing{ 0 };
	FT_Pos text_interline{ 0 };

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
	std::shared_ptr<TrueTypeFont> getFont() const
	{
		return font;
	}

	StringType getText() const
	{
		return text;
	}

	TextOrigin getOrigin() const
	{
		return text_origin;
	}

	TextAlign getAlign() const
	{
		return text_align;
	}

	TextColor getColor() const
	{
		return text_color;
	}

	float getOpacity() const
	{
		return text_color.a * 100.0f;
	}

	float getBaseline() const
	{
		return static_cast<float>(text_baseline / 64.0);
	}

	float getWidth() const
	{
		return static_cast<float>(text_width / 64.0);
	}

	float getHeight() const
	{
		return static_cast<float>(text_height / 64.0);
	}

	float getSize() const
	{
		return static_cast<float>(text_size / 64.0);
	}

	float getSpacing() const
	{
		return static_cast<float>(text_spacing / 64.0);
	}

	float getLineSpacing() const
	{
		return static_cast<float>(text_interline / 64.0);
	}

	size_t getLineCount() const
	{
		return text_lines.size();
	}

	size_t getWrapCount() const
	{
		return text_lines.size() - 1;
	}


public:
	virtual void setFont(std::shared_ptr<TrueTypeFont> new_font)
	{
		std::lock_guard<std::recursive_mutex> lck(base_mutex);

		font = new_font;
		text_size = font->getFontHeight();
		x_height = font->getXHeight();
	}

	virtual void setFont(std::string font_name, int font_size)
	{
		std::lock_guard<std::recursive_mutex> lck(base_mutex);

		font = FontRepository::instance().getFont(font_name, font_size);
		text_size = font->getFontHeight();
		x_height = font->getXHeight();
	}

	virtual void setFontSize(int font_size)
	{
		std::lock_guard<std::recursive_mutex> lck(base_mutex);

		if (font)
		{
			setFont(font->getFontName(), font_size);
		}
	}

	virtual void setText(StringType new_text)
	{
		std::lock_guard<std::recursive_mutex> lck(base_mutex);

		text = new_text;
	}

	virtual void setSpacing(float sp = 0.0f)
	{
		std::lock_guard<std::recursive_mutex> lck(base_mutex);

		text_spacing = static_cast<FT_Pos>(std::floor(sp * 64.0));
	}

	virtual void setLineSpacing(float sp = 0.0f)
	{
		std::lock_guard<std::recursive_mutex> lck(base_mutex);

		text_interline = static_cast<FT_Pos>(std::floor(sp * 64.0));
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

	template <typename ColorType>
	void setColor(ColorType color)
	{
		text_color.r = color.r / 255.0f;
		text_color.g = color.g / 255.0f;
		text_color.b = color.b / 255.0f;
	}

public:
	virtual std::vector<StringType> getLines() const
	{
		return text_lines;
	}

	template <typename T>
	decltype(auto) split(const std::basic_string<T>& s, const std::initializer_list<T> cs)
	{
		using pos_t = typename std::basic_string<T>::size_type;
		std::basic_string<T> css{ cs.begin(), cs.end() };
		std::vector<std::basic_string<T>> parts;
		pos_t next{ 0 };
		pos_t pos{ 0 };
		do {
			next = s.find_first_of(cs, pos);
			parts.push_back(s.substr(pos, next - pos));
			pos = next + 1;
		} while (next != std::basic_string<T>::npos);
		return parts;
	}

	template <typename T>
	decltype(auto) split(const std::basic_string<T>& s, const T c)
	{
		return split(s, { c });
	}

	virtual void splitText()
	{
		std::lock_guard<std::recursive_mutex> lck(base_mutex);

		text_lines.clear();
		text_lines_w.clear();

		StringValueType newline{ '\n' };
		text_lines = split(text, newline);
		text_lines_w.resize(text_lines.size());
	}

	virtual void measureText()
	{
		std::lock_guard<std::recursive_mutex> lck(base_mutex);

		text_baseline = 0;
		text_width = 0;
		text_height = 0;

		if (text_lines.empty())
			return;

		for (int i = 0; i < 1; i++)
		{
			FT_Pos line_width = 0;
			FT_Pos max_ascent = 0;
			StringValueType prev_c = 0;
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
		for (int i = 1; i < (int)text_lines.size() - 1; i++)
		{
			FT_Pos line_width = 0;
			StringValueType prev_c = 0;
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
		text_height += text_interline * (text_lines.size() - 1);
		for (int i = text_lines.size() - 1; i < (int)text_lines.size(); i++)
		{
			FT_Pos line_width = 0;
			FT_Pos max_descent = 0;
			StringValueType prev_c = 0;
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

	virtual void prepareText()
	{
		std::lock_guard<std::recursive_mutex> lck(base_mutex);

		splitText();
		measureText();
	}

	virtual void makeText()
	{
		if (font == nullptr) return;

		std::lock_guard<std::recursive_mutex> lck(base_mutex);

		prepareText();

		text_border.x = std::max<FT_Pos>(3 << 6, (text_size >> 3) >> 6 << 6);
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
		for (int i = 0; i < (int)text_lines.size(); i++)
		{
			auto&& line = text_lines[i];

			FT_Pos cursor = 0 - (font->getGlyphSlot(line.front())->metrics.horiBearingX >> 6);
			StringValueType prev_c = 0;
			for (auto c : line)
			{
				auto g = font->getGlyphSlot(c);
				if (g == nullptr)
					continue;

				auto kerning = font->getFontKerning(prev_c, c);
				cursor += kerning.x;
				// if (kerning.x || kerning.y) fprintf(stderr, "kerning for '%lc' after '%lc' is (%ld,%ld)\n", prev_c, c, kerning.x, kerning.y);
			#ifdef TARGET_LCD
				const int bitmap_width = g->bitmap.width / 3;
			#else
				const int bitmap_width = g->bitmap.width;
			#endif
				const int bitmap_height = g->bitmap.rows;
				int xoff = (cursor + g->metrics.horiBearingX + text_border.x) >> 6;
				xoff += (int)(((text_width - text_lines_w[i]) >> 6) * (float)text_align / 2.0f);
				int yoff = (current_baseline - g->metrics.horiBearingY + text_border.y) >> 6;
				for (int y = 0; y < bitmap_height; y++)
				{
					for (int x = 0; x < bitmap_width; x++)
					{
						auto&& texel = buffer.at(xoff + x, yoff + y);
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
			current_baseline += text_interline;
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
	float measureString(StringType string)
	{
		FT_Pos string_width = 0;

		StringValueType prev_c = 0;
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

public:
	float transition(float x, float xoff, float yoff)
	{
		return std::abs(x - xoff) + yoff;
	}

	void transformOrigin(int & x, int & y)
	{
		x += (int)((text_width >> 6) * -text_origin.x);
		y += (int)((text_size >> 6) * (text_lines.size() - 1) * -text_origin.y);
		y += (int)((x_height >> 6) * -transition(text_origin.y, 0.5f, -0.5f));
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

		for (int i = 0; i < (int)text_lines.size(); ++i)
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

public:
	static std::u32string u8_to_u32(std::string s)
	{
		return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(s);
	}

	static std::u32string u16_to_u32(std::u16string u16s)
	{
		return u8_to_u32(std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16s));
	}

	static std::u32string ws_to_u32(std::wstring ws)
	{
		return u8_to_u32(std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.to_bytes(ws));
	}

	static std::u32string s_to_u32(std::string s)
	{
		return u8_to_u32(s);
	}

	static std::u32string to_u32string(std::string s)
	{
		return u8_to_u32(s);
	}

	static std::u32string to_u32string(std::u16string s)
	{
		return u16_to_u32(s);
	}

	static std::u32string to_u32string(std::wstring s)
	{
		return ws_to_u32(s);
	}

};
