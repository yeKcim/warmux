/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
 *****************************************************************************/

#ifndef MEMBER_H
#define MEMBER_H
#include <map>
#include <vector>
#include <WORMUX_point.h>
#include "character/body.h"

typedef std::vector<Point2f> v_attached;

// Forward declaration
class Sprite;
class c_junction; //defined in body.h
class Movement;
class member_mvt; //defined in movement.h
class Profile;
typedef struct _xmlNode xmlNode;

class Member
{
private:
  /* If you need this, implement it (correctly) */
  Member operator = (const Member &);
  /**********************************************/

  Member* parent;
  Double  angle_rad;
  Double   alpha;
  bool    go_through_ground;
  std::map<std::string, v_attached> attached_members;
  Point2f pos;
  Point2f scale;

protected:
  Sprite*     spr;
  std::string name;
  std::string type;
  Point2f     anchor;

public:

  virtual ~Member();
  Member(const xmlNode *     xml, 
         const std::string & main_folder);
  Member(const Member & m);

  virtual void Draw(const Point2i & _pos, 
                    int             flip_x, 
                    LRDirection   direction);

  void RotateSprite();
  void ResetMovement();
  void ApplySqueleton(Member* parent_member);
  void ApplyMovement(const member_mvt &                mvt, 
                     std::vector<class c_junction *> & skel_lst);
  void SetAngle(const Double & angle);
  void RefreshSprite(LRDirection direction);

  void SetPos(const Point2f & pos);

  const Sprite & GetSprite() const;

  const Point2i GetPos() const;
  const Point2f & GetPosFloat() const;

  const Point2i GetAnchorPos() const;

  const std::string & GetName() const;
  const std::string & GetType() const;

  bool IsGoingThroughGround() const;

  const std::map<std::string, v_attached> & GetAttachedMembers() const;
};

class WeaponMember : public Member
{
public:
  WeaponMember(void);
  void Draw(const Point2i & _pos, 
            int             flip_x, 
            LRDirection   direction);
};

#endif //MEMBER_H
