/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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
 * Téléportation : déplacement d'un ver n'importe où sur le terrain.
 *****************************************************************************/

#ifndef NINJA_ROPE_H
#define NINJA_ROPE_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include "weapon.h"
//-----------------------------------------------------------------------------
namespace Wormux {
//-----------------------------------------------------------------------------

const int node_max=200;

class NinjaRope : public Weapon
{
private:
  struct s_rope_node
  {
    int x,y;
    double angle ;
    int sense ;
  };

  uint last_mvt;
  double last_broken_node_angle;
  double last_broken_node_sense;

  // Rope launching data.
  bool m_attaching;
  bool m_rope_attached;
  double m_initial_angle;
  uint m_launch_time;
  uint m_hooked_time;
  Sprite* m_hook_sprite;
  Sprite* m_node_sprite;
  Sprite* skin;

public:
  s_rope_node rope_node[node_max];
  int last_node;
  int m_fixation_x, m_fixation_y;
  bool go_left, go_right ;
  double delta_len ;

  NinjaRope();
  void Active();
  void Refresh();
  void p_Init();
  void p_Deselect();
  void Draw();
  void HandleKeyEvent(int key, int event_type) ;
  void NotifyMove(bool collision);
  bool p_Shoot();

  EmptyWeaponConfig& cfg();
protected:
  void LoadExtraXml(xmlpp::Element *elem);
  void GoUp();
  void GoDown();
  void GoLeft();
  void GoRight();
  void StopLeft();
  void StopRight();
  void TryAttachRope();
  void UnattachRope();
  bool TryAddNode(int CurrentSense) ;
  bool TryBreakNode(int CurrentSense) ;
  void SignalTurnEnd() ;
  void InitSkinSprite();
};

extern NinjaRope ninjarope;
//-----------------------------------------------------------------------------
} // namespace Wormux
#endif
