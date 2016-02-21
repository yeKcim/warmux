#ifndef __GROUND_H__
#define __GROUND_H__

#include <SDL.h>
#include "draw_operation.h"

typedef struct _gp
{
  Sint16      x;
  Sint16      y;
  draw_op_t  *first_op;

  struct _gp *next;
  struct _gp *prev;

} ground_point_t;

void ground_smoothY(ground_point_t *first, int iter);

ground_point_t* ground_generate_valley(int w, int h, int num_points, int max_var, int bias,
                                       SDL_Surface *ground, Uint32 ground_color, int thick);
void ground_point_clean(ground_point_t *first);

#endif // __GROUND_H__
