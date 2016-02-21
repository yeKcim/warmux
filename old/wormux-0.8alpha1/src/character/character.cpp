/******************************************************************************
 *  Wormux is a convivial mass murder game.
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
 * Refresh d'un ver de terre.
 *****************************************************************************/

#include "character.h"
#include <SDL.h>
#include <sstream>
#include <iostream>
#include "body.h"
#include "../team/macro.h"
#include "move.h"
#include "../game/game.h"
#include "../game/game_mode.h"
#include "../game/game_loop.h"
#include "../game/time.h"
#include "../game/config.h"
#include "../graphic/text.h"
#include "../graphic/font.h"
#include "../include/action_handler.h"
#include "../include/app.h"
#include "../include/constant.h"
#include "../interface/mouse.h"
#include "../interface/interface.h"
#include "../interface/cursor.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../map/water.h"
#include "../network/network.h"
#include "../network/randomsync.h"
#include "../sound/jukebox.h"
#include "../tool/debug.h"
#include "../tool/random.h"
#include "../tool/math_tools.h"
#include "../weapon/suicide.h"
#include "../weapon/crosshair.h"
#include "../weapon/explosion.h"

const uint HAUT_FONT_MIX = 13;

// Space between the name, the skin and the energy bar
const uint ESPACE = 3; // pixels
const uint do_nothing_timeout = 5000;
const double MIN_SPEED_TO_FLY = 15.0;

// Pause for the animation
#ifdef DEBUG
//#define ANIME_VITE
#endif
#ifdef ANIME_VITE
  const uint ANIM_PAUSE_MIN = 100;
  const uint ANIM_PAUSE_MAX = 150;
#else
  const uint ANIM_PAUSE_MIN = 5*1000;
  const uint ANIM_PAUSE_MAX = 60*1000;
#endif

#ifdef DEBUG
//#define DEBUG_STATS
#endif

// Energy bar
const uint LARG_ENERGIE = 40;
const uint HAUT_ENERGIE = 6;

Character::Character (Team& my_team, const std::string &name) :
  PhysicalObj("character"), m_team(my_team), bubble_engine(500)
{
  SetCollisionModel(false, true, true);

  body = NULL;
  step_sound_played = true;
  pause_bouge_dg = 0;
  previous_strength = 0;
  energy = 100;
  channel_step = -1;
  hidden = false;
  do_nothing_time = 0;
  m_allow_negative_y = true;
  animation_time = Time::GetInstance()->Read() + randomObj.GetLong(ANIM_PAUSE_MIN,ANIM_PAUSE_MAX);
  prepare_shoot = false;
  back_jumping = false;
  crosshair_angle = 0;

  // Damage count
  damage_other_team = 0;
  damage_own_team = 0;
  max_damage = 0;
  current_total_damage = 0;

  // Disease damage
  disease_damage_per_turn = 0;
  disease_duration = 0;

  // Survivals
  survivals = 0;

  character_name = name;

  ResetConstants();

  // Name Text object
  if (Config::GetInstance()->GetDisplayNameCharacter())
    name_text = new Text(character_name);
  else
    name_text = NULL;

  // Energy
  energy_bar.InitVal (energy, 0, GameMode::GetInstance()->character.max_energy);
  energy_bar.InitPos (0,0, LARG_ENERGIE, HAUT_ENERGIE);

  energy_bar.SetBorderColor( black_color );
  energy_bar.SetBackgroundColor( gray_color );

  energy = GameMode::GetInstance()->character.init_energy;
  energy_bar.InitVal (energy, 0, GameMode::GetInstance()->character.init_energy);
  lost_energy = 0;
  SetEnergyDelta(0, false); // This is needed to setup the colors of the energy-bar
  MSG_DEBUG("character", "Load character %s", character_name.c_str());
}

Character::Character (const Character& acharacter) : PhysicalObj(acharacter),
                                                     m_team(acharacter.m_team),
                                                     bubble_engine(250)
{
  character_name       = acharacter.character_name;
  step_sound_played    = acharacter.step_sound_played;
  prepare_shoot        = acharacter.prepare_shoot;
  back_jumping         = acharacter.back_jumping;
  energy               = acharacter.energy;
  crosshair_angle      = acharacter.crosshair_angle;
  damage_other_team    = acharacter.damage_other_team;
  damage_own_team      = acharacter.damage_own_team;
  max_damage           = acharacter.max_damage;
  current_total_damage = acharacter.current_total_damage;
  energy_bar           = acharacter.energy_bar;
  survivals            = acharacter.survivals;
  pause_bouge_dg       = acharacter.pause_bouge_dg;
  do_nothing_time      = acharacter.do_nothing_time;
  animation_time       = acharacter.animation_time;
  lost_energy          = acharacter.lost_energy;
  hidden               = acharacter.hidden;
  channel_step         = acharacter.channel_step ;
  previous_strength    = acharacter.previous_strength;
  disease_damage_per_turn = acharacter.disease_damage_per_turn;
  disease_duration     = acharacter.disease_duration;

  if (acharacter.body)
    SetBody( new Body(*acharacter.body) );
  if(acharacter.name_text)
    name_text = new Text(*acharacter.name_text);
}

Character::~Character()
{
  MSG_DEBUG("character", "Unload character %s", character_name.c_str());
  if(body)
    delete body;
  if(name_text)
    delete name_text;
  body      = NULL;
  name_text = NULL;
}

void Character::SetBody(Body* _body)
{
  body = _body;
  body->owner = this;
  SetClothe("normal");
  SetMovement("walk");

  SetDirection( randomSync.GetBool()?1:-1 );
  body->SetFrame( 0 );
  SetSize(body->GetSize());
}

void Character::SignalDrowning()
{
  energy = 0;
  SetMovement("drowned");

  jukebox.Play (GetTeam().GetSoundProfile(),"sink");
  GameLoop::GetInstance()->SignalCharacterDeath (this);
}

// Si un ver devient un fantome, il meurt ! Signale sa mort
void Character::SignalGhostState (bool was_dead)
{
  // Report to damage performer this character lost all of its energy
  ActiveCharacter().MadeDamage(energy, *this);

  MSG_DEBUG("character", "ghost");

  // Signal the death
  if (!was_dead) GameLoop::GetInstance()->SignalCharacterDeath (this);
}

void Character::SetDirection (int nv_direction)
{
  body->SetDirection(nv_direction);
  uint l,r,t,b;
  body->GetTestRect(l,r,t,b);
  SetTestRect(l,r,t,b);
}

void Character::DrawEnergyBar(int dy)
{
  if( IsDead() )
	return;

  energy_bar.DrawXY( Point2i( GetCenterX() - energy_bar.GetWidth() / 2, GetY() + dy)
		  - camera.GetPosition() );
}

void Character::DrawName (int dy) const
{
  if(IsDead()) return;

  const int x =  GetCenterX();
  const int y = GetY()+dy;

  if (Config::GetInstance()->GetDisplayNameCharacter())
  {
    name_text->DrawCenterTopOnMap(x,y);
  }
}

void Character::SetEnergyDelta (int delta, bool do_report)
{
  // If already dead, do nothing
  if (IsDead()) return;

  // Report damage to damage performer
  if (do_report)
    ActiveCharacter().MadeDamage(-delta, *this);

  uint sauve_energie = energy;
  Color color;

  // Change energy
  energy = BorneLong((int)energy +delta, 0, GameMode::GetInstance()->character.max_energy);
  energy_bar.Actu (energy);

  // Energy bar color
  if (70 < energy)
	  color.SetColor(0, 255, 0, 255);
  else if (50 < energy)
	  color.SetColor(255, 255, 0, 255);
  else if (20 < energy)
	  color.SetColor(255, 128, 0, 255);
  else
	  color.SetColor(255, 0, 0, 255);

  energy_bar.SetValueColor( color );


  // Compute energy lost
  if (delta < 0)
  {

    lost_energy += (int)energy - (int)sauve_energie;

    if ( lost_energy > -33 )
      jukebox.Play (GetTeam().GetSoundProfile(), "injured_light");
    else if ( lost_energy > -66 )
      jukebox.Play (GetTeam().GetSoundProfile(), "injured_medium");
    else
      jukebox.Play (GetTeam().GetSoundProfile(), "injured_high");

  }
  else
    lost_energy = 0;

  // "Friendly fire !!"
  if ( (&ActiveCharacter() != this) && ActiveTeam().IsSameAs(m_team) )
  jukebox.Play (GetTeam().GetSoundProfile(), "friendly_fire");

  // Dead character ?
  if (energy == 0) Die();
}

void Character::SetEnergy(int new_energy)
{
  if(! network.IsLocal() )
  {
    if( m_alive == DEAD && new_energy > 0)
    {
      printf("%s have been resurrected\n", GetName().c_str());
      m_alive = ALIVE;
      SetClothe("normal");
      SetMovement("walk");
    }
  }

  //assert( m_alive != DEAD );
  if(IsDead()) return;

  // Change energy
  energy = BorneLong((int)new_energy, 0, GameMode::GetInstance()->character.max_energy);
  energy_bar.Actu (energy);

  // Energy bar color
  Color color;
  if (70 < energy)
	  color.SetColor(0, 255, 0, 255);
  else if (50 < energy)
	  color.SetColor(255, 255, 0, 255);
  else if (20 < energy)
	  color.SetColor(255, 128, 0, 255);
  else
	  color.SetColor(255, 0, 0, 255);

  energy_bar.SetValueColor( color );
}

bool Character::GotInjured() const
{
	return lost_energy < 0;
}

void Character::Die()
{
  assert (m_alive == ALIVE || m_alive == DROWNED);

  MSG_DEBUG("character", "Dying");

  if (m_alive != DROWNED)
  {
    m_alive = DEAD;

    energy = 0;

    jukebox.Play(GetTeam().GetSoundProfile(),"death");
    SetClothe("dead");
    SetMovement("dead");

    ExplosiveWeaponConfig cfg;
    ApplyExplosion ( GetCenter(), cfg);
    assert (IsDead());

    // Signal the death
    GameLoop::GetInstance()->SignalCharacterDeath (this);
  }
}

bool Character::IsDiseased() const
{
  return disease_duration > 0;
}

void Character::SetDiseaseDamage(const uint damage_per_turn, const uint duration)
{
  disease_damage_per_turn = damage_per_turn;
  disease_duration = duration;
}

// Keep almost 1 in energy
uint Character::GetDiseaseDamage() const
{
  if (disease_damage_per_turn < energy) return disease_damage_per_turn;
  return energy - 1;
}

void Character::DecDiseaseDuration()
{
  if (disease_duration > 0) disease_duration--;
  else disease_damage_per_turn = 0;
}

void Character::Draw()
{
  if (hidden) return;

  // Gone in another world ?
  if (IsGhost()) return;

  bool dessine_perte = (lost_energy != 0);
  if ((&ActiveCharacter() == this
    && GameLoop::GetInstance()->ReadState() != GameLoop::END_TURN)
      //&& (game_loop.ReadState() != jeuANIM_FIN_TOUR)
    || IsDead()
     )
    dessine_perte = false;

  if(GameLoop::GetInstance()->ReadState() == GameLoop::END_TURN && body->IsWalking())
    body->ResetWalk();

  if(Time::GetInstance()->Read() > animation_time && &ActiveCharacter()!=this && !IsDead()
  && body->GetMovement().substr(0,9) != "animation"
  &&  body->GetClothe().substr(0,9) != "animation")
  {
    body->PlayAnimation();
    animation_time = Time::GetInstance()->Read() + body->GetMovementDuration() + randomObj.GetLong(ANIM_PAUSE_MIN,ANIM_PAUSE_MAX);
  }

  // Stop the animation or the black skin if we are playing
  if(&ActiveCharacter() == this
  &&(body->GetMovement().substr(0,9) == "animation"
  || body->GetClothe().substr(0,9) == "animation"
  || body->GetClothe() == "black"))
  {
    SetClothe("normal");
    SetMovement("walk");
  }

  // Stop flying if we don't go fast enough
  double n, a;
  GetSpeed(n, a);
  if(body->GetMovement() == "fly" && n < MIN_SPEED_TO_FLY)
    SetMovement("walk");


  Point2i pos = GetPosition();

  if(prepare_shoot)
  {
    body->Build(); // Refresh the body
    if(body->GetMovement() != "weapon-" + ActiveTeam().GetWeapon().GetID() + "-begin-shoot")
    {
      // if the movement is finnished, shoot !
      DoShoot();
      prepare_shoot = false;
    }
  }

  body->Draw(pos);

   // Draw energy bar
  int dy = -ESPACE;
  bool est_ver_actif = (this == &ActiveCharacter());
  Config * config = Config::GetInstance();
  bool display_energy = config->GetDisplayEnergyCharacter();
  display_energy &= !est_ver_actif || (GameLoop::GetInstance()->ReadState() != GameLoop::PLAYING);
  display_energy |= dessine_perte;
  display_energy &= !IsDead();
  if (display_energy)
  {
    dy -= HAUT_ENERGIE;
    DrawEnergyBar (dy);
    dy -= ESPACE;
  }

  // Draw name
  if (config->GetDisplayNameCharacter() && !est_ver_actif)
  {
    dy -= HAUT_FONT_MIX;
    DrawName (dy);
    dy -= ESPACE;
  }

  // Draw lost energy
  if (dessine_perte)
  {
    std::ostringstream ss;
    ss << lost_energy;
    dy -= HAUT_FONT_MIX;
    (*Font::GetInstance(Font::FONT_SMALL)).WriteCenterTop (
			GetPosition() - camera.GetPosition() + Point2i( GetWidth()/2, dy),
		   	ss.str(), white_color);
  }

}

void Character::Jump(double strength, int deg_angle)
{
  do_nothing_time = Time::GetInstance()->Read();

  if (!CanJump() && ActiveTeam().is_local) return;

  SetMovement("jump");

  // Jump !
  double angle = Deg2Rad(deg_angle);
  if (GetDirection() == -1) angle = InverseAngle(angle);
  SetSpeed (strength, angle);
}

void Character::Jump()
{
  MSG_DEBUG("character", "Jump");
  jukebox.Play (ActiveTeam().GetSoundProfile(), "jump");
  Jump(GameMode::GetInstance()->character.jump_strength,
       GameMode::GetInstance()->character.jump_angle);
}

void Character::HighJump()
{
  MSG_DEBUG("character", "HighJump");
  jukebox.Play (ActiveTeam().GetSoundProfile(), "superjump");
  Jump(GameMode::GetInstance()->character.super_jump_strength,
       GameMode::GetInstance()->character.super_jump_angle);
}

void Character::BackJump()
{
  MSG_DEBUG("character", "BackJump");
  back_jumping = true;
  jukebox.Play (ActiveTeam().GetSoundProfile(), "jump");
  Jump(GameMode::GetInstance()->character.back_jump_strength,
       GameMode::GetInstance()->character.back_jump_angle);
}

void Character::PrepareShoot()
{
  SetMovementOnce("weapon-" + ActiveTeam().GetWeapon().GetID() + "-begin-shoot");
  if(body->GetMovement() != "weapon-" + ActiveTeam().GetWeapon().GetID() + "-begin-shoot")
  {
    // If a movement is defined for this weapon, just shoot
    DoShoot();
  }
  else
    prepare_shoot = true;
}

void Character::DoShoot()
{
  SetMovementOnce("weapon-" + ActiveTeam().GetWeapon().GetID() + "-end-shoot");
  ActiveTeam().AccessWeapon().Shoot();
}

void Character::HandleShoot(int event_type)
{
  if(prepare_shoot)
    return;

  switch (event_type) {
    case KEY_PRESSED:
      if (ActiveTeam().GetWeapon().max_strength == 0)
        ActiveTeam().GetWeapon().NewActionShoot();
      else
      if ( (GameLoop::GetInstance()->ReadState() == GameLoop::PLAYING)
         && ActiveTeam().GetWeapon().IsReady() )
        ActiveTeam().AccessWeapon().InitLoading();
      break ;

    case KEY_RELEASED:
      if (ActiveTeam().GetWeapon().IsLoading())
        ActiveTeam().GetWeapon().NewActionShoot();
      break ;

    case KEY_REFRESH:
      if ( ActiveTeam().GetWeapon().IsLoading() )
	{
	  // Strength == max strength -> Fire !!!
	  if (ActiveTeam().GetWeapon().ReadStrength() >=
	      ActiveTeam().GetWeapon().max_strength)
            ActiveTeam().GetWeapon().NewActionShoot();
	  else
	    {
	      // still pressing the Space key
	      ActiveTeam().AccessWeapon().UpdateStrength();
	    }
	}
      break ;

    default:
      break ;
  }
}

void Character::HandleKeyEvent(int action, int event_type)
{
  // The character cannot move anymove if the turn is over...
  if (GameLoop::GetInstance()->ReadState() == GameLoop::END_TURN)
    return ;

  if (ActiveCharacter().IsDead())
    return;

  if (action == ACTION_SHOOT)
    {
      HandleShoot(event_type);
      do_nothing_time = Time::GetInstance()->Read();
      CharacterCursor::GetInstance()->Hide();
      return;
    }

  ActionHandler * action_handler = ActionHandler::GetInstance();

  if(action <= ACTION_NEXT_CHARACTER)
    {
      switch (event_type)
      {
        case KEY_REFRESH:
          Mouse::GetInstance()->Hide();
          Interface::GetInstance()->Hide();
          switch (action) {
            case ACTION_MOVE_LEFT:
              if(ActiveCharacter().IsImmobile())
                MoveCharacterLeft(ActiveCharacter());
              return;
            case ACTION_MOVE_RIGHT:
              if(ActiveCharacter().IsImmobile())
                MoveCharacterRight(ActiveCharacter());
              return;

            default:
	      break ;
          }
          //no break!! -> it's normal
        case KEY_PRESSED:
          Mouse::GetInstance()->Hide();
          Interface::GetInstance()->Hide();
          switch (action)
          {
            case ACTION_UP:
              if(ActiveCharacter().IsImmobile())
              {
                if (ActiveTeam().crosshair.enable)
                {
                  do_nothing_time = Time::GetInstance()->Read();
                  CharacterCursor::GetInstance()->Hide();
                  action_handler->NewAction (new Action(ACTION_UP));
                  crosshair_angle = ActiveTeam().crosshair.GetAngleVal();
                }
              }
              break ;

            case ACTION_DOWN:
              if(ActiveCharacter().IsImmobile())
              {
                if (ActiveTeam().crosshair.enable)
                {
                  do_nothing_time = Time::GetInstance()->Read();
                  CharacterCursor::GetInstance()->Hide();
                  action_handler->NewAction (new Action(ACTION_DOWN));
                  crosshair_angle = ActiveTeam().crosshair.GetAngle();
                }
              }
              break ;
            case ACTION_MOVE_LEFT:
            case ACTION_MOVE_RIGHT:
              InitMouvementDG(PAUSE_BOUGE);
              body->StartWalk();
              break;
            // WARNING!! ALL JUMP KEYS NEEDS TO BE PROCESSED AFTER ANY MOVEMENT KEYS
            // OTHERWISE, THE JUMP ACTION WILL BYPASSED ON DISTANT COMPUTERS BYE THE REFRESH
            // OF THE WALK
            case ACTION_JUMP:
              if(ActiveCharacter().IsImmobile())
                action_handler->NewAction (new Action(ACTION_JUMP));
	            return ;
            case ACTION_HIGH_JUMP:
              if(ActiveCharacter().IsImmobile())
                action_handler->NewAction (new Action(ACTION_HIGH_JUMP));
              return ;
            case ACTION_BACK_JUMP:
              if(ActiveCharacter().IsImmobile())
                action_handler->NewAction (new Action(ACTION_BACK_JUMP));
              return ;
            default:
      	      break;
          }
          break;

        case KEY_RELEASED:
          switch (action) {
            case ACTION_MOVE_LEFT:
            case ACTION_MOVE_RIGHT:
               body->StopWalk();
               SendCharacterPosition();
               break;
            }
        default: break;
      }
    }
}

void Character::Refresh()
{
  if (IsGhost()) return;

  UpdatePosition ();
  Time * global_time = Time::GetInstance();

  if(IsDiseased())
  {
    Point2i bubble_pos = GetPosition();
    if(GetDirection() == -1)
      bubble_pos.x += GetWidth();
    bubble_engine.AddPeriodic(bubble_pos, particle_ILL_BUBBLE, false,
                              - M_PI_2 - (float)GetDirection() * M_PI_4, 20.0);
  }

  if( IsActiveCharacter() && GameLoop::GetInstance()->ReadState() == GameLoop::PLAYING)
  {
    if(do_nothing_time + do_nothing_timeout < global_time->Read())
      CharacterCursor::GetInstance()->FollowActiveCharacter();
  }

  if(body->IsWalking())
  {
    // Play the step sound only twice during the walking animation
    uint frame_nbr = body->GetFrameCount();
    uint cur = body->GetFrame();
    frame_nbr /= 2;
    cur %= frame_nbr;

    if(cur < frame_nbr / 2 && !step_sound_played)
    {
      step_sound_played = true;
      jukebox.Play (GetTeam().GetSoundProfile(),"step");
    }

    if(cur > frame_nbr / 2)
      step_sound_played = false;
  }

  if(back_jumping)
  {
    assert(&ActiveCharacter() == this);
    int rotation;
    static double speed_init = GameMode::GetInstance()->character.back_jump_strength *
       sin(GameMode::GetInstance()->character.back_jump_angle);

    Point2d speed;
    GetSpeedXY(speed);
    rotation = - (int)(180.0 * speed.y / speed_init);
    body->SetRotation(rotation);
  }
}

// Prepare a new turn
void Character::PrepareTurn ()
{
  HandleMostDamage();
  lost_energy = 0;
  pause_bouge_dg = Time::GetInstance()->Read();
}

const Team &Character::GetTeam() const
{
  return m_team;
}

bool Character::MouvementDG_Autorise() const
{
  if (!IsImmobile() || IsFalling()) return false;
  return pause_bouge_dg < Time::GetInstance()->Read();
}

bool Character::CanJump() const
{
	return MouvementDG_Autorise();
}

void Character::InitMouvementDG(uint pause)
{
  do_nothing_time = Time::GetInstance()->Read();
  CharacterCursor::GetInstance()->Hide();
  step_sound_played = true;
  pause_bouge_dg = Time::GetInstance()->Read()+pause;
}

bool Character::CanStillMoveDG(uint pause)
{
  if(pause_bouge_dg+pause<Time::GetInstance()->Read())
  {
    pause_bouge_dg += pause;
    return true;
  }
  return false;
}

// Signal the end of a fall
void Character::SignalCollision()
{
  // Do not manage dead characters.
  if (IsDead()) return;

  pause_bouge_dg = Time::GetInstance()->Read();
  back_jumping = false;
  double norme, degat;
  Point2d speed_vector;
  GameMode * game_mode = GameMode::GetInstance();
  SetMovement("walk");
  SetMovementOnce("soft-land");

  GetSpeedXY (speed_vector);
  norme = speed_vector.Norm();
  if (norme > game_mode->safe_fall && speed_vector.y>0.0)
  {
    norme -= game_mode->safe_fall;
    degat = norme * game_mode->damage_per_fall_unit;
    SetEnergyDelta (-(int)degat);
    GameLoop::GetInstance()->SignalCharacterDamage(this);
    SetMovement("walk");
    SetMovementOnce("hard-land");
  }
}

void Character::SignalExplosion()
{
  if(IsDead()) return;

  double n, a;
  GetSpeed(n, a);
  SetRebounding(true);
  if(n > MIN_SPEED_TO_FLY)
    SetMovement("fly");
  else
  {
    SetClotheOnce("black");
    SetMovementOnce("black");
    if(body->GetClothe() == "black"
    && body->GetMovement() != "black")
      std::cerr << "Error: the clothe \"black\" of the character " << GetName() << " is set, but the skin have no \"black\" movement !!!" << std::endl;
  }
  // bug #7056 : When we are hit by an explosion while using ninja rope, we broke the rope.
  if (IsActiveCharacter()) {
    ActiveTeam().AccessWeapon().Deselect();
    // Select the weapon back. If not, we cannot move the crosshair.
    ActiveTeam().AccessWeapon().Select();
    // End of turn
    GameLoop::GetInstance()->SignalCharacterDamage(this);
  }
}

int Character::GetDirection() const
{
  return body->GetDirection();
}

// End of turn or change of character
void Character::StopPlaying()
{
  if(IsDead()) return;
  SetClothe("normal");
  SetMovement("walk");
  body->ResetWalk();
  SetRebounding(true);
}

// Begining of turn or changed to this character
void Character::StartPlaying()
{
  assert (!IsGhost());
  SetWeaponClothe();
  SetRebounding(false);
  ActiveTeam().crosshair.ChangeAngleVal(crosshair_angle);
}

uint Character::GetEnergy() const
{
//  assert (!IsDead());
  return energy;
}

bool Character::IsActiveCharacter() const
{
  return this == &ActiveCharacter();
}

// Hand position
const Point2i & Character::GetHandPosition() const {
  return body->GetHandPosition();
}

// Hand position
void Character::GetHandPositionf (double &x, double &y)
{
  x = (double) GetHandPosition().x;
  x = (double) GetHandPosition().y;
}

void Character::HandleMostDamage()
{
  if (current_total_damage > max_damage)
  {
    max_damage = current_total_damage;
  }
#ifdef DEBUG_STATS
  std::cerr << m_name << " most damage: " << max_damage << std::endl;
#endif
  current_total_damage = 0;
}

void Character::Hide() { hidden = true; }
void Character::Show() { hidden = false; }

void Character::MadeDamage(const int Dmg, const Character &other)
{
  if (m_team.IsSameAs(other.GetTeam()))
  {
#ifdef DEBUG_STATS
    std::cerr << m_name << " damaged own team with " << Dmg << std::endl;
#endif
    if (Character::IsSameAs(other))
      damage_own_team += Dmg;
  }
  else
  {
#ifdef DEBUG_STATS
    std::cerr << m_name << " damaged other team with " << Dmg << std::endl;
#endif
    damage_other_team += Dmg;
  }

  current_total_damage += Dmg;
}

void Character::SetWeaponClothe()
{
  SetClothe("weapon-" + m_team.GetWeapon().GetID());
  if(body->GetClothe() != "weapon-" + m_team.GetWeapon().GetID())
    SetClothe("normal");
}

void Character::SetMovement(std::string name)
{
  if(IsDead()) return;
  MSG_DEBUG("body","Character %s -> SetMovement : %s",character_name.c_str(),name.c_str());
  body->SetMovement(name);
  uint l,r,t,b;
  body->GetTestRect(l,r,t,b);
  SetTestRect(l,r,t,b);
}

void Character::SetMovementOnce(std::string name)
{
  if(IsDead()) return;
  MSG_DEBUG("body","Character %s -> SetMovementOnce : %s",character_name.c_str(),name.c_str());
  body->SetMovementOnce(name);
  uint l,r,t,b;
  body->GetTestRect(l,r,t,b);
  SetTestRect(l,r,t,b);
}

void Character::SetClothe(std::string name)
{
  if(IsDead() && name!="dead") return;
  MSG_DEBUG("body","Character %s -> SetClothe : %s",character_name.c_str(),name.c_str());
  body->SetClothe(name);
}

void Character::SetClotheOnce(std::string name)
{
  if(IsDead()) return;
  MSG_DEBUG("body","Character %s -> SetClotheOnce : %s",character_name.c_str(),name.c_str());
  body->SetClotheOnce(name);
}

uint Character::GetTeamIndex()
{
  uint index = 0;
  teams_list.FindPlayingById( GetTeam().GetId(), index);
  return index;
}

uint Character::GetCharacterIndex()
{
  uint index = 0;
  for(Team::iterator it = m_team.begin();
                     it != m_team.end() ; ++it, ++index )
  {
    if( &(*it) == this)
      return index;
  }
  assert(false);
  return 0;
}
