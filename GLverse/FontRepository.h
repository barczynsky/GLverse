#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "TrueTypeFont.h"


class FontRepository
{
private:
	FT_Library ft;
	std::unordered_map<std::string, std::unordered_map<size_t, std::shared_ptr<TrueTypeFont>>> fonts;

private:
	FontRepository();

public:
	~FontRepository();
	static FontRepository& instance();

public:
	std::shared_ptr<TrueTypeFont> getFont(std::string font_name, size_t size);

};
