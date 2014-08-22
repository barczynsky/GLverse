#include "LazyText.h"

using namespace std;

LazyText::LazyText(string font_name, int font_size, StringType stext)
: BaseText(font_name, font_size)
{
	setText(std::move(stext));
}

void LazyText::setText(StringType stext)
{
	if (getText() != stext)
		text_changed = true;
	BaseText::setText(stext);
}

void LazyText::setSpacing(float sp)
{
	if (getSpacing() != sp)
		text_changed = true;
	BaseText::setSpacing(sp);
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
