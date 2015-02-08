#pragma once

#include <random>

template <typename Iter>
void random_uniform(vec2 mins, vec2 maxs, Iter begin, Iter end)
{
	std::random_device rd;
	std::default_random_engine engine(rd());
	std::uniform_real_distribution<float> dx(mins.x, maxs.x);
	std::uniform_real_distribution<float> dy(mins.y, maxs.y);

	for (; begin != end; ++begin) {
		begin->x = dx(engine);
		begin->y = dy(engine);
	}
}
