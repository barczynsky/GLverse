#include "StaticText.h"

using namespace std;

StaticText::StaticText(string font_name, int font_size, StringType stext)
: BaseText(font_name, font_size)
{
	setText(std::move(stext));
}

void StaticText::setText(StringType stext)
{
	BaseText::setText(stext);
	makeText();
}

void StaticText::setText(StringType stext, int max_width)
{
	setMaxWidth(max_width);
	setText(stext);
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
