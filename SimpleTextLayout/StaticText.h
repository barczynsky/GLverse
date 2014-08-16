#pragma once
#include <string>

#include "TrueTypeFont.h"

class StaticText
{
private:
	std::wstring text;
	TrueTypeFont font;

	GLuint font_texture{ 0 };

	FT_Pos text_width{ 0 };
	FT_Pos text_height{ 0 };
	FT_Pos text_origin{ 0 };

	FT_Pos text_max_width{ 0 };

	GLfloat text_color[4]{ 1.0f, 1.0f, 1.0f, 1.0f };
	FT_Pos text_spacing{ 0 };

	// unused
	int line_count{ 1 };
	int wrap_count{ 0 };

public:
	// StaticText(){}
	StaticText(std::string font_name, int font_size, std::wstring stext = L"");

public:
	std::wstring getText();
	void setText(std::wstring s, int max_width = 0);

	float getWidth();
	float getHeight();
	float getOrigin();

	void setColor(float r, float g, float b);
	void setOpacity(float pct = 100.0f);
	void setSpacing(float sp = 0.0f);

	// unused
	int getLineCount() { return line_count; }
	int getWrapCount() { return wrap_count; }

public:
	void refreshText();
	void drawText(int x, int y);

};
