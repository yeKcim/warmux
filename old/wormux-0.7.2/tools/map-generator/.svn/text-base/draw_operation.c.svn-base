#include <stdlib.h>
#include "draw_operation.h"

draw_op_t*
draw_texture_op(SDL_Surface *pattern, Uint32 stop, Uint8 thick)
{
  draw_op_t *d   = malloc(sizeof(draw_op_t));

  // Other members are unimportant
  d->type        = DRAW_TEXTURE;
  d->pattern     = pattern;
  d->stop_color  = stop;
  d->thick       = thick;

  d->next        = NULL;

  return d;
}

draw_op_t*
draw_solidfill_op(Uint32 solid_color, Uint32 stop, Uint8 thick)
{
  draw_op_t *d   = malloc(sizeof(draw_op_t));

  // Other members are unimportant
  d->type        = DRAW_SOLID_FILL;
  d->solid_color = solid_color;
  d->stop_color  = stop;
  d->thick       = thick;

  d->next        = NULL;

  return d;
}

void
draw_op_clean(draw_op_t *start)
{
  draw_op_t *d;

  while (start)
  {
    d = start->next;
    free(start);
    start = d;
  }
}
