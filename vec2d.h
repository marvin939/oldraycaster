#ifndef _VEC2D_H_
#define _VEC2D_H_

typedef struct Vector2D {
	double x;
	double y;
} Vec2D;

Vec2D Vec2D_add(Vec2D const v1, Vec2D const v2);
Vec2D Vec2D_subtract(Vec2D const v1, Vec2D const v2);
Vec2D Vec2D_multiply(double const scalar, Vec2D const vec);
double Vec2D_dot(Vec2D const v1, Vec2D const v2);
double Vec2D_norm(Vec2D const vec);
Vec2D Vec2D_normalize(Vec2D const* const vec);


#define Vec2D_length Vec2D_norm
#define Vec2D_magnitude Vec2D_norm

#endif
