#include "FontRepository.h"

using namespace std;

FT_Library FontRepository::ft;
unique_ptr<FontRepository> FontRepository::font_repository_instance;

FontRepository& FontRepository::instance()
{
	if (!font_repository_instance)
	{
		font_repository_instance = make_unique<FontRepository>();
		FT_Init_FreeType(&ft);
	}
	return *font_repository_instance;
}

shared_ptr<TrueTypeFont> FontRepository::getFont(string font_name, int size)
{
	if (fonts.count(font_name))
	{
		if (fonts[font_name].count(size))
		{
			return fonts[font_name][size];
		}
	}

	FT_Face face;
	string font_path = "fonts/" + font_name;
	if(FT_New_Face(ft, (font_path + ".ttf").c_str(), 0, &face))
	{
		if(FT_New_Face(ft, (font_path + ".ttc").c_str(), 0, &face))
		{
			fprintf(stderr, "%s(.ttf|.ttc) font not found!\n", font_path.c_str());
			throw;
		}
	}
	FT_Set_Pixel_Sizes(face, 0, size);
	fonts[font_name][size] = make_shared<TrueTypeFont>(face, font_name);
	return fonts[font_name][size];
}
