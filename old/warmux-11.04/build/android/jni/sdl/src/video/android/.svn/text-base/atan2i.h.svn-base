#ifndef __ATAN2I_H__
#define __ATAN2I_H__
#include <math.h>

// Fast arctan2, returns angle in radians as integer, with fractional part in lower 16 bits
// Stolen from http://www.dspguru.com/dsp/tricks/fixed-point-atan2-with-self-normalization , precision is said to be 0.07 rads

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
enum { atan2i_coeff_1 = ((int)(M_PI*65536.0/4)), atan2i_coeff_2 = (3*atan2i_coeff_1), atan2i_PI = (int)(M_PI * 65536.0) };

static inline int atan2i(int y, int x)
{
   int angle;
   int abs_y = abs(y);
   if( abs_y == 0 )
      abs_y = 1;
   if (x>=0)
   {
      angle = atan2i_coeff_1 - atan2i_coeff_1 * (x - abs_y) / (x + abs_y);
   }
   else
   {
      angle = atan2i_coeff_2 - atan2i_coeff_1 * (x + abs_y) / (abs_y - x);
   }
   if (y < 0)
      return(-angle);     // negate if in quad III or IV
   else
      return(angle);
};

#endif
