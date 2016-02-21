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
 * Refresh d'un ver de terre.
 *****************************************************************************/

#include "character.h"
#include <SDL.h>
#include <sstream>
#include <iostream>
#include "macro.h"
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
#include "../interface/cursor.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../map/water.h"
#include "../sound/jukebox.h"
#include "../tool/debug.h"
#include "../tool/random.h"
#include "../tool/math_tools.h"
#include "../weapon/suicide.h"
#include "../weapon/crosshair.h"
#include "../weapon/weapon_tools.h"

const uint HAUT_FONT_MIX = 13;

// Space between the name, the skin and the energy bar
const uint ESPACE = 3; // pixels
const uint do_nothing_timeout = 5000;

#ifdef DEBUG
//#define ANIME_VITE
//#define DEBUG_CHG_ETAT
//#define DEBUG_PLACEMENT
//#define NO_POSITION_CHECK
//#define DEBUG_SKIN

#define COUT_DBG0 std::cerr << "[Character " << m_name << "]"
#define COUT_DBG COUT_DBG0 " "
#define COUT_PLACEMENT COUT_DBG0 "[Init bcl=" << bcl << "] "
#endif

// Pause for the animation
#ifdef ANIME_VITE
  const uint ANIM_PAUSE_MIN = 100;
  const uint ANIM_PAUSE_MAX = 150;
#else
  const uint ANIM_PAUSE_MIN = 5*1000;
  const uint ANIM_PAUSE_MAX = 60*1000;
#endif

// Barre d'énergie
const uint LARG_ENERGIE = 40;
const uint HAUT_ENERGIE = 6;

Character::Character () :
  PhysicalObj("Soldat inconnu", 0.0)
{
  pause_bouge_dg = 0;
  previous_strength = 0;
  m_team = NULL;
  energy = 100;
  lost_energy = 0;
  desactive = false;
  skin = NULL;
  walk_skin = NULL;
  m_wind_factor = 0.0;
  m_rebound_factor = 0.25;
  m_rebounding = true;
  skin_is_walking = true;
  is_walking = false;
  current_skin = "";
  channel_step = -1;
  hidden = false;
  image = NULL;
  do_nothing_time = 0;
  m_allow_negative_y = true;

  // Damage count
  damage_other_team = 0;
  damage_own_team = 0;
  max_damage = 0;
  current_total_damage = 0;

  // Survivals
  survivals = 0;

  name_text = NULL;
}

// Signale la mort d'un ver
void Character::SignalDeath()
{
  MSG_DEBUG("character", "Dying");

  // No more energy ...
  energy = 0;

  jukebox.Play(GetTeam().GetSoundProfile(),"death");

  SetSkin("dead");

  ExplosiveWeaponConfig cfg;

  ApplyExplosion ( GetCenter(), cfg, NULL);

  // Change test rectangle
  SetSize( image->GetSize() );
  SetXY( GetCenter() - GetSize()/2 );

  assert (m_alive == DEAD);
  assert (IsDead());
  
  // Signal the death
  GameLoop::GetInstance()->SignalCharacterDeath (this);
}

void Character::SignalDrowning()
{
  energy = 0;
  //m_type = objINSENSIBLE;
  SetSkin("drowned");

  jukebox.Play (GetTeam().GetSoundProfile(),"sink");
  GameLoop::GetInstance()->SignalCharacterDeath (this);
}

// Si un ver devient un fantome, il meurt ! Signale sa mort
void Character::SignalGhostState (bool was_dead)
{
  // Deactivate character
  desactive = true;
  
  // Report to damage performer this character lost all of its energy
  ActiveCharacter().MadeDamage(energy, *this);

  MSG_DEBUG("character", "ghost");

  // Signal the death
  if (!was_dead) GameLoop::GetInstance()->SignalCharacterDeath (this);
}

void Character::SetDirection (int nv_direction)
{ 
   image->Scale (nv_direction, 1);
   
   if (GetSkin().anim.utilise) 
     anim.image->Scale (nv_direction, 1);
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

void Character::SetEnergyDelta (int delta)
{
  // If already dead, do nothing
  if (IsDead()) return;

  // Report damage to damage performer
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

    if ( lost_energy < 33 )
      jukebox.Play (GetTeam().GetSoundProfile(), "injured_light");
    else if ( lost_energy < 66 )
      jukebox.Play (GetTeam().GetSoundProfile(), "injured_medium");
    else 
      jukebox.Play (GetTeam().GetSoundProfile(), "injured_high");
    
  }
  else 
    lost_energy = 0;

  // "Friendly fire !!"
  if ( (&ActiveCharacter() != this) && (&ActiveTeam() == m_team) )
  jukebox.Play (GetTeam().GetSoundProfile(), "friendly_fire");
   
  // Dead character ?
  if (energy == 0) Die();
}

void Character::StartBreathing()
{
  SetSkin("breathe");
  if(current_skin == "breathe")
  {
    image->animation.SetSpeedFactor((float)randomObj.GetLong(100,150)/100.0);
    image->animation.SetLoopMode(true);
    image->Start();
  }
}

void Character::StartWalking()
{
  do_nothing_time = Time::GetInstance()->Read();

  if(full_walk && current_skin=="breathe")
  {
    SetSkin("walking");
    image->Start();
    image->animation.SetLoopMode(true);
    image->animation.SetSpeedFactor(1.0);
    image->SetCurrentFrame(0);
  }
}

void Character::StopWalking()
{
  if(image!=NULL && current_skin=="walking" && full_walk)
  {
    image->Finish();
    image->SetCurrentFrame(0);
  }
}

void Character::Draw()
{
  if (hidden) return;

  // Gone in another world ?
  if (!IsActive()) return;

  bool dessine_perte = (lost_energy != 0);
  if ((&ActiveCharacter() == this
    && GameLoop::GetInstance()->ReadState() != GameLoop::END_TURN)
      //&& (game_loop.ReadState() != jeuANIM_FIN_TOUR)
    || IsDead()
     )
    dessine_perte = false;

  // Draw skin
  if(full_walk && !image->IsFinished() && GameLoop::GetInstance()->ReadState() == GameLoop::END_TURN)
    StopWalking();

  if(!skin_is_walking || full_walk) //walking skins image update only when a keyboard key is pressed
  {
    image->Update();
    if(current_skin=="walking" && image->IsFinished())
      StartBreathing();
  }

  if(current_skin=="hard_fall_ending") image->Update();

  if(current_skin=="hard_fall_ending" && image->IsFinished())
  if(!SetSkin("weapon-" + m_team->GetWeapon().GetID()))
    SetSkin("walking");

  Point2i pos = GetPosition();
  image->Draw(pos);
   
  // Draw animation
  if(anim.draw
  && (current_skin=="walking" || current_skin=="breathe")
  && (!GetSkin().anim.not_while_playing || &ActiveCharacter()!=this))
  {
    int dx = 0;
    if(GetDirection()==-1)
      dx = image->GetWidth() - anim.image->GetWidth();
    anim.image->Draw(pos + Point2i(dx, 0));
  }

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

void Character::Jump ()
{
  MSG_DEBUG("character", "Jump");
  do_nothing_time = Time::GetInstance()->Read();

  if (!CanJump()) return;

  jukebox.Play (ActiveTeam().GetSoundProfile(), "jump");
   
  m_rebounding = false;

  if(current_skin=="walking" || current_skin=="breathe")
    SetSkin("jump");

  // Initialise la force
  double angle = Deg2Rad(GameMode::GetInstance()->character.jump_angle);
  if (GetDirection() == -1) angle = InverseAngle(angle);
  SetSpeed (GameMode::GetInstance()->character.jump_strength, angle);
}

void Character::HighJump ()
{
  MSG_DEBUG("character", "HighJump");
  do_nothing_time = Time::GetInstance()->Read();

  if (!CanJump()) return;

  m_rebounding = false;

  jukebox.Play (ActiveTeam().GetSoundProfile(), "superjump");
   
  if(current_skin=="walking" || current_skin=="breathe")
    SetSkin("jump");

  // Initialise la force
  double angle = Deg2Rad(GameMode::GetInstance()->character.super_jump_angle);
  if (GetDirection() == -1) angle = InverseAngle(angle);
  SetSpeed (GameMode::GetInstance()->character.super_jump_strength, angle);
}

void Character::DoShoot()
{
  ActiveTeam().AccessWeapon().StopLoading();
  ActiveTeam().GetWeapon().NewActionShoot();
}

void Character::HandleShoot(int event_type)
{
  switch (event_type) {
    case KEY_PRESSED:
      if (ActiveTeam().GetWeapon().max_strength == 0)
	DoShoot();
      else
	if ( (GameLoop::GetInstance()->ReadState() == GameLoop::PLAYING)
	     && ActiveTeam().GetWeapon().IsReady() )
	  ActiveTeam().AccessWeapon().InitLoading();
      break ;

    case KEY_RELEASED:
      if (ActiveTeam().GetWeapon().IsLoading())
	DoShoot();
      break ;

    case KEY_REFRESH:
      if ( ActiveTeam().GetWeapon().IsLoading() )
	{
	  // Strength == max strength -> Fire !!!
	  if (ActiveTeam().GetWeapon().ReadStrength() >=
	      ActiveTeam().GetWeapon().max_strength)
	    DoShoot();
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
      CurseurVer::GetInstance()->Cache();
      return;
    }

  ActionHandler * action_handler = ActionHandler::GetInstance();

  if (!ActiveCharacter().IsReady())
    return;

  if(action <= ACTION_CHANGE_CHARACTER)
    {
      switch (event_type)
      {
        case KEY_PRESSED:
          switch (action)
          {
            case ACTION_JUMP:
              action_handler->NewAction (Action(ACTION_JUMP));
	            return ;
            case ACTION_HIGH_JUMP:
              action_handler->NewAction (Action(ACTION_HIGH_JUMP));
              return ;
            case ACTION_MOVE_LEFT:
            case ACTION_MOVE_RIGHT:
              is_walking = true;
              break;
            default:
      	      break;
          }
          //no break!! -> it's normal

        case KEY_REFRESH:
          switch (action) {
            case ACTION_MOVE_LEFT:
              if(event_type==KEY_PRESSED)
                InitMouvementDG(PAUSE_BOUGE);
              else
                StartWalking();
              MoveCharacterLeft(ActiveCharacter());
                //        action_handler.NewAction (Action(ACTION_MOVE_LEFT));
              break ;

            case ACTION_MOVE_RIGHT:
              if(event_type==KEY_PRESSED)
                InitMouvementDG(PAUSE_BOUGE);
              else
                StartWalking();
      	      MoveCharacterRight(ActiveCharacter());
    	      //        action_handler.NewAction (Action(ACTION_MOVE_RIGHT));
  	      break ;

            case ACTION_UP:
    	        if (ActiveTeam().crosshair.enable)
              {
                do_nothing_time = Time::GetInstance()->Read();
                CurseurVer::GetInstance()->Cache();
	              action_handler->NewAction (Action(ACTION_UP));
              }
	      break ;

            case ACTION_DOWN:
	            if (ActiveTeam().crosshair.enable)
              {
                do_nothing_time = Time::GetInstance()->Read();
                CurseurVer::GetInstance()->Cache();
     	          action_handler->NewAction (Action(ACTION_DOWN));
              }
	      break ;
            default:
	      break ;
          }
          break;
        case KEY_RELEASED:
          switch (action) {
            case ACTION_MOVE_LEFT:
            case ACTION_MOVE_RIGHT:
               if(current_skin=="walking" && full_walk)
               {
                 image->animation.SetSpeedFactor(2.0);
                 image->animation.SetLoopMode(0);
               }
               is_walking = false;
               break;
            }
        default: break;
      }
    }
}

void Character::Refresh()
{
  if (desactive) return;

  UpdatePosition ();
  Time * global_time = Time::GetInstance();

  if( &ActiveCharacter() == this && GameLoop::GetInstance()->ReadState() == GameLoop::PLAYING)
  {
    if(do_nothing_time + do_nothing_timeout < global_time->Read())
      CurseurVer::GetInstance()->SuitVerActif();
  }

  // Refresh de l'animation
  if (GetSkin().anim.utilise)
  {
    // C'est le début d'une animation ?
    if (!anim.draw && (anim.time < global_time->Read())) 
    {
      anim.image->SetCurrentFrame(0);
      anim.image->Start();
      anim.draw = true;
    }

    // Animation active
    if (anim.draw && (!GetSkin().anim.not_while_playing || &ActiveCharacter()!=this))
    {      
      anim.image->Update();

      if ( anim.image->IsFinished() )
      {
        anim.draw = false;
        anim.time  = randomObj.GetLong (ANIM_PAUSE_MIN, ANIM_PAUSE_MAX);
        anim.time += global_time->Read();
      }
    }
  }
}

// Prepare a new turn
void Character::PrepareTurn ()
{
  HandleMostDamage();
  lost_energy = 0;
  pause_bouge_dg = Time::GetInstance()->Read();
}

const Team& Character::GetTeam() const
{
  assert (m_team != NULL);
  return *m_team;
}

Team& Character::TeamAccess()
{
  assert (m_team != NULL);
  return *m_team;
}

bool Character::MouvementDG_Autorise() const
{
  if (!IsReady() || IsFalling()) return false;
  return pause_bouge_dg < Time::GetInstance()->Read();
}

bool Character::CanJump() const
{
	return MouvementDG_Autorise();
}

void Character::InitMouvementDG(uint pause)
{
  do_nothing_time = Time::GetInstance()->Read();
  CurseurVer::GetInstance()->Cache();
  m_rebounding = false;
  pause_bouge_dg = Time::GetInstance()->Read()+pause;

  StartWalking();
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
void Character::SignalFallEnding()
{
  // Do not manage dead worms.
  if (IsDead()) return;

  pause_bouge_dg = Time::GetInstance()->Read();

  double norme, degat;
  Point2d speed_vector;
  GameMode * game_mode = GameMode::GetInstance();

  StopWalking();
  GetSpeedXY (speed_vector);
  norme = speed_vector.Norm();
  if (norme > game_mode->safe_fall && speed_vector.y>0.0)
  {
    norme -= game_mode->safe_fall;
    degat = norme * game_mode->damage_per_fall_unit;
    SetEnergyDelta (-(int)degat);
    SetSkin("hard_fall_ending");
    GameLoop::GetInstance()->SignalCharacterDamageFalling(this);
  }
  if((current_skin=="jump" || current_skin=="fall"))
  {
    if(!SetSkin("weapon-" + m_team->GetWeapon().GetID()))
      SetSkin("walking");
  }
}

int Character::GetDirection() const 
{ 
  float x,y;
  image->GetScaleFactors(x,y);
  return (x<0)?-1:1;
}

bool Character::IsActive() const 
{ return !desactive; }

// End of turn or change of character
void Character::StopPlaying()
{
  SetSkin("walking");
}

// Begining of turn or changed to this character
void Character::StartPlaying()
{
  assert (!IsGhost());
  desactive = false;
  SetSkin("weapon-" + m_team->GetWeapon().GetID());
}

// Accès à l'avatar
const Skin& Character::GetSkin() const
{
  assert (skin != NULL);
  return *skin;
}

Skin& Character::AccessSkin()
{
  assert (skin != NULL);
  return *skin;
}

// Choose which skin to display (ie. dead skin, swiming skin...)
bool Character::SetSkin(const std::string& skin_name)
{
  //Return true if the this character have this skin. (if it's set in the xml file)

  if(skin_name == current_skin) return true;

  assert (skin != NULL);

#ifdef DEBUG_SKIN
  std::cout << "Setting skin \"" << skin_name << "\" for " << m_name << std::endl;
#endif

  if(AccessSkin().many_skins.find(skin_name) !=
     AccessSkin().many_skins.end())
  {
    float sc_x,sc_y;
    if(current_skin!="")
      image->GetScaleFactors(sc_x,sc_y);
    
    image = new Sprite( *(AccessSkin().many_skins[skin_name].image));

    SetTestRect (AccessSkin().many_skins[skin_name].test_dx, 
                 AccessSkin().many_skins[skin_name].test_dx, 
                 AccessSkin().many_skins[skin_name].test_top, 
                 AccessSkin().many_skins[skin_name].test_bottom);

    SetSize(image->GetSize());
    image->SetCurrentFrame(0);
    image->Start();
    PutOutOfGround();     
    //Restore skins direction
    if(current_skin!="" && sc_x<0)
       image->Scale(-1,1);

    current_skin = skin_name;
    skin_is_walking = false;
    return true;
  }
  else
  if(AccessSkin().many_walking_skins.find(skin_name) != 
     AccessSkin().many_walking_skins.end())
  {
    float sc_x,sc_y;
    if(current_skin!="")
       image->GetScaleFactors(sc_x,sc_y);

    walk_skin = &AccessSkin().many_walking_skins[skin_name];
    full_walk = walk_skin->full_walk;

    image = new Sprite(*(walk_skin->image));

    SetTestRect (walk_skin->test_dx, 
                 walk_skin->test_dx,
                 walk_skin->test_top,
                 walk_skin->test_bottom);
    m_frame_repetition = walk_skin->repetition_frame;

    SetSize(image->GetSize());
    PutOutOfGround();     
    StopWalking();

    //Restore skins direction
    if(current_skin!="" && sc_x<0.0)
      image->Scale( -1.0,1.0);

     
    if(skin_name=="walking" || skin_name=="breathe")
    {
        anim.draw = false;
        anim.time  = randomObj.GetLong (ANIM_PAUSE_MIN, ANIM_PAUSE_MAX);
        anim.time += Time::GetInstance()->Read();
    }

    current_skin = skin_name;
    skin_is_walking = true;
    return true;
  }
  else
  {
//    std::cout << "Unable to set skin : " << skin_name << "\n";
    assert(skin_name!="walking");
//    SetSkin("walking");
//    image->Finish();
#ifdef DEBUG_SKIN
  std::cout << "Skin \"" << skin_name << "\" not found!"<< std::endl;
#endif

    return false;
  }
}

void Character::FrameImageSuivante()
{
  if(full_walk) return;

  m_image_frame++;

  if (image->GetFrameCount()-1 < (m_image_frame/m_frame_repetition)) 
    m_image_frame = 0;

  image->SetCurrentFrame (m_image_frame/m_frame_repetition); 

  if ( channel_step == -1 || !Mix_Playing(channel_step) ) {
    channel_step = jukebox.Play (m_team->GetSoundProfile(), "step");
  }
}

void Character::InitTeam (Team *ptr_equipe, const std::string &name, 
			  Skin* pskin)
{
  GameMode * game_mode = GameMode::GetInstance();
  SetMass (game_mode->character.mass);
  SetAirResistFactor (game_mode->character.air_resist_factor);
  m_team = ptr_equipe;
  m_name = name;
  skin = pskin;

  // Animation ?
  anim.draw = false;
  if (GetSkin().anim.utilise)
  { 
    anim.draw = true;
    anim.image = new Sprite(*GetSkin().anim.image);
  }

  // Energie
  energy_bar.InitVal (energy, 0, game_mode->character.max_energy);
  energy_bar.InitPos (0,0, LARG_ENERGIE, HAUT_ENERGIE);

  energy_bar.SetBorderColor( black_color );
  energy_bar.SetBackgroundColor( gray_color );
}

void Character::Reset() 
{
  // Reset de l'état du ver
  desactive = false;

  Ready();

  //  Reset de l'image et les dimensions
  SetSkin("walking");

  // Animation ?
  if (anim.draw)
  { 
    anim.image->Finish();
    anim.time  = randomObj.GetLong (ANIM_PAUSE_MIN, ANIM_PAUSE_MAX);
    anim.time += Time::GetInstance()->Read();    
  }

  // Initialise l'image
  SetDirection( randomObj.GetBool()?1:-1 );

  if(!full_walk)
    image->SetCurrentFrame ( randomObj.GetLong(0, image->GetFrameCount()-1) );
  else
    StartBreathing();

  m_image_frame = image->GetCurrentFrame();   


  // Prépare l'image du nom
  if (Config::GetInstance()->GetDisplayNameCharacter() && name_text == NULL)
  {
    name_text = new Text(m_name);
  }

  // Energie
  energy = GameMode::GetInstance()->character.init_energy-1;
  energy_bar.InitVal (energy, 0, GameMode::GetInstance()->character.init_energy);
  SetEnergyDelta (1);
  lost_energy = 0;

  // Initialise la position
  uint bcl=0;
  bool pos_ok;
  do
  {
    // Vérifie qu'on ne tourne pas en rond
    FORCE_ASSERT (++bcl < Constants::NBR_BCL_MAX_EST_VIDE);

    // Objet physique dans l'état prêt
    pos_ok = true;
    desactive = false;
    Ready();

    SetXY( randomObj.GetPoint(world.GetSize() - GetSize() + 1) );

#ifndef NO_POSITION_CHECK
    pos_ok &= !IsGhost() && IsInVacuum( Point2i(0,0) ) && (GetY() < static_cast<int>(world.GetHeight() - (WATER_INITIAL_HEIGHT + 30)));
    if (!pos_ok) continue;

    // Chute directe pour le sol
    DirectFall ();
    pos_ok &= !IsGhost() && (GetY() < static_cast<int>(world.GetHeight() - (WATER_INITIAL_HEIGHT + 30)));
#ifdef DEBUG_PLACEMENT
    if (!pos_ok) COUT_PLACEMENT << "Fantome en tombant." << std::endl;
#endif
    if (!pos_ok) continue;

    // Vérifie que le ver ne fois pas trop près de ses voisins
    FOR_ALL_LIVING_CHARACTERS(it_equipe,ver) if (&(*ver) != this)
    {
       Point2i p1 = ver->GetCenter();
       Point2i p2 = GetCenter();
       double dst = p1.Distance( p2 );

      if (dst < world.dst_min_entre_vers) {
	pos_ok = false;
      }
       
    }

    // La position est bonne ?
    pos_ok &= !IsGhost() & !IsInWater() & IsInVacuum( Point2i(0,0) );
#ifdef DEBUG_PLACEMENT
    if (!pos_ok) COUT_PLACEMENT << "Placement final manqué." << std::endl;
#endif

#endif // of #ifndef NO_POSITION_CHECK
  } while (!pos_ok);
  assert (!IsDead());
  Ready();
}

uint Character::GetEnergy() const 
{
  assert (!IsDead());
  return energy; 
}

// Hand position
Point2i Character::GetHandPosition() {
   int frame = image->GetCurrentFrame();
   Point2i result;
   
   assert(walk_skin!=NULL);

   if(current_skin=="breathe")
   {
		//Hand position is first frame of the hand position of the walking skin
		skin_translate_t hand = walk_skin->hand_position.at(0);
		result.y = GetY() + hand.dy;
		if (GetDirection() == 1)
			result.x = GetX() + hand.dx;
		else
      		result.x = GetX() + GetWidth() - hand.dx;
		return result;
  }

  skin_translate_t hand = walk_skin->hand_position.at(frame);
  result.y = GetY() + hand.dy;
  if (GetDirection() == 1)
    result.x = GetX() + hand.dx;
  else
    result.x = GetX() + GetWidth() - hand.dx;

  return result;
}

// Hand position
void Character::GetHandPositionf (double &x, double &y) 
{

   int frame = image->GetCurrentFrame();

  assert(walk_skin!=NULL);
  skin_translate_t hand = walk_skin->hand_position.at(frame);

  y = (double)GetY() +hand.dy;
  if (GetDirection() == 1) 
    x = (double)GetX() +hand.dx;
  else
    x = (double)GetX() +GetWidth() -hand.dx;
}

void Character::HandleMostDamage()
{
  if (current_total_damage > max_damage)
  {
    max_damage = current_total_damage;
  }
#ifdef DEBUG
  std::cerr << m_name << " most damage: " << max_damage << std::endl;
#endif
  current_total_damage = 0;
}

void Character::Hide() { hidden = true; }
void Character::Show() { hidden = false; }

void Character::MadeDamage(const int Dmg, const Character &other)
{
  if (m_team->IsSameAs(other.GetTeam()))
  {
#ifdef DEBUG
    std::cerr << m_name << " damaged own team with " << Dmg << std::endl;
#endif
    if (Character::IsSameAs(other))
      damage_own_team += Dmg;
  }
  else
  {
#ifdef DEBUG
    std::cerr << m_name << " damaged other team with " << Dmg << std::endl;
#endif
    damage_other_team += Dmg;
  }
  
  current_total_damage += Dmg;
}
