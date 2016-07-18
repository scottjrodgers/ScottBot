//fix.h:
#ifndef FIX_H
#define FIX_H

#include <arduino.h>
#include "stdint.h"

typedef int32_t fixed_t;

//Pragmas
#define FP_IBITS        16 //integer bits
#define FP_FBITS        15 //fraction bits
#define FP_BITS         32 //total bits (s16.15)
#define FP_MIN          -2147450880L
#define FP_MAX          2147450880L
#define FP_FMASK        (((int32_t)1<<FP_FBITS) - 1)
#define FP_ONE          ((fixed_t)0x8000)
#define FP_CONST(R)     ((fixed_t)((R)*FP_ONE + ((R) >= 0 ? 0.5 : -0.5)))
#define FP_PI           FP_CONST(3.14159265358979323846)
#define FP_TWO_PI       FP_CONST(2*3.14159265358979323846)
#define FP_HALF_PI      FP_CONST(3.14159265358979323846/2)
#define FP_ABS(A)       ((A) < 0 ? -(A) : (A))
#define FP_FRAC_PART(A) ((fixed_t)(A)&FP_FMASK)
#define FP_DegToRad(D)  (FP_Divide(D, (fixed_t)1877468))
#define FP_RadToDeg(R)  (FP_Multiply(R, (fixed_t)18529868))

//conversion Functions
#define itok(i)         ( (fixed_t)( (int32_t)i<<(int32_t)FP_FBITS ) )
#define ktoi(k)         ( ( (int16_t)( (int32_t)k>>(int32_t)FP_FBITS ) )&0x0000ffff )
#define ftok(f)         ( (fixed_t)(((float)f)*(32768)) )
#define ktof(k)         ( (float) k / 32768.0 )

//extern float FP_FixedToFloat(fixed_t);
//extern fixed_t FP_FloatToFixed(float);

fixed_t FP_StringToFixed(char *s);
void FP_FixedToString(fixed_t f, char *s);

//basic math
// extern fixed_t FP_Inv(fixed_t);
extern fixed_t FP_Multiply(fixed_t, fixed_t);
extern fixed_t FP_Divide(fixed_t, fixed_t);

//special functions
fixed_t FP_Round(fixed_t, uint8_t);

//square root
extern fixed_t _FP_SquareRoot(fixed_t, fixed_t);
#define FP_Sqrt(a)      _FP_SquareRoot(a, 15);

//trig
// extern fixed_t FP_Sin(fixed_t);
// #define FP_Cos(A)       (FP_Sin(FP_HALF_PI - A))
// #define FP_Tan(A)       (FP_Division(FP_Sin(A), FP_Cos(A)))

#endif //FIX_H
