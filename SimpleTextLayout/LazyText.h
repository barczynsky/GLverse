#pragma once
#include <string>

#include "BaseText.h"

class LazyText : public BaseText<std::string>
{
private:
	bool text_changed{ false };

public:
	// LazyText(){}
	LazyText(std::string font_name, int font_size, StringType text = StringType());

public:
	void setText(StringType new_text);
	void setSize(int new_size);
	void setSpacing(float sp = 0.0f);

public:
	void drawText(int x, int y);

};
