#include "LazyText.h"

using namespace std;

LazyText::LazyText(string font_name, int font_size) :
BaseText(font_name, font_size)
{
}

LazyText::LazyText(shared_ptr<TrueTypeFont> font_ptr) :
BaseText(font_ptr)
{
}

void LazyText::setText(StringType new_text)
{
	if (text != new_text)
		text_changed = true;
	BaseText::setText(new_text);
}

void LazyText::setFontSize(int font_size)
{
	if (text_size != font_size)
		text_changed = true;
	BaseText::setFontSize(font_size);
}

void LazyText::setSpacing(float sp)
{
	if (text_spacing != sp)
		text_changed = true;
	BaseText::setSpacing(sp);
}

void LazyText::makeText()
{
	if (text_changed)
		BaseText::makeText();
	text_changed = false;
}

void LazyText::drawText(int x, int y)
{
	makeText();
	BaseText::drawText(x, y);
}

void LazyText::drawAll(int x, int y)
{
	makeText();
	BaseText::drawAll(x, y);
}
