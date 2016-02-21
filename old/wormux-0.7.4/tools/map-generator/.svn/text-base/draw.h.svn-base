#ifndef __DRAW_H__
#define __DRAW_H__

#include <SDL.h>
#include "ground.h" // Ugly to include it

int        draw_line(SDL_Surface *img,
                     Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2,
                     Uint8 r, Uint8 g, Uint8 b, Uint8 thick);
void       draw_vertical_fill(SDL_Surface *img, const draw_op_t *start,
                              Sint16 x1, Sint16 x2);
void       draw_fill(SDL_Surface *img, const ground_point_t *g);
void       draw_line_ground(SDL_Surface *img, const ground_point_t* first,
                            unsigned int num_points);
void       draw_bezier_ground(SDL_Surface *img, const ground_point_t *first,
                              unsigned int num_points);
void       draw_sprites(SDL_Surface *img, const ground_point_t* first,
                        int num_points,
                        SDL_Surface *surf, int surf_gap,
                        SDL_Surface *ground, int ground_gap);
#endif //__DRAW_H__
