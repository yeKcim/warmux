/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *****************************************************************************/

#include <iostream>
#include <SDL.h>
#include <SDL_endian.h>
#include <WARMUX_point.h>
#include "map/camera.h"
#include "map/tile.h"
#include "map/tileitem.h"
#include "game/config.h"
#include "graphic/video.h"
#include "include/app.h"
#include "tool/math_tools.h"

// === Common to all TileItem_* except TileItem_Emtpy ==============================
TileItem_NonEmpty::TileItem_NonEmpty(uint8_t alpha_threshold)
  : TileItem()
  , m_alpha_threshold(alpha_threshold)
{
  m_empty_bitfield = new uint8_t[1<<(2*CELL_BITS-3)];
  ForceRecheck();
}

void TileItem_NonEmpty::Draw(const Point2i &pos)
{
  GetMainWindow().Blit(m_surface,
                       (pos<<CELL_BITS) - Camera::GetInstance()->GetPosition());
}

void TileItem_NonEmpty::Dig(const Point2i &center, const uint radius)
{
  uint8_t    *buf       = m_surface.GetPixels();
  const uint  line_size = m_surface.GetPitch();

  int r = radius+EXPLOSION_BORDER_SIZE;
  int y = center.y - r;
  if (y < 0)
    y = 0;
  int endy = center.y + radius + EXPLOSION_BORDER_SIZE + 1;
  if (endy >= CELL_DIM)
    endy = CELL_DIM;

  buf += y * line_size;

  //Empties each line of the tile horizontaly that are in the circle
  int maxx = 0, minx = CELL_DIM;
  for (; y < endy; buf += line_size, y++) {
    //Abscisse distance from the center of the circle to the circle
    int dac = center.y - y;

    //Darken the border of the removed ground
    int blength = uround(sqrt(Double(r*r - dac*dac)));

    if (minx>center.x-blength)
      minx = center.x-blength;
    if (maxx<center.x+blength+1)
      maxx = center.x+blength+1;

    //Nothing to empty, just darken
    if ((uint)abs(dac) > radius) {
      Darken(center.x-blength, center.x+blength, buf);
      continue;
    }

    //Zone of the line which needs to be emptied
    int length = uround(sqrt(Double(radius*radius - dac*dac)));

    // Left half of the circle
    Darken(center.x-blength, center.x-length, buf);

    // Right half of the circle
    Darken(center.x+length, center.x+blength, buf);

    Empty(center.x-length, center.x+length, buf);
  }

  if (minx < 0)
    minx = 0;
  if (maxx > CELL_DIM)
    maxx = CELL_DIM;

  y = center.y - r;
  m_start_check.SetValues(minx, y>0 ? y : 0);
  m_end_check.SetValues(maxx, endy);

  m_need_check_empty = true;
}

/** CCITT CRC16 */
uint16_t TileItem_NonEmpty::GetChecksum() const
{
  uint16_t        crc = 0xFFFF;
  const uint8_t  *ptr = m_empty_bitfield;
  int             i   = 1<<(2*CELL_BITS-3);
  // Some tiles completely empty are not deleted because of some pixel data
  // being actually displayed. The 16bpp rendering however doesn't keep them
  // as it can't differenciate such pixel data, and really delete the tiles.
  bool            empty = true;

  static const uint16_t table[] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
  };


  while (i--) {
    empty &= ((*ptr)==0xFF);
    crc = (crc<<8) ^ table[(crc>>8) ^ *(ptr++)];
  }

  return empty ? 0 : crc;
}

void TileItem_NonEmpty::CheckEmptyField()
{
  const Uint32 *ptr = (Uint32 *)m_empty_bitfield;

  m_need_check_empty = false;

  for (int i=0; i<1<<(2*CELL_BITS-3-2); i++, ptr++) {
    if (*ptr != 0xFFFFFFFF) {
      m_is_empty = false;
      return;
    }
  }
  m_is_empty = true;
}

void TileItem_NonEmpty::ForceRecheck()
{
  m_start_check.SetValues(0, 0);
  m_end_check.SetValues(CELL_DIM, CELL_DIM);
  m_need_check_empty = true;
  m_is_empty = true;
  m_need_resynch = true;
}

void TileItem_NonEmpty::ForceEmpty()
{
  memset(m_empty_bitfield, 0xFF, 1<<(2*CELL_BITS-3));
  m_is_empty = true;
  m_need_check_empty = false;
}

// === Implemenation of TileItem_BaseColorKey ==============================
TileItem_BaseColorKey::TileItem_BaseColorKey(uint8_t alpha_threshold)
  : TileItem_NonEmpty(alpha_threshold)
{
}

TileItem_BaseColorKey::TileItem_BaseColorKey(uint8_t bpp, uint8_t alpha_threshold)
  : TileItem_NonEmpty(alpha_threshold)
{
  SDL_Surface     *surf = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY,
                                               CELL_DIM, CELL_DIM, bpp, 0, 0, 0, 0);
  m_surface = Surface(SDL_DisplayFormat(surf));
  SDL_FreeSurface(surf);
  MapColorKey();
}

void TileItem_BaseColorKey::ForceEmpty()
{
  TileItem_NonEmpty::ForceEmpty();
  m_surface.Fill(color_key);
}

void TileItem_BaseColorKey::MapColorKey()
{
  color_key = m_surface.MapRGBA(255, 0, 255, 0);
  m_surface.SetColorKey(SDL_SRCCOLORKEY|SDL_RLEACCEL, color_key);
}

void TileItem_BaseColorKey::Dig(const Point2i &position, const Surface& dig)
{
  m_start_check.SetValues(position.max(Point2i(0, 0)));
  m_end_check.SetValues(m_surface.GetSize().min(position+dig.GetSize()));

  const SDL_PixelFormat *fmt = dig.GetSurface()->format;
  Uint32 dig_ckey = (fmt->BitsPerPixel==32) ? 0 : fmt->colorkey;

  for (int py = m_start_check.y ; py < m_end_check.y ; py++) {
    for (int px = m_start_check.x ; px < m_end_check.x ; px++) {
      if (dig.GetPixel(px-position.x, py-position.y) != dig_ckey)
        m_surface.PutPixel(px, py, color_key);
    }
  }

  m_need_check_empty = true;
}

bool TileItem_BaseColorKey::CheckEmpty()
{
  uint8_t *buf = m_empty_bitfield;
  int      sx  = m_start_check.x&0xFFFFFFF8,
           ex = (m_end_check.x+7)&0xFFFFFFF8;

  m_need_check_empty = false;
  m_is_empty = true;
  m_need_resynch = true;

  buf += m_start_check.y<<(CELL_BITS-3);
  for (int py=m_start_check.y; py<m_end_check.y; py++) {
    for (int px=sx; px<ex; px+=8) {
      uint8_t empty_mask = 0;

      for (int i=0; i<8; i++) {
        if (m_surface.GetPixel(px+i, py) == color_key)
          empty_mask |= 1<<i;
      }

      if (empty_mask != 0xFF)
        m_is_empty = false;
      buf[px>>3] = empty_mask;
    }

    buf += CELL_DIM>>3;
  }

  // Make sure it is empty
  if (m_is_empty)
    CheckEmptyField();
  return m_is_empty;
}

void TileItem_BaseColorKey::MergeSprite(const Point2i &position, Surface& spr)
{
  m_start_check.SetValues(position.max(Point2i(0, 0)));
  m_end_check.SetValues(m_surface.GetSize().min(position+spr.GetSize()));

  const SDL_PixelFormat *fmt = spr.GetSurface()->format;
  Uint32 spr_ckey = (fmt->BitsPerPixel==32) ? 0 : fmt->colorkey;

  spr.Lock();

  for (int py = m_start_check.y ; py < m_end_check.y ; py++) {
    for (int px = m_start_check.x ; px < m_end_check.x ; px++) {
      Uint32 pixel = spr.GetPixel(px-position.x, py-position.y);
      if (pixel != spr_ckey) {
        Uint8 r, g, b, a;
        spr.GetRGBA(pixel, r, g, b, a);
        if (a < m_alpha_threshold) {
          pixel = color_key;
        } else {
          pixel = m_surface.MapRGBA(r, g, b, a);
        }
      } else {
        pixel = color_key;
      }

      m_surface.PutPixel(px, py, pixel);
    }
  }

  spr.Unlock();
  m_need_check_empty = true;
}

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#  define ALPHA_OFFSET  3
#else
#  define ALPHA_OFFSET  0
#endif


// === Implemenation of TileItem_ColorKey16 ==============================
TileItem_ColorKey16::TileItem_ColorKey16(void *pixels, int pitch, uint8_t threshold)
  : TileItem_BaseColorKey(threshold)
{
  SDL_PixelFormat fmt = { NULL /* palette */, 32 /*bpp*/, 4 /*Bpp*/,
                          0 /*Rloss*/, 0 /*Gloss*/, 0 /*Bloss*/, 0 /*Aloss*/,
                          16 /*Rshift*/, 8 /*Gshift*/, 0 /*Bshift*/, 24 /*Ashift*/,
                          0xFF0000, 0xFF00, 0xFF, 0xFF000000
#if SDL_MINOR_VERSION != 3
                          , 0 /*colorkey*/, 0 /*alpha*/
#endif
                        };
  m_surface = Surface::DisplayFormatColorKey((uint32_t *)pixels, &fmt,
                                             CELL_DIM, CELL_DIM, pitch, threshold);

  MapColorKey();
}

void TileItem_ColorKey16::Darken(int start_x, int end_x, uint8_t* buf)
{
  if (start_x < CELL_DIM && end_x >= 0) {
    //Clamp the value to empty only the in this tile
    start_x = (start_x < 0) ? 0 : (start_x >= CELL_DIM) ? CELL_DIM - 1 : start_x;
    end_x = (end_x >= CELL_DIM) ? CELL_DIM - start_x : end_x - start_x + 1;

    uint16_t *ptr = (uint16_t*)buf;
    ptr += start_x;
    while (end_x--) {
      uint16_t s = ptr[0];
      if (s != color_key)
        ptr[0] = (s>>1)&0x7BEF;
      ptr++;
    }
  }
}

void TileItem_ColorKey16::Empty(int start_x, int end_x, uint8_t* buf)
{
  if (start_x < CELL_DIM && end_x >= 0) {
    //Clamp the value to empty only the in this tile
    start_x = (start_x < 0) ? 0 : (start_x >= CELL_DIM) ? CELL_DIM - 1 : start_x;
    end_x = (end_x >= CELL_DIM) ? CELL_DIM - start_x : end_x - start_x + 1;

    uint16_t *ptr = (uint16_t*)buf;
    uint16_t ckey = color_key;
    ptr += start_x;
    while (end_x--)
      *(ptr++) = ckey;
  }
}

void TileItem_ColorKey16::ScalePreview(uint8_t* out, int x, uint opitch, uint shift)
{
  const Uint16 *idata  = (Uint16*)m_surface.GetPixels();
  Uint16       *odata  = (Uint16*)out;
  uint          ipitch = m_surface.GetPitch();
  Point2i       start  = m_start_check>>shift;
  Point2i       end    = (m_end_check + (1<<shift) -1)>>shift;

  opitch >>= 1;
  odata += (x<<(CELL_BITS-shift)) + start.y*opitch;
  ipitch >>= 1;
  idata += (start.y<<shift)*ipitch;

  for (int j=start.y; j<end.y; j++) {
    for (int i=start.x; i<end.x; i++) {
      const Uint16* ptr = idata + (i<<shift);
      uint count = 0, p0 = 0, p1 = 0, p2 = 0;

      for (uint u=0; u<(1U<<shift); u++) {
        for (uint v=0; v<(1U<<shift); v++) {
          Uint16 tmp = ptr[v];
          if (tmp != color_key) {
            p0 += tmp&0xF800;
            p1 += tmp&0x07C0;
            p2 += tmp&0x001F;
            count++;
          }
        }
        ptr += ipitch;
      }

      // Convert color_key count to alpha
      if (count) {
        odata[i] = ((p0/count)&0xF800)|((p1/count)&0x07C0)|(p2/count);
      } else {
        // Completely transparent
        odata[i] = 0xF81F;
      }
    }
    odata += opitch;
    idata += ipitch<<shift;
  }

  if (m_need_check_empty)
    CheckEmpty();
}

// === Implemenation of TileItem_ColorKey24 ==============================
TileItem_ColorKey24::TileItem_ColorKey24(void *pixels, int pitch, uint8_t threshold)
  : TileItem_BaseColorKey(threshold)
{
  uint8_t *ptr  = (uint8_t*)pixels;
  int      x, y;

  // Set pixels considered as transparent as colorkey
  for (y=0; y<CELL_DIM; y++) {
    for (x=0; x<CELL_DIM; x++)
      if (ptr[x*4 + ALPHA_OFFSET] == SDL_ALPHA_TRANSPARENT)
        *((Uint32*)(ptr + x*4)) = COLOR_KEY;
    ptr += pitch;
  }

  SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(pixels, CELL_DIM, CELL_DIM, 32, pitch,
                                               0xFF0000, 0xFF00, 0xFF, 0xFF000000);
  SDL_SetAlpha(surf, 0, 0);
  SDL_PixelFormat fmt;
  memset(&fmt, 0, sizeof(fmt));
  fmt.BitsPerPixel = 24;
  m_surface = Surface(SDL_ConvertSurface(surf, &fmt, SDL_SWSURFACE));
  SDL_FreeSurface(surf);
  MapColorKey();
}

void TileItem_ColorKey24::Darken(int start_x, int end_x, uint8_t* buf)
{
  if( start_x < CELL_DIM && end_x >= 0) {
    //Clamp the value to empty only the in this tile
    start_x = (start_x < 0) ? 0 : (start_x >= CELL_DIM) ? CELL_DIM - 1 : start_x;
    end_x = (end_x >= CELL_DIM) ? CELL_DIM - start_x : end_x - start_x + 1;

    buf += 3*start_x;
    while(end_x--) {
      if (buf[0]!=0xFF || buf[1] || buf[2]!=0xFF) {
        buf[0] = (buf[0]>>1)&0x7F;
        buf[1] = (buf[1]>>1)&0x7F;
        buf[2] = (buf[2]>>1)&0x7F;
      }
      buf += 3;
    }
  }
}

void TileItem_ColorKey24::Empty(int start_x, int end_x, uint8_t* buf)
{
  if( start_x < CELL_DIM && end_x >= 0) {
    //Clamp the value to empty only the in this tile
    start_x = (start_x < 0) ? 0 : (start_x >= CELL_DIM) ? CELL_DIM - 1 : start_x;
    end_x = (end_x >= CELL_DIM) ? CELL_DIM - start_x : end_x - start_x + 1;

    buf += 3*start_x;
    while (end_x--) {
      *(buf++) = 0xFF;
      *(buf++) = 0x00;
      *(buf++) = 0xFF;
    }
  }
}

void TileItem_ColorKey24::ScalePreview(uint8_t* out, int x, uint opitch, uint shift)
{
  const uint8_t *idata  = m_surface.GetPixels();
  uint           ipitch = m_surface.GetPitch();
  Point2i        start  = m_start_check>>shift;
  Point2i        end    = (m_end_check + (1<<shift) -1)>>shift;

  out   += 4*(x<<(CELL_BITS-shift)) + start.y*opitch;
  idata += (start.y<<shift)*ipitch;

  for (int j=start.y; j<end.y; j++) {
    for (int i=start.x; i<end.x; i++) {
      const uint8_t* ptr = idata + 3*(i<<shift);
      uint count = 0, p0 = 0, p1 = 0, p2 = 0;

      for (uint u=0; u<(1U<<shift); u++) {
        for (uint v=0; v<(1U<<shift); v++) {
          const uint8_t* pix = ptr + 3*v;
          if (pix[0]!=0xFF || pix[1] || pix[2]!=0xFF) {
            p0 += (uint)pix[0];
            p1 += (uint)pix[1];
            p2 += (uint)pix[2];
            count++;
          }
        }
        ptr += ipitch;
      }

      // Convert color_key count to alpha
      if (count) {
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
        out[4*i+0] = p0 / count;
        out[4*i+1] = p1 / count;
        out[4*i+2] = p2 / count;
        out[4*i+3] = (255*count)>>(2*shift);
#else
        out[4*i+0] = (255*count)>>(2*shift);
        out[4*i+1] = p2 / count;
        out[4*i+2] = p1 / count;
        out[4*i+3] = p0 / count;
#endif
      } else {
        // Completely transparent
        *((Uint32*)(out+4*i)) = 0;
      }
    }
    out   += opitch;
    idata += ipitch<<shift;
  }

  if (m_need_check_empty)
    CheckEmpty();
}

// === Implemenation of TileItem_SoftwareAlpha ==============================
TileItem_AlphaSoftware::TileItem_AlphaSoftware(uint8_t alpha_threshold)
  : TileItem_NonEmpty(alpha_threshold)
{
  // Calls NewSurface, which properly sets to display format alpha
  m_surface = Surface(Point2i(CELL_DIM, CELL_DIM), SDL_SWSURFACE|SDL_SRCALPHA, true);
}

void TileItem_AlphaSoftware::ForceEmpty()
{
  TileItem_NonEmpty::ForceEmpty();
  m_surface.Fill(0);
  transparent = false;
}

TileItem_AlphaSoftware::TileItem_AlphaSoftware(void *pixels, int pitch, uint8_t alpha_threshold)
  : TileItem_NonEmpty(alpha_threshold)
{
  SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(pixels, CELL_DIM, CELL_DIM, 32, pitch,
                                               0xFF0000, 0xFF00, 0xFF, 0xFF000000);
  // Required to have a copy of the area
  m_surface = Surface(SDL_DisplayFormatAlpha(surf));
  SDL_FreeSurface(surf);

  // Check if the tile is in fact transparent (ie even if empty, must not be deleted)
  const Uint32 *ptr = (Uint32*)pixels;
  for (int y=0; y<CELL_DIM; y++) {
    for (int x=0; x<CELL_DIM; x++) {
      Uint32 pix = ptr[x];
      // Does the pixel have data but is considered empty?
      if (pix&0xFFFFFF && (pix>>24)<alpha_threshold) {
        transparent = true;
        return;
      }
    }
    ptr += pitch>>2;
  }

  transparent = false;
}

void TileItem_AlphaSoftware::ScalePreview(uint8_t *odata, int x, uint opitch, uint shift)
{
  const Uint8 *idata  = m_surface.GetPixels();
  uint         ipitch = m_surface.GetPitch();
  Point2i      start  = m_start_check>>shift;
  Point2i      end    = (m_end_check + (1<<shift) -1)>>shift;
  uint         p0, p1, p2, p3;

  //printf("Converted (%i,%i)->(%i,%i) to (%i,%i)->(%i,%i)\n",
  //       m_start_check.x, m_start_check.y, m_end_check.x, m_end_check.y,
  //       start.x, start.y, end.x, end.y);

  odata += (x<<(2+CELL_BITS-shift)) + start.y*opitch;
  idata += (start.y<<shift)*ipitch;

  for (int j=start.y; j<end.y; j++) {
    for (int i=start.x; i<end.x; i++) {
      const Uint8* ptr = idata + (i<<(2+shift));

      p0 = 0; p1 = 0; p2 = 0; p3 = 0;

      for (uint u=0; u<(1U<<shift); u++) {
        for (uint v=0; v<(1U<<shift); v++) {
          p0 += ptr[4*v+0];
          p1 += ptr[4*v+1];
          p2 += ptr[4*v+2];
          p3 += ptr[4*v+3];
        }
        ptr += ipitch;
      }

      odata[4*i+0] = (p0 + (1<<(2*shift-1)))>>(2*shift);
      odata[4*i+1] = (p1 + (1<<(2*shift-1)))>>(2*shift);
      odata[4*i+2] = (p2 + (1<<(2*shift-1)))>>(2*shift);
      odata[4*i+3] = (p3 + (1<<(2*shift-1)))>>(2*shift);
    }
    odata += opitch;
    idata += ipitch<<shift;
  }

  if (m_need_check_empty)
    CheckEmpty();
}

void TileItem_AlphaSoftware::Empty(int start_x, int end_x, uint8_t* buf)
{
  if (start_x < CELL_DIM && end_x >= 0) {
    //Clamp the value to empty only the in this tile
    start_x = (start_x < 0) ? 0 : (start_x >= CELL_DIM) ? CELL_DIM - 1 : start_x;
    end_x = (end_x >= CELL_DIM) ? CELL_DIM - start_x : end_x - start_x + 1;

    buf += start_x * 4;
    memset(buf, 0 , 4 * end_x);
  }
}

void TileItem_AlphaSoftware::Dig(const Point2i &position, const Surface& dig)
{
  m_start_check.SetValues(position.max(Point2i(0, 0)));
  m_end_check.SetValues(m_surface.GetSize().min(position+dig.GetSize()));

  Uint32 *ptr    = (Uint32 *)m_surface.GetPixels();
  int     pitch  = m_surface.GetPitch()>>2;

  ptr += m_start_check.y*pitch;

  for (int py = m_start_check.y ; py < m_end_check.y ; py++) {
    for (int px = m_start_check.x ; px < m_end_check.x ; px++) {
      if (dig.GetPixel(px-position.x, py-position.y) != 0)
        ptr[px] = 0;
    }
    ptr += pitch;
  }

  m_need_check_empty = true;
}

void TileItem_AlphaSoftware::Darken(int start_x, int end_x, uint8_t* buf)
{
  if (start_x < CELL_DIM && end_x >= 0) {
    //Clamp the value to empty only the in this tile
    start_x = (start_x < 0) ? 0 : (start_x >= CELL_DIM) ? CELL_DIM - 1 : start_x;
    end_x = (end_x >= CELL_DIM) ? CELL_DIM - start_x : end_x - start_x + 1;

    uint32_t *ptr = (uint32_t*)buf;
    ptr += start_x;
    while(end_x--) {
      uint32_t s = ptr[0];
      // Mask component MSBs and alpha, then readd alpha
      // We have force the mask
      ptr[0] = ((s>>1)&0x007F7F7F) | (s&0xFF000000);
      ptr++;
    }
  }
}

bool TileItem_AlphaSoftware::CheckEmpty()
{
  const Uint32 *ptr   = (Uint32 *)m_surface.GetPixels();
  int           pitch = m_surface.GetPitch()>>2;
  uint8_t      *buf   = m_empty_bitfield;
  int           sx    = m_start_check.x&0xFFFFFFF8,
                ex    = (m_end_check.x+7)&0xFFFFFFF8;

  m_is_empty = true;
  m_need_check_empty = false;
  m_need_resynch = true;

  ptr += m_start_check.y*pitch;
  buf += m_start_check.y<<(CELL_BITS-3);
  for (int py=m_start_check.y; py<m_end_check.y; py++) {
    for (int px=sx; px<ex; px+=8) {
      uint8_t mask_empty = 0;

      for (int i=0; i<8; i++) {
        if ((ptr[px+i]&0xFF000000)>>24 < m_alpha_threshold)
          mask_empty |= 1<<i;
      }

      if (mask_empty != 0xFF)
        m_is_empty = false;
      buf[px>>3] = mask_empty;
    }

    ptr += pitch;
    buf += CELL_DIM>>3;
  }

  // Make sure it is empty
  if (transparent) {
    m_is_empty = false;
    m_need_check_empty = false;
    return false;
  }
  if (m_is_empty)
    CheckEmptyField();
  return m_is_empty;
}
