#include "FontRepository.h"
#include <stdexcept>
#include <vector>
using namespace std::literals;


FontRepository::FontRepository()
{
	FT_Init_FreeType(&ft);
}

FontRepository::~FontRepository()
{
	FT_Done_FreeType(ft);
}

FontRepository& FontRepository::instance()
{
	static FontRepository font_repository{};
	return font_repository;
}

std::shared_ptr<TrueTypeFont> FontRepository::getFont(std::string font_name, size_t size)
{
	if (fonts.count(font_name))
	{
		if (fonts[font_name].count(size))
		{
			return fonts[font_name][size];
		}
	}

	FT_Face face;
	std::initializer_list<std::string> font_locations{ "fonts/", "./" };
	std::initializer_list<std::string> font_extensions{ ".ttf", ".ttc", ".otf", "" };
	for (auto location : font_locations)
	{
		for (auto extension : font_extensions)
		{
			if (FT_New_Face(ft, (location + font_name + extension).c_str(), 0, &face) == 0)
			{
				FT_Set_Pixel_Sizes(face, 0, size);
				fonts[font_name][size] = std::make_shared<TrueTypeFont>(face, font_name);
				return fonts[font_name][size];
			}
		}
	}

	throw std::runtime_error("missing font: "s + font_name + "(.ttf|.ttc|.otf)\n"s);
}
