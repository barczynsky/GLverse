#include "StaticText.h"

using namespace std;

StaticText::StaticText(string font_name, int font_size, StringType text)
: BaseText(font_name, font_size)
{
	setText(std::move(text));
}

void StaticText::setText(StringType new_text)
{
	BaseText::setText(new_text);
	makeText();
}

void StaticText::setText(StringType new_text, int max_width)
{
	setMaxWidth(max_width);
	setText(new_text);
}

void StaticText::setSize(int new_size)
{
	BaseText::setSize(new_size);
	makeText();
}

void StaticText::setSpacing(float sp)
{
	BaseText::setSpacing(sp);
	makeText();
}

void StaticText::setMaxWidth(int max_width)
{
	text_max_width = max_width;
}
