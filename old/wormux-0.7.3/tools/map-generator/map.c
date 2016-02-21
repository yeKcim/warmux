#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <SDL.h>
#undef main // Defined in some situations by SDL under windows

#include "IMG_Operations.h"
#include "ground.h"
#include "draw.h"


typedef struct
{
  int         width;
  int         height;
  int         num_points;
  int         max_var;
  int         bias;
  int         surf_gap;
  int         ground_gap;
  int         png;

  const char *out_name;
  const char *tex_name;
  const char *surf_name;
  const char *ground_name;
} params_t;

void
help(const params_t* values)
{
  printf("Wormux map generator\n"
         "Arguments are (default value in parenthesis):\n"
         "  -w <int>  Width of the map (%i)\n"
         "  -h <int>  Height of the map (%i)\n"
         "  -n <int>  Number of points the ground is made of (%i)\n"
         "  -m <int>  Maximum variation allowed between points (%i)\n"
         "  -b <int>  Bias to use (%i)\n"
         "  -o <img>  Name for output RGBA PNG (%s)\n"
         "  -p <img>  Name for output RGB24 PPM (%s)\n"
         "  -t <img>  Texture for the ground (%s)\n"
         "  -s <img>  Sprite for the ground surface (%s)\n"
         "  -g <img>  Sprite for the underground (%s)\n",
         values->width, values->height, values->num_points, values->max_var,
         values->bias, values->out_name, values->out_name,
         values->tex_name, values->surf_name, values->ground_name);
}

int
parse(params_t* values, int argc, char *argv[])
{
  int i;
  params_t local;

  memcpy(&local, values, sizeof(local));

  for (i=1; i<argc; i++)
  {
    if (argv[i][0] == '-')
    {
      switch(argv[i][1])
      {
      case 'w': local.width = strtol(argv[++i], NULL, 10); break;
      case 'h': local.height = strtol(argv[++i], NULL, 10); break;
      case 'n': local.num_points = strtol(argv[++i], NULL, 10); break;
      case 'm': local.max_var = strtol(argv[++i], NULL, 10); break;
      case 'b': local.bias = strtol(argv[++i], NULL, 10); break;
      case 'o': local.png = 1; local.out_name = argv[++i]; break;
      case 't': local.png = 0; local.tex_name = argv[++i]; break;
      case 's': local.surf_name = argv[++i]; break;
      case 'g': local.ground_name = argv[++i]; break;
      case '-': help(values); exit(0);
      default: fprintf(stderr, "Unknown option '%s'\n", argv[i]);
      }
    }
    else
    {
      fprintf(stderr, "Unknown parameter '%s'\n", argv[i]);
    }
  }

  /* Check args validity */
  if (values->width < 2)
  {
    fprintf(stderr, "Bad output width: %i\n", values->width);
    return -1;
  }
  if (values->height < 2)
  {
    fprintf(stderr, "Bad output width: %i\n", values->height);
    return -1;
  }
  if (values->num_points < 3 || values->num_points>values->width/4)
  {
    fprintf(stderr, "Bad output width: %i\n", values->num_points);
    return -1;
  }
  memcpy(values, &local, sizeof(local));
  return 0;
}

int main(int argc, char *argv[])
{
  SDL_Surface    *img;
  SDL_Surface    *pattern;
  SDL_Surface    *surf;
  SDL_Surface    *under;
  ground_point_t *points;
  params_t        def = { 1024, 768, 40, 64, -12, 3, 3, 1, "out.png",
                          "texture.png", "cactus.png", "skull.png" };
  Uint32          brown;

  if (parse(&def, argc, argv) < 0)
  {
    help(&def);
    exit(1);
  }

  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
    return 0;
  }

  // Create image
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  img = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, def.width, def.height,
                             32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
  img = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, def.width, def.height,
                             32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif
  assert(img);
  
  // Have a brownish color ready
  brown = SDL_MapRGBA(img->format, 0x87, 0x6E, 0x29, SDL_ALPHA_OPAQUE); //BGR

  // Load texture as 32bits
  pattern = IMG_LoadWithFormat(def.tex_name, img->format);
  // Load surface sprite
  surf = IMG_LoadSprite(def.surf_name, img->format);
  // Load ground sprite
  under = IMG_LoadSprite(def.ground_name, img->format);

  // Generate ground points
  printf("Test: %i %i %i %i\n", NORMRAND(img->h/2), NORMRAND(img->h/2),
         NORMRAND(img->h/2), NORMRAND(img->h/2));
  points = ground_generate_valley(def.width, def.height, def.num_points,
                                  def.max_var, def.bias, pattern, brown, 2);

  // Render ground
#if 0 // More crash-prone
  draw_bezier_ground(img, points, def.num_points);
#else
  ground_smoothY(points, 2);
  draw_line_ground(img, points, def.num_points);
#endif

  // Now place sprites
  draw_fill(img, points);
#if 1
  draw_sprites(img, points, def.num_points,
               surf, def.surf_gap, under, def.ground_gap);
#endif

  // Save
  if (def.png) IMG_SavePNG32(img, "out.png");
  else         IMG_SavePPM24(img, "out.ppm");
  
 out:
  if (img) SDL_FreeSurface(img);
  if (pattern) SDL_FreeSurface(pattern);
  if (surf) SDL_FreeSurface(surf);
  if (under) SDL_FreeSurface(under);
  if (points) ground_point_clean(points);

  SDL_Quit();
  return 0;
}
