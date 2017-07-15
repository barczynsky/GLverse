#pragma once
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "BaseText.h"


class LazyText : public BaseText<std::u32string>
{
private:
	std::mutex lazy_mutex;
	bool text_changed{ true };

private:
	StringType unbroken_text;
	bool attempt_to_break{ false };
	bool needs_rebreaking{ false };
	float max_line_length{};

public:
	// LazyText(){}
	LazyText(std::string font_name, int font_size);
	LazyText(std::shared_ptr<TrueTypeFont> font_ptr);

public:
	void setText(StringType new_text);
	void setText(std::string new_text);
	void setText(std::u16string new_text);
	void setText(std::wstring new_text);
	void setFontSize(int font_size);
	void setSpacing(float spacing);
	void setMaxLineLength(float length);
	void setFont(std::string font_name, int font_size);
	void setFont(std::shared_ptr<TrueTypeFont> font_ptr);

public:
	void makeText();
	void drawText(int x, int y);
	void drawAll(int x, int y);

public:
	StringType fitText(StringType text);
	float measureString(std::string s);
	float measureString(std::u16string s);
	float measureString(std::wstring s);

};
