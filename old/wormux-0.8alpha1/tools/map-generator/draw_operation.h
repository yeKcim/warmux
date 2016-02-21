#ifndef __DRAW_OPERATION_H__
#define __DRAW_OPERATION_H__

#include <math.h>
#include <SDL.h>

// Between -a and a
#define NORMRAND(a) (Uint32)floor( (a) * (0.5f - rand()*1.0f/RAND_MAX) )
#define SCALE(a, b, c) ( ((a)*(b) + ((c)/2)) / (c) )

typedef enum
{
  DRAW_TEXTURE,
  DRAW_SOLID_FILL
} draw_mode_t;

typedef struct _do
{
  // The drawing operation
  draw_mode_t  type;       // Whether solid or texture
  SDL_Surface *pattern;    // The texture for fill
  Uint32       solid_color;// The color for fill

  // Limit of the drawing operation
  Uint32       stop_color; // On which color to stop drawing
  Uint8        thick;      // Additional thickness around it

  struct _do  *next;       // Next operation
} draw_op_t;

draw_op_t* draw_texture_op(SDL_Surface *pattern, Uint32 stop, Uint8 thick);
draw_op_t* draw_solidfill_op(Uint32 solid_color, Uint32 stop, Uint8 thick);
void       draw_op_clean(draw_op_t *start);

#endif //__DRAW_OPERATION_H__
