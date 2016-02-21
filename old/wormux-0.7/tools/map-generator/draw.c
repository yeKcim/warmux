#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "draw_operation.h"
#include "draw.h"
#include <SDL_gfxPrimitives.h>

#define RGB_LIST(a) (a), ((a)>>8) & 0xFF, ((a)>>16) & 0xFF, ((a)>>24) & 0xFF

int
draw_line(SDL_Surface *img, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, 
          Uint8 r, Uint8 g, Uint8 b, Uint8 thick)
{
  int i;
  
  for (i=-thick; i<thick+1; i++)
  {
    lineRGBA(img, x1, y1+i, x2, y2+i, r, g, b, 255);
  }

  return 0;
}

void
draw_line_ground(SDL_Surface *img, const ground_point_t* first,
                 unsigned int num_points)
{
  int                   i;
  const ground_point_t *g   = first->next;
  Uint32                col = first->first_op->stop_color;

  // At list first point
  printf("Drawing: (%i,%i)", first->x, first->y);
  i = 1;

  // For all segments
  while (i<num_points && g!=NULL)
  {
    // If one of the points is above the bottom, draw segment
    if (g->y<img->h || g->prev->y<img->h)
    {
      printf(" -> (%i,%i)", g->x, g->y);
      lineRGBA(img, g->prev->x, g->prev->y, g->x, g->y, RGB_LIST(col));
    }
    else // All the above can be deleted when not in debug
    {
      // Regular point in the skipped zone
      if (g->y>=img->h && g->prev->y>=img->h)
        printf(" (%i,%i)", g->x, g->y);
      else
      {
        // If next is above, end of skipped zone
        if (g->next->y<img->h)
          printf(" END\n");
        // Otherwise, start of skipped zone
        else
          printf("Skipping:");
      }
    }

    // Next line
    i++;
    g = g->next;
  }

  printf("\nDone\n");
}

void
draw_bezier_ground(SDL_Surface *img, const ground_point_t* first,
                   unsigned int num_points)
{
  int                   i;
  int                   len;
  const ground_point_t *g   = first->next;
  Sint16               *X   = (Sint16 *)malloc(num_points*2);
  Sint16               *Y   = (Sint16 *)malloc(num_points*2);
  Uint32                col = first->first_op->stop_color;

  // At list first point
  printf("Drawing: (%i,%i)", first->x, first->y);
  X[0] = first->x;
  Y[0] = first->y;
  len = 1;
  i = 1;
 
  // For all segments
  while (i<num_points && g!=NULL)
  {
    // If one of the points is above the bottom, add segment
    if (g->y<img->h || g->prev->y<img->h)
    {
      printf(" -> (%i,%i)", g->x, g->y);
      X[len] = g->x;
      Y[len] = g->y;
      len++;
    }
    // Otherwise, render current segments
    else
    {
      // Draw any pending line
      if (len > 0)
      {
        bezierRGBA(img, X, Y, len, len, RGB_LIST(col));
        printf(" END\nSkipping: ");
      }

      // Reset segment
      len = 0;

      // If next is above, add current one
      if (g->next->y<img->h)
      {
        printf("\nDrawing: (%i,%i)", g->x, g->y);
        X[0] = g->x;
        Y[0] = g->y;
        len++;
      }
      else
        printf(" (%i,%i)", g->x, g->y);
    }
    i++;
    g = g->next;
  }

  // Draw ultimate Bezier line if needed
  if (len>1)
  {
    printf(" END\n");
    bezierRGBA(img, X, Y, len, len, RGB_LIST(col));
  }
  free(X);
  free(Y);
  printf("Done\n");
}

void
draw_sprites(SDL_Surface *img, const ground_point_t *first, int num_points,
             SDL_Surface *surf, int surf_gap,
             SDL_Surface *ground, int ground_gap)
{
  int                   surf_occur   = surf_gap;
  int                   ground_occur = ground_gap;
  const ground_point_t *g            = first->next;
  int                   i            = 0;

  while (i<num_points && g)
  {
    // Only insert sprites where there is ground
    if (g->prev->y<img->h || g->y<img->h)
    {
      SDL_Rect dst;

      //printf("Segment (%i,%i)->(%i,%i):", g->prev->x, g->prev->y, g->x, g->y);

      // Check whether to put surface sprite
      if (rand()>RAND_MAX/2)
      {
        //printf(" surface");
        // Check if the request gap between 2 sprites has been reached
        if (surf_occur)
        {
          surf_occur--;
        }
        else
        {
          dst.x = (g->x + g->prev->x + 1 - surf->w)>>1;
          dst.y = ((g->y + g->prev->y + 1)>>1) - surf->h;
          
          SDL_BlitSurface(surf, NULL, img, &dst);
          surf_occur = surf_gap;
          g = g->next;
          continue;
        }
      }

      // Check whether to put ground sprite
      if (rand()>RAND_MAX/2)
      {
        // Where to start looking
        int pos = (3*ground->h + g->y + g->prev->y + 1)>>1;

        //printf(" underground");
        // Check if the request gap between 2 sprites has been reached
        if (ground_occur)
        {
          ground_occur--;
        }
        // Make sure there's enough place to store into the ground 
        else if (img->h-pos > ground->h)
        {
          dst.y = pos + abs(NORMRAND(img->h-pos-ground->h));
          dst.x = (g->x + g->prev->x + 1 - ground->w)>>1;
          SDL_BlitSurface(ground, NULL, img, &dst);
          ground_occur = ground_gap;
          g = g->next;
          continue;
        }
      }
      //printf("\n");
    }

    // Next segment
    g = g->next;
  }
}

void
draw_vertical_fill(SDL_Surface *img, const draw_op_t *start,
                   Sint16 x1, Sint16 x2)
{
  int              y;
  int              j;
  int              x;
  const draw_op_t *current;
  Uint32          *out_ptr;

#define MOVE_REL_LINE(a)                                        \
  out_ptr = (Uint32*)( ((Uint8*)out_ptr) + (a)*img->pitch )

  for (x=x1; x<x2; x++)
  {
    current = start;
    y       = 0;
    out_ptr = ((Uint32 *)img->pixels) + x;

    // Loop on drawing operations untill bottom of image
    while (current && y<img->h)
    {
      // Loop untill change of drawing mode
      //printf("Stop color: %08lX\n", current->stop_color);
      while (y<img->h && *out_ptr!=current->stop_color)
      {
        //if (*out_ptr) printf("Color: %08lX\n", *out_ptr);
        switch (current->type)
        {
        case DRAW_TEXTURE:
          {
            SDL_Surface *ptn = current->pattern;
            *out_ptr = ((Uint32 *)ptn->pixels)[(x%ptn->w) + (y%ptn->h)*ptn->w];
          }
          break;
        case DRAW_SOLID_FILL:
          *out_ptr = current->solid_color;
          break;
        default: assert(0);
        }

        y++;
        MOVE_REL_LINE(1);
      }

      // Thickness below the limit
      if (y<img->h)
      {
        MOVE_REL_LINE(-current->thick);
        for (j=y-current->thick; j<y; j++)
        {
          *out_ptr = current->stop_color;
          MOVE_REL_LINE(1);
        }
      }

      // Skip all of limit
      while (y<img->h && *((Uint32*)out_ptr) == current->stop_color)
      {
        y++;
        MOVE_REL_LINE(1);
      }

      // Thickness below the limit
      if (y<img->h)
      {
        for (j=y; j<y+current->thick && j<img->h; j++)
        {
          *out_ptr = current->stop_color;
          MOVE_REL_LINE(1);
        }
        y = j;
      }

      // Change drawing operation
      current = current->next;
    }
  }
}

void
draw_fill(SDL_Surface *img, const ground_point_t *g)
{
  g = g->next;
  int i = 0;

  while (g)
  {
    //printf("Filling %i->%i\n", g->prev->x, g->x);
    draw_vertical_fill(img, g->prev->first_op, g->prev->x, g->x);
    g = g->next;
  }
}
