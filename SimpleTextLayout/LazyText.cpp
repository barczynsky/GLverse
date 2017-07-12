#include "LazyText.h"
#include <unordered_set>


LazyText::LazyText(std::string font_name, int font_size):
	BaseText(font_name, font_size)
{
}

LazyText::LazyText(std::shared_ptr<TrueTypeFont> font_ptr):
	BaseText(font_ptr)
{
}

void LazyText::setText(StringType new_text)
{
	std::lock_guard<std::mutex> lck(lazy_mutex);
	if (needs_rebreaking || (attempt_to_break && new_text != unbroken_text))
	{
		unbroken_text = new_text;
		new_text = this->fitText(new_text);
		text_changed = true;
	}
	else if (!attempt_to_break && text != new_text)
	{
		text_changed = true;
	}
	BaseText::setText(new_text);
	BaseText::prepareText();
}

void LazyText::setText(std::string new_text)
{
	setText(u8_to_u32(new_text));
}

void LazyText::setText(std::u16string new_text)
{
	setText(to_u32string(new_text));
}

void LazyText::setText(std::wstring new_text)
{
	setText(to_u32string(new_text));
}

void LazyText::setFontSize(int font_size)
{
	std::lock_guard<std::mutex> lck(lazy_mutex);
	if (text_size != font_size)
	{
		text_changed = true;
	}
	BaseText::setFontSize(font_size);
	BaseText::prepareText();
}

void LazyText::setSpacing(float spacing)
{
	std::lock_guard<std::mutex> lck(lazy_mutex);
	if (text_spacing != spacing)
	{
		text_changed = true;
	}
	BaseText::setSpacing(spacing);
	BaseText::prepareText();
}

void LazyText::setMaxLineLength(float length)
{
	attempt_to_break = true;
	if (max_line_length != length)
	{
		needs_rebreaking = true;
	}
	max_line_length = length;
}

void LazyText::setFont(std::shared_ptr<TrueTypeFont> font_ptr)
{
	if (font_ptr.get() != font.get())
	{
		text_changed = true;
	}
	BaseText::setFont(font_ptr);
	BaseText::prepareText();
}

void LazyText::setFont(std::string font_name, int font_size)
{
	auto font_ptr = FontRepository::instance().getFont(font_name, font_size);
	if (font_ptr.get() != font.get())
	{
		text_changed = true;
	}
	BaseText::setFont(font_ptr);
	BaseText::prepareText();
}

void LazyText::makeText()
{
	std::lock_guard<std::mutex> lck(lazy_mutex);
	if (text_changed)
	{
		BaseText::makeText();
	}
	text_changed = false;
}

void LazyText::drawText(int x, int y)
{
	this->makeText();
	std::lock_guard<std::mutex> lck(lazy_mutex);
	BaseText::drawText(x, y);
}

void LazyText::drawAll(int x, int y)
{
	this->makeText();
	std::lock_guard<std::mutex> lck(lazy_mutex);
	BaseText::drawAll(x, y);
}

LazyText::StringType LazyText::fitText(LazyText::StringType text)
{
	using pos_t = typename std::basic_string<StringValueType>::size_type;
	StringValueType newline{ '\n' };
	StringValueType whitespace{ ' ' }; //TODO: no-break space (0x00A0) / CJK ideographic space (0x3000)
	auto words = BaseText::split(text, { whitespace, newline });

	auto it = text.begin();
	std::unordered_set<StringValueType> separator{ newline, whitespace };
	std::vector<pos_t> separator_index;

	for (size_t i = 0; i < text.size(); ++i)
	{
		if (separator.count(text[i]) == 1)
		{
			separator_index.push_back(i);
			if (text[i] == newline)
			{
				text[i] = whitespace;
			}
		}
	}

	pos_t p{};
	for (size_t i = 0; i < separator_index.size(); ++i)
	{
		auto sep_p = separator_index[i];
		auto pre_p = separator_index[std::max<size_t>(i - 1, 0)];
		auto p_it = it + p;
		auto sep_it = it + sep_p;
		auto pre_it = it + pre_p;

		if (BaseText::measureString({ p_it, sep_it }) > max_line_length)
		{
			*pre_it = newline;
			p = sep_p + 1;
		}
	}

	return text;
}

float LazyText::measureString(std::string s)
{
	return BaseText::measureString(to_u32string(s));
}

float LazyText::measureString(std::u16string s)
{
	return BaseText::measureString(to_u32string(s));
}

float LazyText::measureString(std::wstring s)
{
	return BaseText::measureString(to_u32string(s));
}
