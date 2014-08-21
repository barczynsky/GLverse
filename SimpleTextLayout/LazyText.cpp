#include "LazyText.h"

using namespace std;

LazyText::LazyText(string font_name, int font_size, StringType stext)
: BaseText(font_name, font_size)
{
	setText(std::move(stext));
}

void LazyText::setText(StringType stext)
{
	BaseText::setText(stext);
	text_changed = true;
}

void LazyText::setSpacing(float sp)
{
	BaseText::setSpacing(sp);
	text_changed = true;
}

void LazyText::drawText(int x, int y)
{
	if (text_changed)
	{
		BaseText::makeText();
		text_changed = false;
	}
	BaseText::drawText(x, y);
}
