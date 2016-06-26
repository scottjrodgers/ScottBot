// Fixed Math Definitions
#ifndef FIXED_MATH
#define FIXED_MATH

// Define what the value 1.0f is in fixed integer math
#define F_ONE 0x1000

// Declare my own time for clarity
typedef int fixed_t;

// Declare vector
typedef struct{
  fixed_t x;
  fixed_t y;
  fixed_t z;
} vector_t;

// Declare vector
typedef struct{
  fixed_t x;
  fixed_t y;
  fixed_t z;
  fixed_t dx;
  fixed_t dy;
  fixed_t dz;
} k_vector_t;

#endif

