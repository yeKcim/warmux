/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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
 * Add a structure to the ground
 *****************************************************************************/

#include "weapon/construct.h"
#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"

#include "character/character.h"
#include "game/game_mode.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "include/action_handler.h"
#include "interface/interface.h"
#include "interface/mouse.h"
#include "map/camera.h"
#include "map/map.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"

#ifdef DEBUG
#include "graphic/video.h"
#include "graphic/colors.h"
#include "include/app.h"
#endif

const double DELTA_ANGLE = M_PI / 6.0; // should be a multiple


Construct::Construct() : Weapon(WEAPON_CONSTRUCT, "construct",
                                new WeaponConfig(),
                                NEVER_VISIBLE)
{
  UpdateTranslationStrings();

  construct_spr = resource_manager.LoadSprite( weapons_res_profile, "construct_spr");
  construct_spr->EnableRotationCache(static_cast<int>(2 * M_PI / DELTA_ANGLE));
  m_name = _("Construct");
  m_category = TOOL;
  angle = 0;
  target_chosen = false;
}

void Construct::UpdateTranslationStrings()
{
  m_name = _("Construct");

  /* TODO: FILL IT */
  /* m_help = */
}

Construct::~Construct()
{
  delete construct_spr;
}

std::string Construct::GetWeaponWinString(const char *TeamName, uint items_count) const
{
  return Format(ngettext(
            "%s team has won %u construct weapon! Don't forget your helmet.",
            "%s team has won %u construct weapons! Don't forget your helmet.",
            items_count), TeamName, items_count);
}

bool Construct::p_Shoot ()
{
  if(!target_chosen)
    return false;
  JukeBox::GetInstance()->Play("share", "weapon/construct");
  world.MergeSprite(dst - construct_spr->GetSizeMax()/2, construct_spr);

  target_chosen = false; // ensure next shoot cannot be done pressing key space
  return true;
}

void Construct::Draw()
{
  if (!IsInUse()) {
    Weapon::Draw();

    if (EnoughAmmo()
	&& EnoughAmmoUnit()
	&& !Interface::GetInstance()->weapons_menu.IsDisplayed()
	&& Interface::GetInstance()->IsDisplayed()) {
      dst = Mouse::GetInstance()->GetWorldPosition();
      construct_spr->SetRotation_rad(angle);
      construct_spr->Draw(dst - construct_spr->GetSize() / 2);

#ifdef DEBUG
      if (IsLOGGING("test_rectangle"))
	{
	  Rectanglei test_rect(dst - construct_spr->GetSizeMax() / 2, construct_spr->GetSizeMax());
	  test_rect.SetPosition(test_rect.GetPosition() - Camera::GetInstance()->GetPosition());
	  GetMainWindow().RectangleColor(test_rect, primary_red_color, 1);
	}
#endif
    }
  }
}

void Construct::ChooseTarget(Point2i mouse_pos)
{
  dst = mouse_pos;

  Point2i test_target = dst - construct_spr->GetSizeMax() / 2;
  Rectanglei rect(test_target, construct_spr->GetSizeMax());

  if (!world.ParanoiacRectIsInVacuum(rect))
    return;

  // Check collision with characters and other physical objects
  FOR_ALL_CHARACTERS(team, c) {
    if ((c->GetTestRect()).Intersect(rect))
      return;
  }

  FOR_ALL_OBJECTS(it) {
    PhysicalObj *obj = *it;
    if ((obj->GetTestRect()).Intersect(rect))
      return;
  }

  target_chosen = true;
  Shoot();
}

void Construct::Up() const
{
  double new_angle = angle + DELTA_ANGLE;

  Action* a = new Action(Action::ACTION_WEAPON_CONSTRUCTION, new_angle);
  ActionHandler::GetInstance()->NewAction(a);
}

void Construct::Down() const
{
  double new_angle = angle - DELTA_ANGLE;

  Action* a = new Action(Action::ACTION_WEAPON_CONSTRUCTION, new_angle);
  ActionHandler::GetInstance()->NewAction(a);
}

WeaponConfig& Construct::cfg()
{ return static_cast<WeaponConfig&>(*extra_params); }

bool Construct::IsInUse() const
{
  return m_last_fire_time > 0 && m_last_fire_time + m_time_between_each_shot > Time::GetInstance()->Read();
}
