#pragma once
#include <string>

#include "BaseText.h"

class LazyText : public BaseText<std::string>
{
private:
	bool text_changed{ false };

public:
	// LazyText(){}
	LazyText(std::string font_name, int font_size, StringType stext = StringType());

public:
	void setText(StringType s);
	void setSpacing(float sp = 0.0f);

public:
	void drawText(int x, int y);

};
