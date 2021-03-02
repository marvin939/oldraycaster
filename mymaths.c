#include <math.h>
#include "mymaths.h"

// Need gnu11 standard to compile and link to math lib.

double degreesToRadians(double const degrees)
{
	return M_PI / 180.0 * degrees;
}

double radiansToDegrees(double const radians)
{
	return 180.0 / M_PI * radians;
}


inline double degToRad(double const degrees) { return degreesToRadians(degrees); }
inline double radians(double const degrees)  { return degreesToRadians(degrees); }

inline double radToDeg(double const radians) { return radiansToDegrees(radians); }
inline double degrees(double const radians)  { return radiansToDegrees(radians); }
