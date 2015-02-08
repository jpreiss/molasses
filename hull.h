#pragma once

#include "vec2.h"
#include "geom2d.h"

#include <algorithm>

std::vector<vec2> convex_hull(vec2 const *pts, size_t n)
{
	// Graham Scan algorithm
	// copied from Wikipedia pseudocode

	std::vector<vec2> sorted(n + 1);
	std::copy(pts, pts + n, sorted.begin() + 1);
	auto sbegin = sorted.begin() + 1;
	auto send = sorted.end();

	auto ymin = std::min_element(sbegin, send, y_lt);
	std::iter_swap(sbegin, ymin);

	std::sort(sbegin + 1, send,
		[&](vec2 a, vec2 b) { return angle(*sbegin, a) < angle(*sbegin, b); });

	sorted[0] = sorted[n];

	size_t m = 1;
	for (size_t i = 2; i <= n; ++i) {
		while (is_cw(turn_direction(sorted[m - 1], sorted[m], sorted[i]))) {
			if (m > 1) {
				--m;
			}
			else if (i == n) {
				break;
			}
			else {
				++i;
			}
		}
		++m;
		std::swap(sorted[m], sorted[i]);
	}

	sorted.resize(m);
	sorted.shrink_to_fit();
	return sorted;
}
