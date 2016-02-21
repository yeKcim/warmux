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

#include "map/tile.h"
#include <SDL.h>
#include <png.h>
#include "map/tileitem.h"
#include "game/config.h"
#include "game/game.h"
#include "game/game_time.h"
#include "graphic/surface.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "include/app.h"
#include "map/camera.h"

// We need only one empty tile
static TileItem_Empty EmptyTile;

Tile::Tile()
  : m_use_alpha(true)
  , nbCells(Point2i(0, 0))
  , m_preview(NULL)
  , m_last_preview_redraw(0)
{
  ASSERT(CELL_BITS > 3);
}

void Tile::FreeMem()
{
  for (int i = 0; i < nbCells.x * nbCells.y; i++) {
    // Don't delete empty tile as we use only one instance for empty tile
    if(item[i] != &EmptyTile)
      delete item[i];
  }
  nbCells.x = nbCells.y = 0;
  item.clear();
  if (m_preview)
    delete m_preview;
  m_preview = NULL;

  crc = 0;
}

Tile::~Tile()
{
  FreeMem();
}

void Tile::InitTile(const Point2i &pSize, const Point2i & upper_left_offset, const Point2i & lower_right_offset)
{
  m_upper_left_offset = upper_left_offset;
  m_lower_right_offset = lower_right_offset;
  startCell = upper_left_offset >> CELL_BITS;

  size = pSize + upper_left_offset;
  endCell = size >> CELL_BITS;
  if (size.x & CELL_MASK)
    endCell.x++;
  if (size.y & CELL_MASK)
    endCell.y++;

  size += lower_right_offset;
  nbCells = size >> CELL_BITS;
  if (size.x & CELL_MASK)
    nbCells.x++;
  if (size.y & CELL_MASK)
    nbCells.y++;
}

void Tile::Dig(const Point2i &position, const Surface& dig)
{
  Point2i  firstCell = Clamp(position>> CELL_BITS);
  Point2i  lastCell  = Clamp((position + dig.GetSize())>> CELL_BITS);
  uint     index     = firstCell.y*nbCells.x;

  m_preview->Lock();

  uint8_t *dst       = m_preview->GetPixels();
  int      pitch     = m_preview->GetPitch();
  Point2i c;

  dst += (firstCell.y-startCell.y)*(pitch<<(CELL_BITS-m_shift));

  for (c.y = firstCell.y; c.y <= lastCell.y; c.y++) {
    for(c.x = firstCell.x; c.x <= lastCell.x; c.x++) {
      TileItem *ti = item[index + c.x];
      if (!ti->IsTotallyEmpty()) {
        TileItem_NonEmpty *tin = static_cast<TileItem_NonEmpty *>(ti);

        tin->GetSurface().Lock();
        tin->Dig(position - (c<<CELL_BITS), dig);
        if (c >= startCell && c < endCell) // don't write outside minimap!
          tin->ScalePreview(dst, c.x-startCell.x, pitch, m_shift);
        tin->GetSurface().Unlock();

        if (tin->NeedDelete()) {
          // no need to display this tile as it can be deleted!
          delete tin;
          // Don't instanciate a new empty tile but use the already existing one
          item[index + c.x] = &EmptyTile;
        }
      }
    }
    dst   += pitch<<(CELL_BITS-m_shift);
    index += nbCells.x;
  }

  m_preview->Unlock();

  m_last_preview_redraw = GameTime::GetInstance()->Read();
}

void Tile::Dig(const Point2i &center, const uint radius)
{
  Point2i size = Point2i(2 * (radius + EXPLOSION_BORDER_SIZE),
                         2 * (radius + EXPLOSION_BORDER_SIZE));
  Point2i position  = center - Point2i(radius + EXPLOSION_BORDER_SIZE,
                                       radius + EXPLOSION_BORDER_SIZE);

  Point2i  firstCell = Clamp(position>>CELL_BITS);
  Point2i  lastCell  = Clamp((position+size)>>CELL_BITS);
  uint     index     = firstCell.y*nbCells.x;

  m_preview->Lock();

  uint8_t *dst       = m_preview->GetPixels();
  int      pitch     = m_preview->GetPitch();
  Point2i  c;

  dst += (firstCell.y-startCell.y)*(pitch<<(CELL_BITS-m_shift));

  for (c.y = firstCell.y; c.y <= lastCell.y; c.y++) {
    for (c.x = firstCell.x; c.x <= lastCell.x; c.x++) {
      TileItem *ti = item[index + c.x];
      if (!ti->IsTotallyEmpty()) {
        TileItem_NonEmpty *tin = static_cast<TileItem_NonEmpty *>(ti);

        tin->GetSurface().Lock();
        tin->Dig(center - (c<<CELL_BITS), radius);
        if (c >= startCell && c < endCell) // don't write outside minimap!
          tin->ScalePreview(dst, c.x-startCell.x, pitch, m_shift);
        tin->GetSurface().Unlock();
 
        if (tin->NeedDelete()) {
          // no need to display this tile as it can be deleted!
          delete tin;
          // Don't instanciate a new empty tile but use the already existing one
          item[index + c.x] = &EmptyTile;
        }
      }
    }
    dst   += pitch<<(CELL_BITS-m_shift);
    index += nbCells.x;
  }

  m_preview->Unlock();
  m_last_preview_redraw = GameTime::GetInstance()->Read();
}

TileItem_NonEmpty* Tile::GetNonEmpty(uint x, uint y)
{
  TileItem          *ti  = item[y*nbCells.x + x];
  TileItem_NonEmpty *tin = NULL;

  if (ti->IsTotallyEmpty()) {
    // Do not delete the tile, it's a empty one!
    if (m_use_alpha) tin = new TileItem_AlphaSoftware(m_alpha_threshold);
    else             tin = new TileItem_ColorKey16(m_alpha_threshold);
    tin->ForceEmpty();
    item[y*nbCells.x +x] = tin;
  } else {
    tin = static_cast<TileItem_NonEmpty*>(ti);
  }

  return tin;
}

TileItem_NonEmpty* Tile::CreateNonEmpty(uint8_t *ptr, int stride)
{
  Uint32 *pix = (Uint32 *)ptr;

  for (int y=0; y<CELL_DIM; y++) {
    for (int x=0; x<CELL_DIM; x++) {
      if (pix[x]) {
        if (m_use_alpha)
          return new TileItem_AlphaSoftware(ptr, stride, m_alpha_threshold);
        return new TileItem_ColorKey16(ptr, stride, m_alpha_threshold);
      }
    }
    pix += stride>>2;
  }

  return new TileItem_ColorKey24(ptr, stride, m_alpha_threshold);
}

void Tile::PutSprite(const Point2i& pos, Sprite* spr)
{
  Point2i    firstCell = Clamp(pos>>CELL_BITS);
  Surface&   s         = spr->GetSurface();
  Point2i    lastCell  = Clamp((pos + s.GetSize())>>CELL_BITS);
  Rectanglei rec(pos, s.GetSize());

  m_preview->Lock();

  uint8_t   *pdst      = m_preview->GetPixels();
  int        pitch     = m_preview->GetPitch();
  Point2i c;

  s.SetAlpha(0, 0);
  pdst += (firstCell.y-startCell.y)*(pitch<<(CELL_BITS-m_shift));

  for (c.y = firstCell.y; c.y <= lastCell.y; c.y++) {
    for (c.x = firstCell.x; c.x <= lastCell.x; c.x++) {
#if 1
      Point2i cell_pos = (c<<CELL_BITS);
      Rectanglei src;
      Rectanglei dst;
      src.SetPosition(rec.GetPosition() - cell_pos);
      if (src.GetPositionX() < 0)
        src.SetPositionX(0);
      if (src.GetPositionY() < 0)
        src.SetPositionY(0);

      src.SetSize(rec.GetPosition() + rec.GetSize() - cell_pos - src.GetPosition());
      if (src.GetSizeX() + src.GetPositionX() > CELL_DIM)
        src.SetSizeX(CELL_DIM - src.GetPositionX());
      if (src.GetSizeY() + src.GetPositionY() > CELL_DIM)
        src.SetSizeY(CELL_DIM - src.GetPositionY());

      dst.SetPosition(cell_pos - rec.GetPosition());
      if (dst.GetPositionX() < 0)
        dst.SetPositionX(0);
      if (dst.GetPositionY() < 0)
        dst.SetPositionY(0);
      dst.SetSize(src.GetSize());

      TileItem_NonEmpty *tin = GetNonEmpty(c.x, c.y);
      tin->GetSurface().Blit(s, dst, src.GetPosition());
#else
      TileItem_NonEmpty *tin = GetNonEmpty(c.x, c.y);
      tin->GetSurface().Blit(s, pos-(c<<CELL_BITS));
#endif

      tin->GetSurface().Lock();
      if (c >= startCell && c < endCell) // don't write outside minimap!
        tin->ScalePreview(pdst, c.x-startCell.x, pitch, m_shift);
      tin->GetSurface().Unlock();

      if (tin->NeedDelete()) {
        // no need to display this tile as it can be deleted!
        delete tin;
        // Don't instanciate a new empty tile but use the already existing one
        item[c.y*nbCells.x + c.x] = &EmptyTile;
      }
    }
    pdst += pitch<<(CELL_BITS-m_shift);
  }

  s.SetAlpha(SDL_SRCALPHA, 0);

  m_preview->Unlock();
  m_last_preview_redraw = GameTime::GetInstance()->Read();
}

void Tile::MergeSprite(const Point2i &position, Surface& surf)
{
  Point2i  firstCell = Clamp(position>>CELL_BITS);
  Point2i  lastCell  = Clamp((position + surf.GetSize())>>CELL_BITS);

  m_preview->Lock();

  uint8_t *dst       = m_preview->GetPixels();
  int      pitch     = m_preview->GetPitch();
  Point2i  c;

  dst += (firstCell.y-startCell.y)*(pitch<<(CELL_BITS-m_shift));

  for (c.y = firstCell.y; c.y <= lastCell.y; c.y++) {
    for (c.x = firstCell.x; c.x <= lastCell.x; c.x++) {

      Point2i offset = position - (c<<CELL_BITS);
      TileItem_NonEmpty *tin = GetNonEmpty(c.x, c.y);

      tin->GetSurface().Lock();
      tin->MergeSprite(offset, surf);
      if (c >= startCell && c < endCell) // don't write outside minimap!
        tin->ScalePreview(dst, c.x-startCell.x, pitch, m_shift);
      tin->GetSurface().Unlock();

      if (tin->NeedDelete()) {
        // no need to display this tile as it can be deleted!
        delete tin;
        // Don't instanciate a new empty tile but use the already existing one
        item[c.y*nbCells.x + c.x] = &EmptyTile;
      }
    }
    dst += pitch<<(CELL_BITS-m_shift);
  }

  m_preview->Unlock();
  m_last_preview_redraw = GameTime::GetInstance()->Read();
}

// Initialize preview depending on current video and map sizes
void Tile::InitPreview()
{
  Point2i world_size = (endCell+1-startCell)<<(CELL_BITS-m_shift);

  if (m_preview)
    delete m_preview;
  Quality qual = Config::GetInstance()->GetQuality();
  m_use_alpha = qual > QUALITY_16BPP;
  if (!m_use_alpha) {
    m_preview = new Surface(world_size, SDL_SWSURFACE, false);
    m_preview->SetColorKey(SDL_SRCCOLORKEY|SDL_RLEACCEL, 0xF81F);
    m_preview->Fill(0xF81F);
  } else {
    m_preview = new Surface(world_size, SDL_SWSURFACE, true);
    // Having an alpha channel forces SDL_SRCALPHA, so we must remove it
    m_preview->SetAlpha(0, 0);
  }

  // Actual preview size from pixel-wise information
  m_preview_size = (size - m_upper_left_offset - m_lower_right_offset)>>m_shift;
  // Offset is the difference between pixel-wise info and cell-wise one
  m_preview_rect = Rectanglei((m_upper_left_offset & CELL_MASK)>>m_shift,
                              m_preview_size);

  m_last_preview_redraw = GameTime::GetInstance()->Read();
}

// Rerender all of the preview
void Tile::CheckPreview(bool force)
{
  if (m_last_preview_redraw == 0) {
    m_last_preview_redraw = GameTime::GetInstance()->Read();
  }

  const Surface& window = GetMainWindow();
  if (!force && window.GetSize() == m_last_video_size)
    return;

  InitPreview();
  m_preview->Lock();
  uint8_t *dst   = m_preview->GetPixels();
  int      pitch = m_preview->GetPitch();

  // Fill the TileItem objects
  Point2i i;
  uint    offset = startCell.y*nbCells.x;

  for (i.y = startCell.y; i.y < endCell.y; i.y++) {
    for (i.x =startCell.x; i.x < endCell.x; i.x++) {
      TileItem *ti = item[i.x + offset];
      if (!ti->IsTotallyEmpty()) {
        TileItem_NonEmpty *tin = static_cast<TileItem_NonEmpty *>(ti);

        tin->GetSurface().Lock();
        tin->ScalePreview(dst, i.x-startCell.x, pitch, m_shift);
        tin->GetSurface().Unlock();
      }
    }
    dst    += pitch<<(CELL_BITS-m_shift);
    offset += nbCells.x;
  }

  m_preview->Unlock();
  m_last_video_size = GetMainWindow().GetSize();
}

void Tile::SetPreviewSizeDelta(int delta)
{
  // Preview too big
  if (m_shift+delta < 2)
    return;

  // Preview too small
  if (m_shift+delta > CELL_BITS)
    return;

  m_shift += delta;
  CheckPreview(true);
}

static void read_png_rows(png_structp png_ptr, uint8_t *buffer,
                          uint height, int stride)
{
  while (height--) {
    png_read_row(png_ptr, buffer, NULL);
    buffer += stride;
  }
}

bool Tile::LoadImage(const std::string& filename,
                     uint8_t alpha_threshold,
                     const Point2i & upper_left_offset,
                     const Point2i & lower_right_offset)
{
  FILE        *f        = NULL;
  png_structp  png_ptr  = NULL;
  png_infop    info_ptr = NULL;
  bool         ret      = false;
  uint8_t     *buffer   = NULL;
  int          stride;
  int          offsetx, offsety, endoffy;
  Point2i      i, world_size;
  uint8_t     *dst;
  int          width, height, pitch, color_type, depth, has_tRNS;

  // Opening the existing file
  f = fopen(filename.c_str(), "rb");
  if (!f)
    goto end;

  // Creating a png ...
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) // Structure and ...
    goto end;
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) // Information.
    goto end;

  // Associate png struture with a file
  png_init_io(png_ptr, f);
  png_read_info(png_ptr, info_ptr);
  width  = png_get_image_width(png_ptr, info_ptr);
  height = png_get_image_height(png_ptr, info_ptr);
  color_type = png_get_color_type(png_ptr, info_ptr);
  depth = png_get_bit_depth(png_ptr, info_ptr);

  // expand paletted colors into true RGB triplets
  has_tRNS = png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS);
  if (color_type == PNG_COLOR_TYPE_PALETTE || has_tRNS
      || (color_type == PNG_COLOR_TYPE_GRAY && depth < 8))
    png_set_expand(png_ptr);
  if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png_ptr) ;

  // tell libpng to strip 16 bit/color files down to 8 bits/color
  if (depth == 16)
    png_set_strip_16(png_ptr);

  // Find the proper mapping of channels
  png_set_bgr(png_ptr);

  FreeMem();
  m_alpha_threshold = alpha_threshold;
  InitTile(Point2i(width, height), upper_left_offset, lower_right_offset);

  // Task 6730: biggest dimension won't be bigger than 2/5,
  // often less in fact
  m_shift = 0;
  world_size = (endCell+1-startCell)<<CELL_BITS;
  m_last_video_size = GetMainWindow().GetSize();
  while (5*world_size.x>2*m_last_video_size.x ||
         5*world_size.y>2*m_last_video_size.y) {
    world_size >>= 1;
    m_shift++;
  }
  InitPreview();

  stride  = (endCell.x+1 - startCell.x)<<(CELL_BITS+2);
  buffer  = new uint8_t[stride<<CELL_BITS];
  offsetx = (upper_left_offset.x & CELL_MASK)<<2;
  offsety = upper_left_offset.y & CELL_MASK;
  endoffy = (height + upper_left_offset.y) & CELL_MASK;

  m_preview->Lock();
  dst   = m_preview->GetPixels();
  pitch = m_preview->GetPitch();

  // Top transparent+empty row
  for (i.y = 0; i.y < startCell.y; i.y++)
    for (i.x = 0; i.x < nbCells.x; i.x++)
      item.push_back(&EmptyTile);

  for (; i.y < endCell.y; i.y++) {
    // Empty left border
    for (i.x = 0; i.x<startCell.x; i.x++)
      item.push_back(&EmptyTile);

    // Prepare buffer for current line
    if (i.y == startCell.y) {
      memset(buffer, 0, stride<<CELL_BITS);
      // Only some lines are to be loaded
      read_png_rows(png_ptr, buffer + offsetx + offsety*stride,
                    CELL_DIM - offsety, stride);
    } else if (i.y == endCell.y-1) {
      memset(buffer, 0, stride<<CELL_BITS);
      // Only some lines are to be loaded
      read_png_rows(png_ptr, buffer + offsetx, endoffy, stride);
    } else {
      read_png_rows(png_ptr, buffer + offsetx, CELL_DIM, stride);
    }

    for (; i.x < endCell.x; i.x++) {
      TileItem_NonEmpty *ti;

      if (m_use_alpha) {
        ti = CreateNonEmpty(buffer + ((i.x - startCell.x)<<(CELL_BITS+2)), stride);
      } else {
        ti = new TileItem_ColorKey16(buffer + ((i.x - startCell.x)<<(CELL_BITS+2)),
                                     stride, m_alpha_threshold);
      }

      if (ti->NeedDelete()) {
        // no need to display this tile as it can be deleted!
        delete ti;
        // Don't instanciate a new empty tile but use the already existing one
        item.push_back(&EmptyTile);
      } else {
        //printf("Cell (%i,%i) not deleted\n", i.x, i.y);
        ti->GetSurface().Lock();
        ti->ScalePreview(dst, i.x-startCell.x, pitch, m_shift);
        ti->GetSurface().Unlock();
        // 16 bits values output, so 65536 non-empty tiles to overflow that result
        // That's at list 1GB of memory for a map somewhere...
        crc += ti->GetChecksum();
        item.push_back(ti);
      }
    }
    dst += pitch<<(CELL_BITS-m_shift);

    // Empty right border
    for (; i.x < nbCells.x; i.x++)
      item.push_back(&EmptyTile);
  }

  m_preview->Unlock();

  // Bottom transparent+empty row
  for (; i.y < nbCells.y; i.y++)
    for (i.x = 0; i.x < nbCells.x; i.x++)
      item.push_back(&EmptyTile);
  ret = true;

end:
  if (buffer)
    delete[] buffer;
  if (png_ptr)
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  if (info_ptr)
    png_destroy_info_struct(png_ptr, &info_ptr);
  if (f)
    fclose(f);
  return ret;
}

bool Tile::IsEmpty(const Point2i & pos) const
{
  return item[(pos.y>>CELL_BITS) * nbCells.x + (pos.x>>CELL_BITS)]->IsEmpty(pos & CELL_MASK);
}

void Tile::DrawTile()
{
  Point2i firstCell = Clamp(Camera::GetInstance()->GetPosition()>> CELL_BITS);
  Point2i lastCell = Clamp((Camera::GetInstance()->GetPosition() + Camera::GetInstance()->GetSize())>> CELL_BITS);
  Point2i i;
  for (i.y = firstCell.y; i.y <= lastCell.y; i.y++)
    for (i.x = firstCell.x; i.x <= lastCell.x; i.x++)
      item[i.y*nbCells.x + i.x]->Draw(i);
}

void Tile::DrawTile_Clipped(const Rectanglei & worldClip) const
{
  // Revision 514:
  // worldClip.SetSize( worldClip.GetSize() + 1); // mmm, does anything gives areas
  // too small to redraw ?
  //
  // Revision 3095:
  // Sorry, I don't understand that comment. Moreover the +1 produces a bug when the ground of
  // a map have an alpha value != 255 and != 0
  Point2i cam_pos   = Camera::GetInstance()->GetPosition();
  Point2i firstCell = Clamp(worldClip.GetPosition()>> CELL_BITS);
  Point2i lastCell  = Clamp((worldClip.GetBottomRightPoint())>> CELL_BITS);
  Point2i c;

  for (c.y = firstCell.y; c.y <= lastCell.y; c.y++) {
    uint index = c.y*nbCells.x;
    for (c.x = firstCell.x; c.x <= lastCell.x; c.x++) {
      if (item[index + c.x]->IsTotallyEmpty())
        continue;
      TileItem_NonEmpty *tin = static_cast<TileItem_NonEmpty*>(item[index + c.x]);

      // For all selected items, clip source and destination blitting rectangles
      Rectanglei destRect(c<<CELL_BITS, Point2i(CELL_DIM, CELL_DIM));

      destRect.Clip(worldClip);
      Point2i ptDest = destRect.GetPosition() - cam_pos;
      Point2i ptSrc = destRect.GetPosition() - (c<<CELL_BITS);

      GetMainWindow().Blit(tin->GetSurface(),
                           Rectanglei(ptSrc, destRect.GetSize()),
                           ptDest);
    }
  }

#if 0
  GetMainWindow().RectangleColor(Rectanglei(worldClip.GetPosition() - cam_pos,
                                            worldClip.GetSize()),
                                 primary_red_color);
#endif
}

Surface Tile::GetPart(const Rectanglei& rec)
{
  Surface part(rec.GetSize(), SDL_SWSURFACE|SDL_SRCALPHA, true);
  part.SetAlpha(0, 0);
  part.Fill(0x00000000);
  part.SetAlpha(SDL_SRCALPHA, 0);

  Point2i firstCell = Clamp(rec.GetPosition()>> CELL_BITS);
  Point2i lastCell = Clamp((rec.GetPosition() + rec.GetSize())>> CELL_BITS);
  Point2i i = nbCells - 1;
  bool    force_copy = SDL_GetVideoInfo()->vfmt->BytesPerPixel>2;

  for (i.y = firstCell.y; i.y <= lastCell.y; i.y++) {
    uint index = i.y*nbCells.x;
    for (i.x = firstCell.x; i.x <= lastCell.x; i.x++) {
      TileItem *ti = item[index + i.x];
      if (ti->IsTotallyEmpty())
        continue;
      TileItem_NonEmpty *tin = static_cast<TileItem_NonEmpty*>(ti);

      Point2i cell_pos = i<<CELL_BITS;
      Rectanglei src;
      Point2i dst;
      src.SetPosition(rec.GetPosition() - cell_pos);
      if (src.GetPositionX() < 0) src.SetPositionX(0);
      if (src.GetPositionY() < 0) src.SetPositionY(0);

      src.SetSize(rec.GetPosition() + rec.GetSize() - cell_pos - src.GetPosition());
      if (src.GetSizeX() + src.GetPositionX() > CELL_DIM)
        src.SetSizeX(CELL_DIM - src.GetPositionX());
      if (src.GetSizeY() + src.GetPositionY() > CELL_DIM)
        src.SetSizeY(CELL_DIM - src.GetPositionY());

      dst =  cell_pos - rec.GetPosition();
      if (dst.x < 0) dst.x = 0;
      if (dst.y < 0) dst.y = 0;

      if (force_copy) tin->GetSurface().SetAlpha(0, 0);
      part.Blit(tin->GetSurface(), src, dst);
      if (force_copy) tin->GetSurface().SetAlpha(SDL_SRCALPHA, 0);
    }
  }
  return part;
}

Tile::SynchTileList Tile::GetTilesToSynch()
{
  SynchTileList list;
  uint cellsCount = nbCells.x * nbCells.y;

  for (uint i = 0; i < cellsCount; i++) {
    if (item[i]->IsTotallyEmpty())
      continue;
    TileItem_NonEmpty *t = static_cast<TileItem_NonEmpty*>(item[i]);

    if (t->NeedSynch()) {
      SynchTileInfo info = { i, t->GetSynchsum() };
      list.push_back(info);
    }
  }

  return list;
}
