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
 ******************************************************************************
 * Polygon class. Store point of a polygon and handle affine transformation
 *****************************************************************************/

#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <WARMUX_point.h>
#include <WARMUX_rectangle.h>
#include <WARMUX_types.h>

// Forward declarations
class Color;
class AffineTransform2D;
class Surface;
class Sprite;

/** Use to draw the polygon */
class PolygonBuffer
{
public:
  int16_t * vx;
  int16_t * vy;
  int buffer_size;
  int array_size;
  PolygonBuffer();
  ~PolygonBuffer() { delete[] vx; delete[] vy; }
  int GetSize() const { return buffer_size; }
  void SetSize(const int size);
};

/** Store information about a item (sprite) of the polygon */
class PolygonItem
{
public:
  typedef enum { LEFT, H_CENTERED, RIGHT } H_align;
  typedef enum { TOP,  V_CENTERED, BOTTOM } V_align;

protected:
  Point2d position;
  Point2d transformed_position;
  Sprite * item;
  H_align h_align;
  V_align v_align;

  virtual Point2i GetOffsetAlignment() const;

public:
  PolygonItem();
  PolygonItem(PolygonItem * item);
  PolygonItem(const Sprite * sprite, const Point2d & pos, H_align h_a = H_CENTERED, V_align v_a = V_CENTERED);
  virtual ~PolygonItem();
  void SetPosition(const Point2d & pos) { transformed_position = position = pos; }
  void SetAlignment(H_align h_a = H_CENTERED, V_align v_a = V_CENTERED)
  {
    h_align = h_a;
    v_align = v_a;
  }

  H_align GetHAlign() const { return h_align; }
  V_align GetVAlign() const { return v_align; }
  Point2d & GetPosition() { return position; }
  Point2d & GetTransformedPosition() { return transformed_position; }
  Point2i GetIntTransformedPosition() const { return Point2i(transformed_position.x, transformed_position.y); }
  virtual bool Contains(const Point2d & p) const;
  void SetSprite(Sprite * sprite) { item = sprite; }
  Sprite * GetSprite() { return item; }
  virtual void ApplyTransformation(const AffineTransform2D & trans);
  virtual void Draw(Surface * dest);
};

/** Store information about a simple shape */
class Polygon
{
private:
  void Init();

protected:
  bool is_closed;
  Surface * texture;
  Color * plane_color;
  Color * border_color;
  Point2d max;
  Point2d min;
  // Original shape
  std::vector<Point2d> original_shape;
  std::vector<Point2d> transformed_shape;
  // Vector of icons
  std::vector<PolygonItem *> items;
  // Shape position after an affine transformation
  PolygonBuffer * shape_buffer;

private:
  Polygon operator=(const Polygon&);

public:
  Polygon() { Init(); }
  Polygon(const std::vector<Point2d>& shape);
  Polygon(Polygon & poly);
  virtual ~Polygon();
  // Point handling
  void AddPoint(const Point2d & p);
  void InsertPoint(int index, const Point2d & p);
  void DeletePoint(int index);
  virtual void ApplyTransformation(const AffineTransform2D & trans, bool save_transformation = false);
  virtual void ResetTransformation();
  // Applying definitively the transformation
  void SaveTransformation(const AffineTransform2D & trans) { ApplyTransformation(trans, true); }

  // Test
  bool IsInsidePolygon(const Point2d & point) const;
  bool IsOverlapping(const Polygon & poly) const;
  bool IsClockWise() const;

  // Use to randomize a construction
  Point2d GetRandomUpperPoint();
  int GetRandomPointIndex();

  // Interpolation handling
  void AddBezierCurve(const Point2d& anchor1, const Point2d& control1,
                      const Point2d& control2, const Point2d& anchor2,
                      const int num_steps = 20, const bool add_first_point = true,
                      const bool add_last_point = true);
  void AddRandomCurve(const Point2d& start, const Point2d& end,
                      const Double x_random_offset, const Double y_random_offset,
                      const int num_steps, const bool add_first_point = true,
                      const bool add_last_point = true);
  Polygon * GetBezierInterpolation(Double smooth_value = 1.0, int num_steps = 20, Double rand = 0.0);
  void Expand(Double expand_value);

  // Size information
  Double GetWidth() const { return max.x - min.x; }
  Double GetHeight() const { return max.y - min.y; }
  Point2d GetSize() const { return max - min; }
  Point2i GetIntSize() const { return GetIntMax() - GetIntMin() + Point2i(1, 1); }
  int GetNbOfPoint() const { return (int)original_shape.size(); }
  Point2d GetMin() const { return min; }
  Point2i GetIntMin() const { return Point2i(min.x, min.y); }
  Point2d GetMax() const { return max; }
  Point2i GetIntMax() const { return Point2i(max.x, max.y); }
  virtual Rectanglei GetRectangleToRefresh() const { return Rectanglei(GetIntMin(), GetIntSize()); }

  // Buffer of transformed point
  PolygonBuffer * GetPolygonBuffer() { return shape_buffer; }

  // Type of the polygon
  bool IsTextured() const { return texture != NULL; }
  bool IsPlaneColor() const { return plane_color != NULL; }
  bool IsBordered() const { return border_color != NULL; }
  bool IsClosed() const;

  // Set type to Open
  void SetOpen();
  void SetClosed();

  // Texture handling
  Surface * GetTexture() { return texture; }
  void SetTexture(Surface * texture_surface) { texture = texture_surface; }

  // Color handling
  void SetBorderColor(const Color & color);
  void SetPlaneColor(const Color & color);
  const Color & GetBorderColor() const { return *border_color; }
  const Color & GetPlaneColor() const { return *plane_color; }

  // Drawing
  virtual void Draw(Surface * dest);
  virtual void DrawOnScreen();

  // Item management
  void AddItem(const Sprite * sprite, const Point2d & pos,
               PolygonItem::H_align h_a = PolygonItem::H_CENTERED,
               PolygonItem::V_align v_a = PolygonItem::V_CENTERED)
  { items.push_back(new PolygonItem(sprite, pos, h_a, v_a)); }
  virtual void AddItem(PolygonItem * item) { items.push_back(item); }
  void DelItem(int index);
  const std::vector<PolygonItem *>& GetItem() const { return items; }
  void ClearItem(bool free_mem = true);
};

class DecoratedBox : public Polygon
{
 public :
  enum Style {STYLE_ROUNDED, STYLE_SQUARE};

  DecoratedBox(Double width, Double height);
  ~DecoratedBox();
  virtual void Draw(Surface * dest);
  virtual void ApplyTransformation(const AffineTransform2D & trans, bool save_transformation);
  virtual void AddItem(PolygonItem * item);
  virtual void ResetTransformation();
  void SetPosition(Double x, Double y);
  void SetStyle(Style style) { m_style = style; }

 private :
  Point2d max_refresh;
  Point2d min_refresh;
  Point2d original_max;
  Point2d original_min;
  Surface *m_border;
  Style m_style;
};

void GenerateStyledBorder(Surface & source, DecoratedBox::Style style);

#endif /* POLYGON_H */
