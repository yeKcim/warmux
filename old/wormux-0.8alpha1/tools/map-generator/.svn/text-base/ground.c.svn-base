#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <SDL_endian.h>
#include "ground.h"
#include <SDL_endian.h>

void
ground_smoothY(ground_point_t *first, int iter)
{
  assert(first);
  while (iter>0)
  {
    ground_point_t *current = first->next;
    
    while (current->next)
    {
      current->y = (current->prev->y + current->next->y + 2*current->y + 1)>>2;
      current = current->next;
    }

    iter--;
  }
}

ground_point_t*
ground_generate_valley(int w, int h, int num_points, int max_var, int bias,
                       SDL_Surface *ground, Uint32 ground_color, int thick)
{
  ground_point_t *first    = malloc(sizeof(ground_point_t));
  ground_point_t *previous = first;
  int             i;
  int             len;

  // Generate seed
  srand ( time(NULL) + first);

  // Initial point
  first->x = 0;
  first->y = h/2 + NORMRAND(h/4);
  first->first_op = draw_solidfill_op(SDL_ALPHA_TRANSPARENT, ground_color, thick);
  first->first_op->next = draw_texture_op(ground, ground_color, thick);
  first->next = first->prev = NULL;
  len = 0;

  // Generate remaining points
  //printf("Generating with: w=%i h=%i max_var=%i bias=%i\n"
  //       "(%i,%i)", w, h, max_var, bias, first->x, first->y);
  for (i=1; i<num_points; i++)
  {
    // New point
    ground_point_t *current = malloc(sizeof(ground_point_t));
    previous->next          = current;
    current->prev           = previous;
    current->next           = NULL;
    current->first_op       = draw_solidfill_op(SDL_ALPHA_TRANSPARENT, ground_color, thick);
    current->first_op->next = draw_texture_op(ground, ground_color, thick);

    // Change def.biais on some occasions
    if (!(i%32) && rand()>RAND_MAX/3)
    {
      bias = -bias;
    }
    if (len > num_points/10)
    {
      len = 0;
      bias = -bias;
      previous->y = h;
    }

    // Compute new coordinates
    current->x = SCALE(i, w, num_points-1);
    current->y = previous->y + NORMRAND(max_var) - bias;

    if (current->y < 32)
    {
      current->y = 32;
    }
    else if (current->y>h)
    {
      len++;
    }
    //printf(" (%i,%i)\n", current->x, current->y);

    // Next
    previous = current;
  }
  //printf("\n");

  return first;
}

void
ground_point_clean(ground_point_t *first)
{
  ground_point_t *tmp;

  while (first)
  {
    draw_op_clean(first->first_op);
    tmp = first->next;
    free(first);
    first = tmp;
  }
}
