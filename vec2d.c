#include "vec2d.h"
#include <math.h>

Vec2D Vec2D_add(Vec2D const v1, Vec2D const v2)
{
	return (Vec2D){v1.x + v2.x, v1.y + v2.y};
}

Vec2D Vec2D_subtract(Vec2D const v1, Vec2D const v2)
{
	return (Vec2D){v1.x - v2.x, v1.y - v2.y};
}

Vec2D Vec2D_multiply(double const scalar, Vec2D const vec)
{
	return (Vec2D){scalar * vec.x, scalar * vec.y};
}

inline double Vec2D_dot(Vec2D const v1, Vec2D const v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

inline double Vec2D_norm(Vec2D const vec)
{
	// TOO many copies if dot is not inline:
	return sqrt(Vec2D_dot(vec, vec));
	//~ return sqrt(vec.x * vec.x + vec.y * vec.y);
}

Vec2D Vec2D_normalize(Vec2D const* const vec)
{
	Vec2D normalized;
	double norm = Vec2D_norm(*vec);
	normalized = Vec2D_multiply(1.0 / norm, *vec);
	
	return normalized;
}
