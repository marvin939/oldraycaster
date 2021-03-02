#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "vec2d.h"

int main(int argc, char* argv[])
{
	Vec2D v1 = {1, -5};
	Vec2D v2 = {-3, 8};
	Vec2D v3;
	
	// Test addition
	v3 = Vec2D_add(v1, v2);
	assert(v3.x == -2 && "Vec2D .x addition failed");
	assert(v3.y == 3 && "Vec2D .y addition failed");
	
	// Test subtraction
	v3 = Vec2D_subtract(v1, v2);
	assert(v3.x == 4 && "Vec2D .x subtract failed");
	assert(v3.y == -13 && "Vec2D .y subtract failed");
	
	// Test scalar multiplication
	v3 = Vec2D_multiply(5, v1);
	assert(v3.x == 5.0 && "Vec2D .x scalar multiplication failed");
	assert(v3.y == -25.0 && "Vec2D .y scalar multiplication failed");
	
	// Test dot product
	double dotproduct = Vec2D_dot(v1, v1);
	assert(dotproduct == 26 && "Vec2D dot product failed");
	
	// Test norm/length
	double norm = Vec2D_norm((Vec2D){4, 3});
	assert(norm == 5 && "Vec2D norm failed");
	norm = Vec2D_norm((Vec2D){3, -4});
	assert(norm == 5 && "Vec2D norm failed");
	
	// Test norm aliases
	norm = Vec2D_length((Vec2D){4, 3});
	assert(norm == 5 && "Vec2D norm alias \"length\" failed");
	norm = Vec2D_magnitude((Vec2D){4, 3});
	assert(norm == 5 && "Vec2D norm alias \"magnitude\" failed");
	
	// Test normalized
	v3 = Vec2D_normalize(&v1);
	norm = Vec2D_norm(v3);
	assert(1.0 == norm && "Vec2D normalized length is not equal to 1!");
	
	printf("Vector2D test successful.\n");
}
