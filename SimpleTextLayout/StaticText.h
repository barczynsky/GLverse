#pragma once
#include <string>

#include "BaseText.h"

class StaticText : public BaseText<std::string>
{
private:
	FT_Pos text_max_width{ 0 };

public:
	// StaticText(){}
	StaticText(std::string font_name, int font_size, StringType text = StringType());

public:
	void setText(StringType new_text);
	void setText(StringType new_text, int max_width);
	void setSize(int new_size);
	void setSpacing(float sp = 0.0f);
	void setMaxWidth(int max_width = 0);

};
