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
	std::unordered_map<std::string, std::unordered_map<int, std::shared_ptr<TrueTypeFont>>> fonts;

private:
	static FT_Library ft;
	static std::unique_ptr<FontRepository> font_repository_instance;

public:
	static FontRepository& instance();

	TrueTypeFont& getFont(std::string font_name, int size);

};
