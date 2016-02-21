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
 * Rectangle.h: Standard C++ Rectangle template
 ******************************************************************************
 * 2005/09/21:  Jean-Christophe Duberga (jcduberga@gmx.de)
 *              Initial version
 *****************************************************************************/

#ifndef WARMUX_RECTANGLE_H
#define WARMUX_RECTANGLE_H

#include <cmath>
#include <WARMUX_error.h>
#include <WARMUX_types.h>
#include <WARMUX_vector2.h>

/**
 * This template handle rectangles.
 *
 * @param T Type for position and size of the Rectangle
 */
template<class T> class rectangle
{
  protected:
    /** Position of the rectangle. */
    Vector2<T> position;
    /** Size of the rectangle. */
    Vector2<T> size;

  public:
    /**
     * Default constructor
     */
    inline rectangle():
                  position(0, 0),
                  size(0, 0)
                { }

                virtual ~rectangle() {};

    /**
     * Constructor for building a new rectangle.
     *
     * @param x Position among the x axe.
     * @param y Position among the y axe.
     * @param width Width of the new rectangle.
     * @param height Height of the new rectangle.
     */
    inline rectangle(T x, T y, T width, T height):
                  position(x, y),
                  size(width, height)
                { }

    /**
     * Constructor for building a new rectangle with the position and size specified.
     *
     * @param thePosition Position of the new rectangle.
     * @param theSize Size of the new rectangle.
     */
    inline rectangle(Vector2<T> thePosition, Vector2<T> theSize):
      position(thePosition),
                        size(theSize)
                {}

    /**
     * Set the position of the rectangle.
     *
     * @param x New X position.
     * @param y New Y position.
     */
    inline void SetPosition(T x, T y){
      position.SetValues(x, y);
    }

    /**
     * Change the position of the rectangle.
     *
     * @param newPos The new position of the rectangle.
     */
    inline void SetPosition(const Vector2<T> &newPos){
      position = newPos;
    }

    /**
     * Change the x position of the rectangle.
     *
     * @param x New X position.
     */
    inline void SetPositionX(T x){
      position.x = x;
    }

    /**
     * Change the y position of the rectangle.
     *
     * @param y New Y position.
     */
    inline void SetPositionY(T y){
      position.y = y;
    }

    /**
     * Set the size of the rectangle.
     *
     * @param sizeX new size among the x axe.
     * @param sizeY new size among the y axe.
     */
    inline void SetSize(T sizeX, T sizeY){
      size.SetValues(sizeX, sizeY);
    }

    /**
     * Change the X size of the rectangle.
     *
     * @param x New size among x axe.
     */
    inline void SetSizeX(T sizeX){
      size.x = sizeX;
    }

    /**
     * Change the Y size of the rectangle.
     *
     * @param y New size among y axe.
     */
    inline void SetSizeY(T sizeY){
      size.y = sizeY;
    }

    inline void SetSize(Vector2<T> newSize){
      size = newSize;
    }

    inline rectangle<T> GetRectangle() const{
      return *this;
    }

    /**
     * Return the position of the rectangle.
     */
    inline Vector2<T> GetPosition() const{
      return position;
    }

    inline T GetPositionX() const{
      return position.x;
    }

    inline T GetPositionY() const{
      return position.y;
    }

    inline T GetLeft() const{
      ASSERT( !IsSizeZero() );
      return GetPositionX();
    }

    inline T GetRight() const{
      ASSERT( !IsSizeZero() );
      return position.x + size.x;
    }

    inline T GetTop() const{
      ASSERT( !IsSizeZero() );
      return GetPositionY();
    }

    inline T GetBottom() const{
      ASSERT( !IsSizeZero() );
      return position.y + size.y;
    }

    /**
     * Return the size of the rectangle.
     */
    inline Vector2<T> GetSize() const{
      return size;
    }

    inline T GetSizeX() const{
      return size.x;
    }

    inline T GetSizeY() const{
      return size.y;
    }

    /**
     * Clip the current rectangle using an other rectangle.
     *
     * @param cr The rectangle used for clipping
     */
    void Clip(const rectangle &cr) {
      if( IsSizeZero() || cr.IsSizeZero() ) {
        size.x = 0;
        size.y = 0;
        return;
      }

      Vector2<T> r1BR = GetBottomRightPoint();
      Vector2<T> r2BR = cr.GetBottomRightPoint();
      Vector2<T> r1TL = GetTopLeftPoint();
      Vector2<T> r2TL = cr.GetTopLeftPoint();

      if (r1BR.x < r2TL.x || r1BR.y < r2TL.y ||
          r2BR.x < r1TL.x || r2BR.y < r1TL.y ) {
        size.x = 0;
        size.y = 0;
        return;
      }

      position.x = std::max(r1TL.x, r2TL.x);
      position.y = std::max(r1TL.y, r2TL.y);
      size.x     = std::min(r1BR.x, r2BR.x) - position.x + 1;
      size.y     = std::min(r1BR.y, r2BR.y) - position.y + 1;
    }

    /**
     * Return true if the point p is contained in the rectangle.
     *
     * @param p Point used to perform the check.
     */
    inline bool Contains( const Vector2<T> p ) const{
      if( IsSizeZero() )
        return false;

      return p >= GetTopLeftPoint() &&
        p <= GetBottomRightPoint();
    }

    /**
     * Return true if r2 is contained in the current rectangle.
     *
     * @param r2 The rectangle for witch the check if performed.
     */
    inline bool Contains( const rectangle<T> &r2 ) const{
      if( r2.IsSizeZero() )
        return false;

      return Contains( r2.GetTopLeftPoint() ) &&
        Contains( r2.GetBottomRightPoint() );
    }

    /**
     * Return true if there is an intersection between the current rectangle
     * and the r2 rectangle.
     *
     * @param r2 The second rectangle.
     */
    inline bool Intersect( const rectangle<T> &r2 ) const{
      if( IsSizeZero() || r2.IsSizeZero() )
        return false;

      Vector2<T> r1BR = GetBottomRightPoint();
      Vector2<T> r2BR = r2.GetBottomRightPoint();
      Vector2<T> r1TL = GetTopLeftPoint();
      Vector2<T> r2TL = r2.GetTopLeftPoint();

      if( r1BR.x < r2TL.x || r1BR.y < r2TL.y ||
           r2BR.x < r1TL.x || r2BR.y < r1TL.y )
        return false;

      return true;
    }

    /**
     * Return the point in the top left corner of the rectangle.
     *
     * If the rectangle has a size of zero, this point doesn't exist,
     * so the program crash with a failled ASSERTion.
     */
    inline Vector2<T> GetTopLeftPoint() const{
      ASSERT( !IsSizeZero() );
      return position;
    }

    /**
     * Return the point in the top right corner of the rectangle.
     *
     * If the rectangle has a size of zero, this point doesn't exist,
     * so the program crash with a failled ASSERTion.
     */
    inline Vector2<T> GetTopRightPoint() const{
      ASSERT( !IsSizeZero() );
      Vector2<T> r = position;

      r.x += size.x - 1;

      return r;
    }

    /**
     * Return the point in the bottom left corner of the rectangle.
     *
     * If the rectangle has a size of zero, this point doesn't exist,
     * so the program crash with a failled ASSERTion.
     */
    inline Vector2<T> GetBottomLeftPoint() const{
      ASSERT( !IsSizeZero() );
      Vector2<T> r = position;

      r.y += size.y - 1;

      return r;
    }

    /**
     * Return the point in the top left corner of the rectangle.
     *
     * If the rectangle has a size of zero, this point doesn't exist,
     * so the program crash with a failled ASSERTion. */
    inline Vector2<T> GetBottomRightPoint() const{
      ASSERT( !IsSizeZero() );
      return position + size - 1;
    }

    /**
     * Return true if the rectangle has a size of zero.
     */
    inline bool IsSizeZero() const{
      return size.IsXNull() || size.IsYNull();
    }

    inline bool operator==(const rectangle<T> &p2) const{
      return size==p2.size && position==p2.position;
    }
};

typedef rectangle<int>    Rectanglei;
typedef rectangle<Double>  Rectanglef;
typedef rectangle<Double> Rectangled;
#endif // _RECTANGLE_H
