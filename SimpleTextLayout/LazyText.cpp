#include "LazyText.h"

using namespace std;

LazyText::LazyText(string font_name, int font_size, StringType text)
: BaseText(font_name, font_size)
{
	setText(std::move(text));
}

void LazyText::setText(StringType new_text)
{
	if (getText() != new_text)
		text_changed = true;
	BaseText::setText(new_text);
}

void LazyText::setSize(int new_size)
{
	if (getSize() != new_size)
		text_changed = true;
	BaseText::setSize(new_size);
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
