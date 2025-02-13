#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

#include "U_DataTypes.h"

#define FIX_ANGLES(a) (a > 2*M_PI) ? a -= 2*M_PI : (a < 0) ? a += 2*M_PI : a
#define FIX_ANGLES_DEGREES(a) (a > 360) ? a -= 360 : (a < 0) ? a += 360 : a
#define RADIAN_TO_DEGREE 57.2957795131

void    U_SetBit(byte* b, byte n);
byte    U_GetBit(byte* b, byte n);
void    U_MixRand(long long);
int     U_Rand(int min, int max);

// -------------------------------
// Perform quicksorts before drawing drawables to draw from farther to nearest
// -------------------------------
void    U_QuicksortDrawables(drawabledata_t drawables[], int first, int last);

// -------------------------------
// Perform quicksorts before drawing drawables to draw from farther to nearest
// -------------------------------
void    U_QuicksortWallData(walldata_t data[], int first, int last);
#endif
