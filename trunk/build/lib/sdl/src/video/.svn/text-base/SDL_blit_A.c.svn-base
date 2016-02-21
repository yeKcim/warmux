/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2009 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

#include "SDL_video.h"
#include "SDL_blit.h"

/* fast RGB888->(A)RGB888 blending with surface alpha=128 special case */
static void BlitRGBtoRGBSurfaceAlpha128(SDL_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint32 *srcp = (Uint32 *)info->s_pixels;
	int srcskip = info->s_skip >> 2;
	Uint32 *dstp = (Uint32 *)info->d_pixels;
	int dstskip = info->d_skip >> 2;

	while(height--) {
	    DUFFS_LOOP4({
		    Uint32 s = *srcp++;
		    Uint32 d = *dstp;
		    *dstp++ = ((((s & 0x00fefefe) + (d & 0x00fefefe)) >> 1)
			       + (s & d & 0x00010101)) | 0xff000000;
	    }, width);
	    srcp += srcskip;
	    dstp += dstskip;
	}
}

/* fast RGB888->(A)RGB888 blending with surface alpha */
static void BlitRGBtoRGBSurfaceAlpha(SDL_BlitInfo *info)
{
	unsigned alpha = info->src->alpha;
	if(alpha == 128) {
		BlitRGBtoRGBSurfaceAlpha128(info);
	} else {
		int width = info->d_width;
		int height = info->d_height;
		Uint32 *srcp = (Uint32 *)info->s_pixels;
		int srcskip = info->s_skip >> 2;
		Uint32 *dstp = (Uint32 *)info->d_pixels;
		int dstskip = info->d_skip >> 2;
		Uint32 s;
		Uint32 d;
		Uint32 s1;
		Uint32 d1;

		while(height--) {
			DUFFS_LOOP_DOUBLE2({
				/* One Pixel Blend */
				s = *srcp;
				d = *dstp;
				s1 = s & 0xff00ff;
				d1 = d & 0xff00ff;
				d1 = (d1 + ((s1 - d1) * alpha >> 8))
				     & 0xff00ff;
				s &= 0xff00;
				d &= 0xff00;
				d = (d + ((s - d) * alpha >> 8)) & 0xff00;
				*dstp = d1 | d | 0xff000000;
				++srcp;
				++dstp;
			},{
			        /* Two Pixels Blend */
				s = *srcp;
				d = *dstp;
				s1 = s & 0xff00ff;
				d1 = d & 0xff00ff;
				d1 += (s1 - d1) * alpha >> 8;
				d1 &= 0xff00ff;
				     
				s = ((s & 0xff00) >> 8) | 
					((srcp[1] & 0xff00) << 8);
				d = ((d & 0xff00) >> 8) |
					((dstp[1] & 0xff00) << 8);
				d += (s - d) * alpha >> 8;
				d &= 0x00ff00ff;
				
				*dstp++ = d1 | ((d << 8) & 0xff00) | 0xff000000;
				++srcp;
				
			        s1 = *srcp;
				d1 = *dstp;
				s1 &= 0xff00ff;
				d1 &= 0xff00ff;
				d1 += (s1 - d1) * alpha >> 8;
				d1 &= 0xff00ff;
				
				*dstp = d1 | ((d >> 8) & 0xff00) | 0xff000000;
				++srcp;
				++dstp;
			}, width);
			srcp += srcskip;
			dstp += dstskip;
		}
	}
}

/* fast ARGB888->(A)RGB888 blending with pixel alpha */
static void BlitRGBtoRGBPixelAlpha(SDL_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint32 *srcp = (Uint32 *)info->s_pixels;
	int srcskip = info->s_skip >> 2;
	Uint32 *dstp = (Uint32 *)info->d_pixels;
	int dstskip = info->d_skip >> 2;

	while(height--) {
	    DUFFS_LOOP4({
		Uint32 dalpha;
		Uint32 d;
		Uint32 s1;
		Uint32 d1;
		Uint32 s = *srcp;
		Uint32 alpha = s >> 24;
		/* FIXME: Here we special-case opaque alpha since the
		   compositioning used (>>8 instead of /255) doesn't handle
		   it correctly. Also special-case alpha=0 for speed?
		   Benchmark this! */
		if(alpha) {   
		  if(alpha == SDL_ALPHA_OPAQUE) {
		    *dstp = (s & 0x00ffffff) | (*dstp & 0xff000000);
		  } else {
		    /*
		     * take out the middle component (green), and process
		     * the other two in parallel. One multiply less.
		     */
		    d = *dstp;
		    dalpha = d & 0xff000000;
		    s1 = s & 0xff00ff;
		    d1 = d & 0xff00ff;
		    d1 = (d1 + ((s1 - d1) * alpha >> 8)) & 0xff00ff;
		    s &= 0xff00;
		    d &= 0xff00;
		    d = (d + ((s - d) * alpha >> 8)) & 0xff00;
		    *dstp = d1 | d | dalpha;
		  }
		}
		++srcp;
		++dstp;
	    }, width);
	    srcp += srcskip;
	    dstp += dstskip;
	}
}

/* 16bpp special case for per-surface alpha=50%: blend 2 pixels in parallel */

/* blend a single 16 bit pixel at 50% */
#define BLEND16_50(d, s, mask)						\
	((((s & mask) + (d & mask)) >> 1) + (s & d & (~mask & 0xffff)))

/* blend two 16 bit pixels at 50% */
#define BLEND2x16_50(d, s, mask)					     \
	(((s & (mask | mask << 16)) >> 1) + ((d & (mask | mask << 16)) >> 1) \
	 + (s & d & (~(mask | mask << 16))))

static void Blit16to16SurfaceAlpha128Key(SDL_BlitInfo *info, Uint16 mask)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint16 *srcp = (Uint16 *)info->s_pixels;
	int srcskip = info->s_skip >> 1;
	Uint16 *dstp = (Uint16 *)info->d_pixels;
	int dstskip = info->d_skip >> 1;
        Uint16 ckey = info->src->colorkey;
        Uint32 ckey32 = ckey<<16|ckey;

	while(height--) {
		if(((uintptr_t)srcp ^ (uintptr_t)dstp) & 2) {
			/*
			 * Source and destination not aligned, pipeline it.
			 * This is mostly a win for big blits but no loss for
			 * small ones
			 */
			Uint32 prev_sw;
			int w = width;

			/* handle odd destination */
			if((uintptr_t)dstp & 2) {
				Uint16 s = *srcp;
                                if (s != ckey)
				  *dstp = BLEND16_50(*dstp, s, mask);
				dstp++;
				srcp++;
				w--;
			}
			srcp++;	/* srcp is now 32-bit aligned */

			/* bootstrap pipeline with first halfword */
			prev_sw = ((Uint32 *)srcp)[-1];

			while(w > 1) {
				Uint32 sw, dw, s;
				sw = *(Uint32 *)srcp;
				dw = *(Uint32 *)dstp;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
				s = (prev_sw << 16) + (sw >> 16);
#else
				s = (prev_sw >> 16) + (sw << 16);
#endif
				prev_sw = sw;
                                if (s != ckey32)
				  *(Uint32 *)dstp = BLEND2x16_50(dw, s, mask);
                                else {
                                  int i;
                                  for (i=0; i<2; i++) {
                                    Uint16 s16 = srcp[i];
                                    if (s16 != ckey)
				     dstp[i] = BLEND16_50(dstp[i], s16, mask);
                                  }
                                }
				dstp += 2;
				srcp += 2;
				w -= 2;
			}

			/* final pixel if any */
			if(w) {
				Uint16 s;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
				s = (Uint16)prev_sw;
#else
				s = (Uint16)(prev_sw >> 16);
#endif
                                if (s != ckey)
				  *dstp = BLEND16_50(*dstp, s, mask);
				srcp++;
				dstp++;
			}
			srcp += srcskip - 1;
			dstp += dstskip;
		} else {
			/* source and destination are aligned */
			int w = width;

			/* first odd pixel? */
			if((uintptr_t)srcp & 2) {
				Uint16 s = *srcp;
                                if (s != ckey)
				  *dstp = BLEND16_50(*dstp, s, mask);
				srcp++;
				dstp++;
				w--;
			}
			/* srcp and dstp are now 32-bit aligned */

			while(w > 1) {
				Uint32 sw = *(Uint32 *)srcp;
				Uint32 dw = *(Uint32 *)dstp;
                                if (sw != ckey32)
				  *(Uint32 *)dstp = BLEND2x16_50(dw, sw, mask);
                                else {
                                  int i;
                                  for (i=0; i<2; i++) {
                                    Uint16 s16 = srcp[i];
                                    if (s16 != ckey)
				     dstp[i] = BLEND16_50(dstp[i], s16, mask);
                                  }
                                }
				srcp += 2;
				dstp += 2;
				w -= 2;
			}

			/* last odd pixel? */
			if(w) {
				Uint16 s = *srcp;
                                if (s != ckey)
				  *dstp = BLEND16_50(*dstp, s, mask);
				srcp++;
				dstp++;
				w--;
			}
			srcp += srcskip;
			dstp += dstskip;
		}
	}
}

static void Blit16to16SurfaceAlpha128(SDL_BlitInfo *info, Uint16 mask)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint16 *srcp = (Uint16 *)info->s_pixels;
	int srcskip = info->s_skip >> 1;
	Uint16 *dstp = (Uint16 *)info->d_pixels;
	int dstskip = info->d_skip >> 1;

	while(height--) {
		if(((uintptr_t)srcp ^ (uintptr_t)dstp) & 2) {
			/*
			 * Source and destination not aligned, pipeline it.
			 * This is mostly a win for big blits but no loss for
			 * small ones
			 */
			Uint32 prev_sw;
			int w = width;

			/* handle odd destination */
			if((uintptr_t)dstp & 2) {
				Uint16 d = *dstp, s = *srcp;
				*dstp = BLEND16_50(d, s, mask);
				dstp++;
				srcp++;
				w--;
			}
			srcp++;	/* srcp is now 32-bit aligned */

			/* bootstrap pipeline with first halfword */
			prev_sw = ((Uint32 *)srcp)[-1];

			while(w > 1) {
				Uint32 sw, dw, s;
				sw = *(Uint32 *)srcp;
				dw = *(Uint32 *)dstp;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
				s = (prev_sw << 16) + (sw >> 16);
#else
				s = (prev_sw >> 16) + (sw << 16);
#endif
				prev_sw = sw;
				*(Uint32 *)dstp = BLEND2x16_50(dw, s, mask);
				dstp += 2;
				srcp += 2;
				w -= 2;
			}

			/* final pixel if any */
			if(w) {
				Uint16 d = *dstp, s;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
				s = (Uint16)prev_sw;
#else
				s = (Uint16)(prev_sw >> 16);
#endif
				*dstp = BLEND16_50(d, s, mask);
				srcp++;
				dstp++;
			}
			srcp += srcskip - 1;
			dstp += dstskip;
		} else {
			/* source and destination are aligned */
			int w = width;

			/* first odd pixel? */
			if((uintptr_t)srcp & 2) {
				Uint16 d = *dstp, s = *srcp;
				*dstp = BLEND16_50(d, s, mask);
				srcp++;
				dstp++;
				w--;
			}
			/* srcp and dstp are now 32-bit aligned */

			while(w > 1) {
				Uint32 sw = *(Uint32 *)srcp;
				Uint32 dw = *(Uint32 *)dstp;
				*(Uint32 *)dstp = BLEND2x16_50(dw, sw, mask);
				srcp += 2;
				dstp += 2;
				w -= 2;
			}

			/* last odd pixel? */
			if(w) {
				Uint16 d = *dstp, s = *srcp;
				*dstp = BLEND16_50(d, s, mask);
				srcp++;
				dstp++;
			}
			srcp += srcskip;
			dstp += dstskip;
		}
	}
}

/* fast RGB565->RGB565 blending with surface alpha */
static void Blit565to565SurfaceAlpha(SDL_BlitInfo *info)
{
	unsigned alpha = info->src->alpha;
	if(alpha == 128) {
		Blit16to16SurfaceAlpha128(info, 0xf7de);
	} else {
		int width = info->d_width;
		int height = info->d_height;
		Uint16 *srcp = (Uint16 *)info->s_pixels;
		int srcskip = info->s_skip >> 1;
		Uint16 *dstp = (Uint16 *)info->d_pixels;
		int dstskip = info->d_skip >> 1;
		alpha >>= 3;	/* downscale alpha to 5 bits */

		while(height--) {
			DUFFS_LOOP4({
				Uint32 s = *srcp++;
				Uint32 d = *dstp;
				/*
				 * shift out the middle component (green) to
				 * the high 16 bits, and process all three RGB
				 * components at the same time.
				 */
				s = (s | s << 16) & 0x07e0f81f;
				d = (d | d << 16) & 0x07e0f81f;
				d += (s - d) * alpha >> 5;
				d &= 0x07e0f81f;
				*dstp++ = (Uint16)(d | d >> 16);
			}, width);
			srcp += srcskip;
			dstp += dstskip;
		}
	}
}

/* fast RGB565->RGB565 blending with surface alpha */
static void Blit565to565SurfaceAlphaKey(SDL_BlitInfo *info)
{
	unsigned alpha = info->src->alpha;
	if(alpha == 128) {
		Blit16to16SurfaceAlpha128Key(info, 0xf7de);
	} else {
		int width = info->d_width;
		int height = info->d_height;
		Uint16 *srcp = (Uint16 *)info->s_pixels;
		int srcskip = info->s_skip >> 1;
		Uint16 *dstp = (Uint16 *)info->d_pixels;
                Uint32 ckey  = info->src->colorkey;
		int dstskip = info->d_skip >> 1;
		alpha >>= 3;	/* downscale alpha to 5 bits */

		while(height--) {
			DUFFS_LOOP4({
				Uint32 s = *srcp++;
                                if (s != ckey) {
				        Uint32 d = *dstp;
				        /*
				         * shift out the middle component (green) to
				         * the high 16 bits, and process all three RGB
				         * components at the same time.
				         */
				        s = (s | s << 16) & 0x07e0f81f;
				        d = (d | d << 16) & 0x07e0f81f;
				        d += (s - d) * alpha >> 5;
				        d &= 0x07e0f81f;
				        *dstp = (Uint16)(d | d >> 16);
                                }
                                dstp++;
			}, width);
			srcp += srcskip;
			dstp += dstskip;
		}
	}
}

/* fast ARGB8888->RGB565 blending with pixel alpha */
extern void scanline_t32cb16blend_arm(Uint16*, Uint32*, size_t);
static void BlitARGBto565PixelAlpha(SDL_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	const Uint32 * __restrict__ srcp = (Uint32 *)info->s_pixels;
	int srcskip = info->s_skip >> 2;
	Uint16 * __restrict__ dstp = (Uint16 *)info->d_pixels;
	int dstskip = info->d_skip >> 1;

	while(height--) {
	    DUFFS_LOOP4({
		Uint32 s = *srcp;
		unsigned alpha = s >> 27; /* downscale alpha to 5 bits */
		/* FIXME: Here we special-case opaque alpha since the
		   compositioning used (>>8 instead of /255) doesn't handle
		   it correctly. Also special-case alpha=0 for speed?
		   Benchmark this! */
		if(alpha) {   
		  if(alpha == (SDL_ALPHA_OPAQUE >> 3)) {
		    *dstp = (Uint16)((s >> 8 & 0xf800) + (s >> 5 & 0x7e0) + (s >> 3  & 0x1f));
		  } else {
		    Uint32 d = *dstp;
		    /*
		     * convert source and destination to G0RAB65565
		     * and blend all components at the same time
		     */
		    s = ((s & 0xfc00) << 11) + (s >> 8 & 0xf800)
		      + (s >> 3 & 0x1f);
		    d = (d | d << 16) & 0x07e0f81f;
		    d += (s - d) * alpha >> 5;
		    d &= 0x07e0f81f;
		    *dstp = (Uint16)(d | d >> 16);
		  }
		}
		srcp++;
		dstp++;
	    }, width);
	    srcp += srcskip;
	    dstp += dstskip;
	}
}

SDL_loblit SDL_CalculateAlphaBlit(SDL_Surface *surface, int blit_index)
{
    SDL_PixelFormat *sf = surface->format;
    SDL_PixelFormat *df = surface->map->dst->format;

    if(sf->Amask == 0) {
	if((surface->flags & SDL_SRCCOLORKEY) == SDL_SRCCOLORKEY) {
          return Blit565to565SurfaceAlphaKey;
	} else {
	    /* Per-surface alpha blits */
	    switch(df->BytesPerPixel) {
	    case 1:
		exit(1);

	    case 2:
		return Blit565to565SurfaceAlpha;

	    case 4:
		return BlitRGBtoRGBSurfaceAlpha;

	    case 3:
	    default:
		exit(1);
	    }
	}
    } else {
	/* Per-pixel alpha blits */
	switch(df->BytesPerPixel) {
	case 1:
	    exit(1);

	case 2:
            // hardcoding it
	    return BlitARGBto565PixelAlpha;

	case 4:
            // hardcoding it
            return BlitRGBtoRGBPixelAlpha;

        case 3:
	default:
	    exit(1);
	}
    }
}

