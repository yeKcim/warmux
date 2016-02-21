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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * Grapple
 *****************************************************************************/

#ifndef GRAPPLE_H
#define GRAPPLE_H
//-----------------------------------------------------------------------------
#include "weapon.h"
#include "include/base.h"
//-----------------------------------------------------------------------------

class GrappleConfig : public EmptyWeaponConfig
{
 public:
  uint max_rope_length; // Max rope length in pixels
  uint automatic_growing_speed; // Pixel per 1/100 second.
  int push_force;

 public:
  GrappleConfig();
  void LoadXml(xmlpp::Element *elem);
};

//-----------------------------------------------------------------------------

class Grapple : public Weapon
{
  private:
    typedef struct 
    {
      Point2i pos;
      double angle;
      int sense;
    } rope_node_t;

    uint last_mvt;
    double last_broken_node_angle;
    double last_broken_node_sense;

    // Rope launching data.
    bool m_attaching;
    double m_initial_angle;
    uint m_launch_time;
    uint m_hooked_time;
    Sprite* m_hook_sprite;
    Sprite* m_node_sprite;

  protected:
    void Refresh();
    void p_Deselect();
    bool p_Shoot();

    void GoUp();
    void GoDown();
    void GoLeft();
    void GoRight();
    void StopLeft();
    void StopRight();

    bool WillBeAttached();
    bool TryAttachRope();
    bool TryAddNode(int CurrentSense) ;
    bool TryBreakNode(int CurrentSense) ;

  public:
    enum {
      ATTACH_ROPE,
      ATTACH_NODE,
      DETACH_NODE,
      SET_ROPE_SIZE,
      UPDATE_PLAYER_POSITION
    } grapple_movement_t;

    std::list<rope_node_t> rope_nodes;
    Point2i m_fixation_point;
    bool go_left, go_right;
    double delta_len ;

    Grapple();
    void Draw();
    virtual void NotifyMove(bool collision);
    
    virtual void ActionStopUse();
    virtual void SignalTurnEnd();
    
    GrappleConfig& cfg();

    // Attaching and dettaching nodes rope
    // This is public because of network
    void AttachRope(Point2i contact_point);
    void DetachRope();

    void AttachNode(Point2i contact_point, 
		    double angle, 
		    int sense);
    void DetachNode();
    void SetRopeSize(double length);

    // Keys management
    virtual void HandleKeyPressed_Up();
    virtual void HandleKeyRefreshed_Up();
    virtual void HandleKeyReleased_Up();
    
    virtual void HandleKeyPressed_Down();
    virtual void HandleKeyRefreshed_Down();
    virtual void HandleKeyReleased_Down();

    virtual void HandleKeyPressed_MoveRight();
    virtual void HandleKeyRefreshed_MoveRight();
    virtual void HandleKeyReleased_MoveRight();
    
    virtual void HandleKeyPressed_MoveLeft();
    virtual void HandleKeyRefreshed_MoveLeft();
    virtual void HandleKeyReleased_MoveLeft();

    virtual void HandleKeyPressed_Shoot();
    virtual void HandleKeyRefreshed_Shoot();
    virtual void HandleKeyReleased_Shoot();

    void PrintDebugRope();

    DECLARE_GETWEAPONSTRING();
};

//-----------------------------------------------------------------------------
#endif /* GRAPPLE_H */
