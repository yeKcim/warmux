#include <stdlib.h>
#include <assert.h>
#include <SDL_image.h>
#include <png.h>
#include "IMG_Operations.h"

int
IMG_SavePNG32(SDL_Surface* img, const char *name)
{
  int          err      = 1;
  FILE        *f        = NULL;
  png_structp  png_ptr  = NULL;
  png_infop    info_ptr = NULL;
  int          h        = img->h;
  Uint8       *ptr      = img->pixels;

  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) goto out;

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) goto out;

  f = fopen(name, "wb");
  if (!f) goto out;
  png_init_io(png_ptr, f);
  png_set_IHDR(png_ptr, info_ptr, img->w, img->h, 8,
               PNG_COLOR_TYPE_RGB_ALPHA,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);
  //png_set_filter(png_ptr, 0, PNG_FILTER_PAETH | PNG_FILTER_VALUE_PAETH);
  png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

  // Write lines
  png_write_info(png_ptr, info_ptr);
  while (h)
  {
    png_write_row(png_ptr, ptr);
    ptr += img->pitch;
    h--;
  }
  png_write_flush(png_ptr);
  png_write_end(png_ptr, info_ptr);

 out:
  if (png_ptr) png_destroy_write_struct(&png_ptr, NULL);
  return err;
}

int
IMG_SavePPM24(SDL_Surface* img, const char *name)
{
  FILE  *f;
  int    h = img->h;
  int    x;
  Uint8 *p = img-> pixels;
  Uint8 *tmp;
  Uint8  bpp = img->format->BytesPerPixel;

  f = fopen(name, "wb");
  if (!f) return 0;
  fprintf(f, "P6\n%i %i\n255\n", img->w, img->h);
  
  // Special case
  if (img->w*3 == img->pitch)
  {
    fwrite(f, img->pitch, img->h, img->pixels);
    fclose(f);
    return 1;
  }

  // Alloc temp buffer
  tmp = (Uint8*)malloc(img->w*3);
  if (!tmp)
  {
    fclose(f);
    remove(name);
    return 0;
  }

  // Copy lines
  while (h)
  {
    Uint8 *ptmp = tmp;
    for (x=0; x<img->w; x++, ptmp+=3, p+=bpp)
    {
      ptmp[0] = p[0];
      ptmp[1] = p[1];
      ptmp[2] = p[2];
    }
    fwrite(tmp, img->w, 3, f);
    p += img->pitch - img->w*bpp;
    h--;
  }

  fclose(f);
  return 0;
}


SDL_Surface*
IMG_LoadWithFormat(const char* name, SDL_PixelFormat* fmt)
{
  SDL_Surface *tmp;
  
  tmp = IMG_Load(name);
  if (!tmp)
  {
    fprintf(stderr, "Couldn't load '%s'\n", name);
    exit(1);
  }
  if (tmp->format->BytesPerPixel != fmt->BytesPerPixel)
  {
    SDL_Surface *new = SDL_ConvertSurface(tmp, fmt, SDL_SWSURFACE);
    SDL_FreeSurface(tmp);
    assert(new);
    return new;
  }

  return tmp;
}

SDL_Surface*
IMG_LoadSprite(const char* name, SDL_PixelFormat* fmt)
{
  SDL_Surface *tmp = IMG_LoadWithFormat(name, fmt);
  SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL,
                  SDL_MapRGBA(fmt, 255, 255, 255, SDL_ALPHA_TRANSPARENT));
  SDL_SetAlpha(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, 0);
  return tmp;
}
