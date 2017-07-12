#pragma once
#include <cstdint>
#include <vector>


namespace TexelVectorElement
{
	struct BGRATexel
	{
		uint8_t b{};
		uint8_t g{};
		uint8_t r{};
		uint8_t a{};
	};
}

namespace TVE = TexelVectorElement;


class TexelVector : public std::vector<TVE::BGRATexel>
{
private:
	size_t tex_w{};
	size_t tex_h{};

public:
	TexelVector(size_t width, size_t height, TVE::BGRATexel seed)
	{
		tex_w = width;
		tex_h = height;
		resize(width * height, seed);
	}

public:
	decltype(auto) at(size_t pos)
	{
		return std::vector<TVE::BGRATexel>::at(pos);
	}

	decltype(auto) at(size_t x, size_t y)
	{
		return std::vector<TVE::BGRATexel>::at(tex_w * y + x);
	}

	auto get_w() const
	{
		return tex_w;
	}

	auto get_h() const
	{
		return tex_h;
	}
};
