#include <cstddef>
#include "TexelVector.h"

using namespace std;

TexelVector::TexelVector(size_t width, size_t height, TexelVector::BGRATexel init)
{
	texture_width = width;
	texture_height = height;
	texel_vector.resize(width * height, init);
}

vector<TexelVector::BGRATexel>::reference TexelVector::at(size_t pos)
{
	return texel_vector.at(pos);
}

vector<TexelVector::BGRATexel>::reference TexelVector::at(size_t x, size_t y)
{
	return texel_vector.at(texture_width * y + x);
}

TexelVector::BGRATexel* TexelVector::data()
{
	return texel_vector.data();
}

size_t TexelVector::size()
{
	return texel_vector.size();
}
