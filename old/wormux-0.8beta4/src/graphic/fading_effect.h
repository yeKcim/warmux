/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A ARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU GeneralPublic License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * fading effect largely copy/paste from SDL_Gfx
 *****************************************************************************/

#ifndef FADING_EFFECT_H
#define FADING_EFFECT_H

#include <SDL_gfxPrimitives.h>
#include <SDL.h>

/* ----- Defines for pixel clipping tests */

#define clip_xmin(surface) surface->clip_rect.x
#define clip_xmax(surface) surface->clip_rect.x+surface->clip_rect.w-1
#define clip_ymin(surface) surface->clip_rect.y
#define clip_ymax(surface) surface->clip_rect.y+surface->clip_rect.h-1

/* AA Line */

#define AAlevels 256
#define AAbits 8

#define CLIP_LEFT_EDGE   0x1
#define CLIP_RIGHT_EDGE  0x2
#define CLIP_BOTTOM_EDGE 0x4
#define CLIP_TOP_EDGE    0x8
#define CLIP_INSIDE(a)   (!a)
#define CLIP_REJECT(a,b) (a&b)
#define CLIP_ACCEPT(a,b) (!(a|b))

static int clipEncode(Sint16 x, Sint16 y, Sint16 left, Sint16 top, Sint16 right, Sint16 bottom)
{
    int code = 0;

    if (x < left) {
        code |= CLIP_LEFT_EDGE;
    } else if (x > right) {
        code |= CLIP_RIGHT_EDGE;
    }
    if (y < top) {
        code |= CLIP_TOP_EDGE;
    } else if (y > bottom) {
        code |= CLIP_BOTTOM_EDGE;
    }
    return code;
}

static int clipLine(SDL_Surface * dst, Sint16 * x1, Sint16 * y1, Sint16 * x2, Sint16 * y2)
{
    Sint16 left, right, top, bottom;
    int code1, code2;
    int draw = 0;
    Sint16 swaptmp;
    float m;

    /*
     * Get clipping boundary
     */
    left = dst->clip_rect.x;
    right = dst->clip_rect.x + dst->clip_rect.w - 1;
    top = dst->clip_rect.y;
    bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

    while (1) {
        code1 = clipEncode(*x1, *y1, left, top, right, bottom);
        code2 = clipEncode(*x2, *y2, left, top, right, bottom);
        if (CLIP_ACCEPT(code1, code2)) {
            draw = 1;
            break;
        } else if (CLIP_REJECT(code1, code2))
            break;
        else {
            if (CLIP_INSIDE(code1)) {
                swaptmp = *x2;
                *x2 = *x1;
                *x1 = swaptmp;
                swaptmp = *y2;
                *y2 = *y1;
                *y1 = swaptmp;
                swaptmp = code2;
                code2 = code1;
                code1 = swaptmp;
            }
            if (*x2 != *x1) {
                m = (*y2 - *y1) / (float) (*x2 - *x1);
            } else {
                m = 1.0f;
            }
            if (code1 & CLIP_LEFT_EDGE) {
                *y1 += (Sint16) ((left - *x1) * m);
                *x1 = left;
            } else if (code1 & CLIP_RIGHT_EDGE) {
                *y1 += (Sint16) ((right - *x1) * m);
                *x1 = right;
            } else if (code1 & CLIP_BOTTOM_EDGE) {
                if (*x2 != *x1) {
                    *x1 += (Sint16) ((bottom - *y1) / m);
                }
                *y1 = bottom;
            } else if (code1 & CLIP_TOP_EDGE) {
                if (*x2 != *x1) {
                    *x1 += (Sint16) ((top - *y1) / m);
                }
                *y1 = top;
            }
        }
    }

    return draw;
}

int _putPixelAlpha(SDL_Surface * surface, Sint16 x, Sint16 y, Uint32 color, Uint8 alpha)
{
    Uint32 Rmask = surface->format->Rmask, Gmask =
        surface->format->Gmask, Bmask = surface->format->Bmask, Amask = surface->format->Amask;
    Uint32 R, G, B, A = 0;

    if (x >= clip_xmin(surface) && x <= clip_xmax(surface)
        && y >= clip_ymin(surface) && y <= clip_ymax(surface)) {

        switch (surface->format->BytesPerPixel) {
        case 1:{                /* Assuming 8-bpp */
                if (alpha == 255) {
                    *((Uint8 *) surface->pixels + y * surface->pitch + x) = color;
                } else {
                    Uint8 *pixel = (Uint8 *) surface->pixels + y * surface->pitch + x;

                    Uint8 dR = surface->format->palette->colors[*pixel].r;
                    Uint8 dG = surface->format->palette->colors[*pixel].g;
                    Uint8 dB = surface->format->palette->colors[*pixel].b;
                    Uint8 sR = surface->format->palette->colors[color].r;
                    Uint8 sG = surface->format->palette->colors[color].g;
                    Uint8 sB = surface->format->palette->colors[color].b;

                    dR = dR + ((sR - dR) * alpha >> 8);
                    dG = dG + ((sG - dG) * alpha >> 8);
                    dB = dB + ((sB - dB) * alpha >> 8);

                    *pixel = SDL_MapRGB(surface->format, dR, dG, dB);
                }
            }
            break;

        case 2:{                /* Probably 15-bpp or 16-bpp */
                if (alpha == 255) {
                    *((Uint16 *) surface->pixels + y * surface->pitch / 2 + x) = color;
                } else {
                    Uint16 *pixel = (Uint16 *) surface->pixels + y * surface->pitch / 2 + x;
                    Uint32 dc = *pixel;

                    R = ((dc & Rmask) + (((color & Rmask) - (dc & Rmask)) * alpha >> 8)) & Rmask;
                    G = ((dc & Gmask) + (((color & Gmask) - (dc & Gmask)) * alpha >> 8)) & Gmask;
                    B = ((dc & Bmask) + (((color & Bmask) - (dc & Bmask)) * alpha >> 8)) & Bmask;
                    if (Amask)
                        A = ((dc & Amask) + (((color & Amask) - (dc & Amask)) * alpha >> 8)) & Amask;

                    *pixel = R | G | B | A;
                }
            }
            break;

        case 3:{                /* Slow 24-bpp mode, usually not used */
                Uint8 *pix = (Uint8 *) surface->pixels + y * surface->pitch + x * 3;
                Uint8 rshift8 = surface->format->Rshift / 8;
                Uint8 gshift8 = surface->format->Gshift / 8;
                Uint8 bshift8 = surface->format->Bshift / 8;
                Uint8 ashift8 = surface->format->Ashift / 8;


                if (alpha == 255) {
                    *(pix + rshift8) = color >> surface->format->Rshift;
                    *(pix + gshift8) = color >> surface->format->Gshift;
                    *(pix + bshift8) = color >> surface->format->Bshift;
                    *(pix + ashift8) = color >> surface->format->Ashift;
                } else {
                    Uint8 dR, dG, dB, dA = 0;
                    Uint8 sR, sG, sB, sA = 0;

                    pix = (Uint8 *) surface->pixels + y * surface->pitch + x * 3;

                    dR = *((pix) + rshift8);
                    dG = *((pix) + gshift8);
                    dB = *((pix) + bshift8);
                    dA = *((pix) + ashift8);

                    sR = (color >> surface->format->Rshift) & 0xff;
                    sG = (color >> surface->format->Gshift) & 0xff;
                    sB = (color >> surface->format->Bshift) & 0xff;
                    sA = (color >> surface->format->Ashift) & 0xff;

                    dR = dR + ((sR - dR) * alpha >> 8);
                    dG = dG + ((sG - dG) * alpha >> 8);
                    dB = dB + ((sB - dB) * alpha >> 8);
                    dA = dA + ((sA - dA) * alpha >> 8);

                    *((pix) + rshift8) = dR;
                    *((pix) + gshift8) = dG;
                    *((pix) + bshift8) = dB;
                    *((pix) + ashift8) = dA;
                }
            }
            break;

        case 4:{                /* Probably 32-bpp */
                if (alpha == 255) {
                    *((Uint32 *) surface->pixels + y * surface->pitch / 4 + x) = color;
                } else {
                    Uint32 *pixel = (Uint32 *) surface->pixels + y * surface->pitch / 4 + x;
                    Uint32 dc = *pixel;

                    R = ((dc & Rmask) + (((color & Rmask) - (dc & Rmask)) * alpha >> 8)) & Rmask;
                    G = ((dc & Gmask) + (((color & Gmask) - (dc & Gmask)) * alpha >> 8)) & Gmask;
                    B = ((dc & Bmask) + (((color & Bmask) - (dc & Bmask)) * alpha >> 8)) & Bmask;
                    if (Amask)
                        A = ((dc & Amask) + (((color & Amask) - (dc & Amask)) * alpha >> 8)) & Amask;

                    *pixel = R | G | B | A;
                }
            }
            break;
        }
    }

    return (0);
}

int pixelColorNolock(SDL_Surface * dst, Sint16 x, Sint16 y, Uint32 color)
{
    Uint8 alpha;
    Uint32 mcolor;
    int result = 0;

    /*
     * Setup color
     */
    alpha = color & 0x000000ff;
    mcolor =
        SDL_MapRGBA(dst->format, (color & 0xff000000) >> 24,
                    (color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, alpha);

    /*
     * Draw
     */
    result = _putPixelAlpha(dst, x, y, mcolor, alpha);

    return (result);
}

int pixelColorWeightNolock(SDL_Surface * dst, Sint16 x, Sint16 y, Uint32 color, Uint32 weight)
{
    Uint32 a;

    /*
     * Get alpha
     */
    a = (color & (Uint32) 0x000000ff);

    /*
     * Modify Alpha by weight
     */
    a = ((a * weight) >> 8);

    return (pixelColorNolock(dst, x, y, (color & (Uint32) 0xffffff00) | (Uint32) a));
}

Uint32 interpolateColor(Uint32 color1, Uint32 color2, float step)
{
  int c1, c2, c3, c4;
  c1 = color1 & 0xFF000000;
  c1 = (c1 + (int)((((int)color2 & 0xFF000000) - c1) * step)) & 0xFF000000;

  c2 = color1 & 0x00FF0000;
  c2 = (c2 + (int)((((int)color2 & 0x00FF0000) - c2) * step)) & 0x00FF0000;

  c3 = color1 & 0x0000FF00;
  c3 = (c3 + (int)((((int)color2 & 0x0000FF00) - c3) * step)) & 0x0000FF00;

  c4 = color1 & 0x000000FF;
  c4 = (c4 + (int)((((int)color2 & 0x000000FF) - c4) * step)) & 0x000000FF;
  return (Uint32)(c1 | c2 | c3 | c4);
}

int aafadingLineColorInt(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color1,
                         Uint32 color2, int draw_endpoint)
{
    Sint32 xx0, yy0, xx1, yy1;
    int result;
    int dx, dy, tmp, xdir;
    Uint32 color = color1;
    float step;

    if (y1 == y2) {
        /* Horizontal line */
        dx = x2-x1;
        xdir = 1;
        if (dx < 0) {
            dx = -dx;
            xdir = -1;
        }
        step = dx;
        while (--dx) {
            color = interpolateColor(color2, color1, (float)dx / step);
            result |= pixelColorNolock(dst, x1, y1, color);
            x1 += xdir;
        }
    }
    if (y1 > y2) {
        color = color1;
        color1 = color2;
        color2 = color;
    }

    /*
     * Clip line and test if we have to draw
     */
    if (!(clipLine(dst, &x1, &y1, &x2, &y2))) {
        return (0);
    }

    /*
     * Keep on working with 32bit numbers
     */
    xx0 = x1;
    yy0 = y1;
    xx1 = x2;
    yy1 = y2;

    /*
     * Reorder points if required
     */
    if (yy0 > yy1) {
        tmp = yy0;
        yy0 = yy1;
        yy1 = tmp;
        tmp = xx0;
        xx0 = xx1;
        xx1 = tmp;
    }

    /*
     * Calculate distance
     */
    dx = xx1 - xx0;
    dy = yy1 - yy0;

    /*
     * Adjust for negative dx and set xdir
     */
    if (dx >= 0) {
        xdir = 1;
    } else {
        xdir = -1;
        dx = (-dx);
    }

    /* Lock surface */
    if (SDL_MUSTLOCK(dst)) {
        if (SDL_LockSurface(dst) < 0) {
            return (-1);
        }
    }

    /*
     * Draw the initial pixel in the foreground color
     */
    result = pixelColorNolock(dst, x1, y1, color1);

    /*
     * Check for special cases
     */
    if (dx == 0) {
        /* Vertical line */
        step = dy;
        while (--dy) {
            color = interpolateColor(color2, color1, (float)dy / step);
            result |= pixelColorNolock(dst, x1, ++yy0, color);
        }
    } else if (dy == 0) {
    } else if (dx == dy) {
        /* Diagonal line */
        step = dx;
        while (--dx) {
            color = interpolateColor(color2, color1, (float)dx / step);
            result |= pixelColorNolock(dst, xx0, ++yy0, color);
            xx0 += xdir;
        }
    }
    else {
        Uint32 erracc = 0; /* Zero accumulator */
        Uint32 intshift = 32 - AAbits; /* # of bits by which to shift erracc to get intensity level */
        Uint32 erradj;
        Uint32 erracctmp, wgt;

        /*
         * x-major or y-major?
         */
        if (dy > dx) {

            /*
             * y-major.  Calculate 16-bit fixed point fractional part of a
             * pixel that X advances every time Y advances 1 pixel, truncating
             * the result so that we won't overrun the endpoint along the
             * X axis
             */
            /*
             * Not-so-portable version:
             * erradj = ((Uint64)dx << 32) / (Uint64)dy;
             */
            erradj = ((dx << 16) / dy) << 16;

            /*
             * draw all pixels other than the first and last
             */
            int x0pxdir = xx0 + xdir;
            step = dy;
            while (--dy) {
                color = interpolateColor(color2, color1, (float)dy / step);
                erracctmp = erracc;
                erracc += erradj;
                if (erracc <= erracctmp) {
                    /*
                     * rollover in error accumulator, x coord advances
                     */
                    xx0 = x0pxdir;
                    x0pxdir += xdir;
                }
                yy0++;              /* y-major so always advance Y */

                /*
                 * the AAbits most significant bits of erracc give us the
                 * intensity weighting for this pixel, and the complement of
                 * the weighting for the paired pixel.
                 */
                wgt = (erracc >> intshift) & 255;
                result |= pixelColorWeightNolock (dst, xx0, yy0, color, 255 - wgt);
                result |= pixelColorWeightNolock (dst, x0pxdir, yy0, color, wgt);
            }

        } else {

            /*
             * x-major line.  Calculate 16-bit fixed-point fractional part of
             * a pixel that Y advances each time X advances 1 pixel truncating
             * the result so that we won't overrun the endpoint along the
             * X axis.
             */
            /*
             * Not-so-portable version:
             * erradj = ((Uint64)dy << 32) / (Uint64)dx;
             */
            erradj = ((dy << 16) / dx) << 16;

            /*
             * draw all pixels other than the first and last
             */
            int y0p1 = yy0 + 1;
            step = dx;
            while (--dx) {
                color = interpolateColor(color2, color1, (float)dx / step);
                erracctmp = erracc;
                erracc += erradj;
                if (erracc <= erracctmp) {
                    /*
                     * Accumulator turned over, advance y
                     */
                    yy0 = y0p1;
                    y0p1++;
                }
                xx0 += xdir;        /* x-major so always advance X */
                /*
                 * the AAbits most significant bits of erracc give us the
                 * intensity weighting for this pixel, and the complement of
                 * the weighting for the paired pixel.
                 */
                wgt = (erracc >> intshift) & 255;
                result |= pixelColorWeightNolock (dst, xx0, yy0, color, 255 - wgt);
                result |= pixelColorWeightNolock (dst, xx0, y0p1, color, wgt);
            }
        }
    }
    /*
     * Do we have to draw the endpoint
     */
    if (draw_endpoint) {
        /*
         * Draw final pixel, always exactly intersected by the line and doesn't
         * need to be weighted.
         */
        result |= pixelColorNolock (dst, x2, y2, color2);
    }

    /* Unlock surface */
    if (SDL_MUSTLOCK(dst)) {
        SDL_UnlockSurface(dst);
    }

    return (result);
}

int aafadingLineColor(SDL_Surface * dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color1, Uint32 color2)
{
    return (aafadingLineColorInt(dst, x1, y1, x2, y2, color1, color2, 1));
}

#endif /* FADING_EFFECT_H */
