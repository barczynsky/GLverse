#pragma once
#include <cstdint>
#include <vector>

class TexelVector
{
public:
	struct BGRATexel
	{
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t a;
	};

private:
	std::vector<BGRATexel> texel_vector;
	size_t texture_width;
	size_t texture_height;

public:
	TexelVector(size_t width, size_t height, BGRATexel init);

	std::vector<BGRATexel>::reference at(size_t pos);
	std::vector<BGRATexel>::reference at(size_t x, size_t y);
	BGRATexel* data();
	size_t size();

};
