#pragma once
#include <string>

#include "BaseText.h"

class LazyText : public BaseText<std::string>
{
private:
	bool text_changed{ true };

public:
	// LazyText(){}
	LazyText(std::string font_name, int font_size);
	LazyText(std::shared_ptr<TrueTypeFont> font_ptr);

public:
	void setText(StringType new_text);
	void setFontSize(int font_size);
	void setSpacing(float sp = 0.0f);

public:
	void makeText();
	void drawText(int x, int y);
	void drawAll(int x, int y);

};
