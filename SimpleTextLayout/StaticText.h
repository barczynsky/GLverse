#pragma once
#include <string>

#include "BaseText.h"

class StaticText : public BaseText<std::wstring>
{
private:
	FT_Pos text_max_width{ 0 };

public:
	// StaticText(){}
	StaticText(std::string font_name, int font_size, StringType stext = StringType());

public:
	void setText(StringType s);
	void setText(StringType s, int max_width);
	void setSpacing(float sp = 0.0f);
	void setMaxWidth(int max_width = 0);

};
