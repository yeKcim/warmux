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

#include <SDL.h>

#include "character.h"
//-----------------------------------------------------------------------------
#include <SDL.h>
#include <sstream>
#include <iostream>
#include "move.h"
#include "macro.h"
#include "../game/game.h"
#include "../game/game_mode.h"
#include "../game/game_loop.h"
#include "../game/time.h"
#include "../game/config.h"
#include "../graphic/text.h"
#include "../include/action_handler.h"
#include "../include/app.h"
#include "../include/constant.h"
#include "../include/global.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../map/water.h"
#include "../sound/jukebox.h"
#include "../tool/random.h"
#include "../tool/math_tools.h"
#include "../weapon/suicide.h"
#include "../weapon/crosshair.h"
#include "../weapon/weapon_tools.h"
#include "../interface/cursor.h"

using namespace std;
using namespace Wormux;
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

Character::Character () : PhysicalObj("Soldat inconnu", 0.0)
{
  pause_bouge_dg = 0;
  previous_strength = 0;
  m_team = NULL;
  energy = 100;
  losted_energy = 0;
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

  name_text = NULL;
}

//-----------------------------------------------------------------------------

// Signale la mort d'un ver
void Character::SignalDeath()
{
#ifdef DEBUG_CHG_ETAT
  COUT_DBG << "Meurt." << endl;
#endif

  // No more energy ...
  energy = 0;

  jukebox.Play(GetTeam().GetSoundProfile(),"death");

  SetSkin("dead");

  //--- Do the skin explosion :-) ---
  SDL_Surface *img_trou = suicide.hole_image;

   
  // The hole is at the center
  Point2i trou(GetCenter());
   
  // But the explosion is on the foots :)
  Point2i explosion(GetCenterX(), GetY()+ GetHeight());
   
  AppliqueExplosion (explosion, trou, img_trou, suicide.cfg(), NULL);

  // Change test rectangle
  int x = GetCenterX(), y=GetCenterY();
  SetSize (image->GetWidth(), image->GetHeight());
  SetXY (x - GetWidth()/2, y - GetHeight()/2);

  assert (m_alive == DEAD);
  assert (IsDead());
  
  // Signal the death
  game_loop.SignalCharacterDeath (this);
}

//-----------------------------------------------------------------------------

void Character::SignalDrowning()
{
  energy = 0;
  //m_type = objINSENSIBLE;
  SetSkin("drowned");

  jukebox.Play (GetTeam().GetSoundProfile(),"sink");
  game_loop.SignalCharacterDeath (this);
}

//-----------------------------------------------------------------------------

// Si un ver devient un fantome, il meurt ! Signale sa mort
void Character::SignalGhostState (bool was_dead)
{
  // Désactive le ver
  desactive = true;

#ifdef DEBUG_CHG_ETAT
  COUT_DBG << "Fantome." << endl;
#endif

  // Signal the death
  if (!was_dead) game_loop.SignalCharacterDeath (this);
}

//-----------------------------------------------------------------------------

void Character::SetDirection (int nv_direction)
{ 
   image->Scale (nv_direction, 1);
   
   if (GetSkin().anim.utilise) 
     anim.image->Scale (nv_direction, 1);
}

//-----------------------------------------------------------------------------

void Character::DrawEnergyBar(int dy) const
{
  if(IsDead()) return;
  energy_bar.DrawXY ( GetCenterX()-energy_bar.GetWidth()/2-camera.GetX(), 
		      GetY()+dy-camera.GetY());
}

//-----------------------------------------------------------------------------

void Character::DrawName (int dy) const
{
  if(IsDead()) return;

  const int x =  GetCenterX();
  const int y = GetY()+dy;

  if (config.display_name_character)
  {
    name_text->DrawCenterTopOnMap(x,y);
  }
}

//-----------------------------------------------------------------------------

void Character::SetEnergyDelta (int delta)
{
  // If already dead, do nothing
  if (IsDead()) return;

  uint sauve_energie = energy;
  uchar R,V,B;

  // Change energy
  energy = BorneLong((int)energy +delta, 0, game_mode.character.max_energy);
  energy_bar.Actu (energy);
    
  // Energy bar color
  if (70 < energy) {
    V = 255;
    R = 0;
  } else if (50 < energy) {
    V = 255;
    R = 255;
  } else if (20 < energy) {
    V = 128;
    R = 255;
  } else {
    V = 0;
    R = 255;
  }
  B = 0;

  energy_bar.SetValueColor( R,V,B);

   
  // Compute energy lost
  if (delta < 0)
  {

    losted_energy += (int)energy - (int)sauve_energie;

    if ( losted_energy < 33 )
      jukebox.Play (GetTeam().GetSoundProfile(), "injured_light");
    else if ( losted_energy < 66 )
      jukebox.Play (GetTeam().GetSoundProfile(), "injured_medium");
    else 
      jukebox.Play (GetTeam().GetSoundProfile(), "injured_high");
    
  }
  else 
    losted_energy = 0;

  // "Friendly fire !!"
  if ( (&ActiveCharacter() != this) && (&ActiveTeam() == m_team) )
  jukebox.Play (GetTeam().GetSoundProfile(), "friendly_fire");
   
  // Dead character ?
  if (energy == 0) Die();
}

//-----------------------------------------------------------------------------
void Character::StartBreathing()
{
  SetSkin("breathe");
  if(current_skin == "breathe")
  {
    image->SetSpeedFactor((float)RandomLong(100,150)/100.0);
    image->SetLoopMode();
    image->Start();
  }
}

//-----------------------------------------------------------------------------
void Character::StartWalking()
{
  do_nothing_time = Wormux::global_time.Read();

  if(full_walk && current_skin=="breathe")
  {
    SetSkin("walking");
    image->Start();
    image->SetLoopMode();
    image->SetSpeedFactor(1.0);
    image->SetCurrentFrame(0);
  }
}

//-----------------------------------------------------------------------------
void Character::StopWalking()
{
  if(image!=NULL && current_skin=="walking" && full_walk)
  {
    image->Finish();
    image->SetCurrentFrame(0);
  }
}

//-----------------------------------------------------------------------------
void Character::Draw()
{
  if (hidden) return;

  // Gone in another world ?
  if (!IsActive()) return;

  bool dessine_perte = (losted_energy != 0);
  if ((&ActiveCharacter() == this
    && game_loop.ReadState() != gameEND_TURN)
      //&& (game_loop.ReadState() != jeuANIM_FIN_TOUR)
    || IsDead()
     )
    dessine_perte = false;

  // Draw skin
  if(full_walk && !image->IsFinished() && game_loop.ReadState() == gameEND_TURN)
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

  int x = GetX();
  int y = GetY();
  image->Draw(x,y);
   
  // Draw animation
  if(anim.draw
  && (current_skin=="walking" || current_skin=="breathe")
  && (!GetSkin().anim.not_while_playing || &ActiveCharacter()!=this))
  {
    int dx = 0;
    if(GetDirection()==-1)
      dx = image->GetWidth() - anim.image->GetWidth();
    anim.image->Draw(x+dx,y);
  }

   // Draw energy bar
  int dy = -ESPACE;
  bool est_ver_actif = (this == &ActiveCharacter());
  bool display_energy = config.display_energy_character;
  display_energy &= !est_ver_actif || (game_loop.ReadState() != gamePLAYING);
  display_energy |= dessine_perte;
  display_energy &= !IsDead();
  if (display_energy)
  { 
    dy -= HAUT_ENERGIE; 
    DrawEnergyBar (dy); 
    dy -= ESPACE; 
  }

  // Draw name
  if (config.display_name_character && !est_ver_actif) 
  { 
    dy -= HAUT_FONT_MIX;
    DrawName (dy);
    dy -= ESPACE; 
  }

  // Draw lost energy
  if (dessine_perte)
  {
    ostringstream ss;
    ss << losted_energy;
    dy -= HAUT_FONT_MIX;
    global().small_font().WriteCenterTop (GetX() +GetWidth()/2-camera.GetX(), GetY()+dy-camera.GetY(), ss.str(), white_color);    
  }

}

//-----------------------------------------------------------------------------

void Character::Saute ()
{
#ifdef DEBUG_CHG_ETAT
  COUT_DBG << "Saute." << endl;
#endif
  do_nothing_time = Wormux::global_time.Read();

  if (!CanJump()) return;

  jukebox.Play (ActiveTeam().GetSoundProfile(), "jump");
   
  m_rebounding = false;

  if(current_skin=="walking" || current_skin=="breathe")
    SetSkin("jump");

  // Initialise la force
  double angle = Deg2Rad(game_mode.character.jump_angle);
  if (GetDirection() == -1) angle = InverseAngle(angle);
  SetSpeed (game_mode.character.jump_strength, angle);
}

//-----------------------------------------------------------------------------

void Character::SuperSaut ()
{
#ifdef DEBUG_CHG_ETAT
  COUT_DBG << "SuperSaut." << endl;
#endif
  do_nothing_time = Wormux::global_time.Read();

  if (!CanJump()) return;

  m_rebounding = false;

  jukebox.Play (ActiveTeam().GetSoundProfile(), "superjump");
   
  if(current_skin=="walking" || current_skin=="breathe")
    SetSkin("jump");

  // Initialise la force
  double angle = Deg2Rad(game_mode.character.super_jump_angle);
  if (GetDirection() == -1) angle = InverseAngle(angle);
  SetSpeed (game_mode.character.super_jump_strength, angle);
}

//-----------------------------------------------------------------------------

void Character::DoShoot()
{
  ActiveTeam().AccessWeapon().StopLoading();
  ActiveTeam().GetWeapon().NewActionShoot();
}

//-----------------------------------------------------------------------------

void Character::HandleShoot(int event_type)
{
  switch (event_type) {
    case KEY_PRESSED:
      if (ActiveTeam().GetWeapon().max_strength == 0)
	DoShoot();
      else
	if ( (game_loop.ReadState() == gamePLAYING)
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

//-----------------------------------------------------------------------------

void Character::HandleKeyEvent(int action, int event_type)
{
  // The character cannot move anymove if the turn is over...
  if (game_loop.ReadState() == gameEND_TURN)
    return ;

  if (ActiveCharacter().IsDead())
    return;

  if (action == ACTION_SHOOT)
    {
      HandleShoot(event_type);
      do_nothing_time = Wormux::global_time.Read();
      curseur_ver.Cache();
      return;
    }

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
              action_handler.NewAction (Action(ACTION_JUMP));
	            return ;
            case ACTION_SUPER_JUMP:
              action_handler.NewAction (Action(ACTION_SUPER_JUMP));
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
                do_nothing_time = Wormux::global_time.Read();
                curseur_ver.Cache();
	              action_handler.NewAction (Action(ACTION_UP));
              }
	      break ;

            case ACTION_DOWN:
	            if (ActiveTeam().crosshair.enable)
              {
                do_nothing_time = Wormux::global_time.Read();
                curseur_ver.Cache();
     	          action_handler.NewAction (Action(ACTION_DOWN));
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
                 image->SetSpeedFactor(2.0);
                 image->SetLoopMode(0);
               }
               is_walking = false;
               break;
            }
        default: break;
      }
    }
}

//-----------------------------------------------------------------------------

void Character::Refresh()
{
  if (desactive) return;

  UpdatePosition ();

  if( &ActiveCharacter() == this && game_loop.ReadState() == gamePLAYING)
  {
    if(do_nothing_time + do_nothing_timeout < Wormux::global_time.Read())
      curseur_ver.SuitVerActif();
  }

  // Refresh de l'animation
  if (GetSkin().anim.utilise)
  {
    // C'est le début d'une animation ?
    if (!anim.draw && (anim.time < global_time.Read())) 
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
        anim.time  = RandomLong (ANIM_PAUSE_MIN, ANIM_PAUSE_MAX);
        anim.time += global_time.Read();
      }
    }
  }
}

//-----------------------------------------------------------------------------

// Prepare a new turn
void Character::PrepareTour ()
{
  losted_energy = 0;
  pause_bouge_dg = global_time.Read();
}

//-----------------------------------------------------------------------------

const Team& Character::GetTeam() const
{
  assert (m_team != NULL);
  return *m_team;
}

//-----------------------------------------------------------------------------

Team& Character::TeamAccess()
{
  assert (m_team != NULL);
  return *m_team;
}

//-----------------------------------------------------------------------------

bool Character::MouvementDG_Autorise() const
{
  if (!IsReady() || IsFalling()) return false;
  return pause_bouge_dg < global_time.Read();
}

//-----------------------------------------------------------------------------

bool Character::CanJump() const
{
	return MouvementDG_Autorise();
}

//-----------------------------------------------------------------------------

void Character::InitMouvementDG(uint pause)
{
  do_nothing_time = Wormux::global_time.Read();
  curseur_ver.Cache();
  m_rebounding = false;
  pause_bouge_dg = global_time.Read()+pause;

  StartWalking();
}

//-----------------------------------------------------------------------------

bool Character::CanStillMoveDG(uint pause)
{
  if(pause_bouge_dg+pause<global_time.Read())
  {
    pause_bouge_dg += pause;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------

// Signal the end of a fall
void Character::SignalFallEnding()
{
  // Do not manage dead worms.
  if (IsDead()) return;

  pause_bouge_dg = global_time.Read();

  double norme, degat;
  DoubleVector speed_vector ;

  StopWalking();
  GetSpeedXY (speed_vector);
  norme = Norm (speed_vector);
  if (norme > game_mode.safe_fall && speed_vector.y>0.0)
  {
    norme -= game_mode.safe_fall;
    degat = norme * game_mode.damage_per_fall_unit;
    SetEnergyDelta (-(int)degat);
    SetSkin("hard_fall_ending");
    game_loop.SignalCharacterDamageFalling(this);
  }
  if((current_skin=="jump" || current_skin=="fall"))
  {
    if(!SetSkin("weapon-" + m_team->GetWeapon().GetID()))
      SetSkin("walking");
  }
}

//-----------------------------------------------------------------------------

int Character::GetDirection() const 
{ 
  float x,y;
  image->GetScaleFactors(x,y);
  return (x<0)?-1:1;
}

//-----------------------------------------------------------------------------

bool Character::IsActive() const 
{ return !desactive; }

//-----------------------------------------------------------------------------

// End of turn or change of character
void Character::StopPlaying()
{
  SetSkin("walking");
}

//-----------------------------------------------------------------------------

// Begining of turn or changed to this character
void Character::StartPlaying()
{
  assert (!IsGhost());
  desactive = false;
  SetSkin("weapon-" + m_team->GetWeapon().GetID());
}

//-----------------------------------------------------------------------------

// Accès à l'avatar
const Skin& Character::GetSkin() const
{
  assert (skin != NULL);
  return *skin;
}

//-----------------------------------------------------------------------------

Skin& Character::AccessSkin()
{
  assert (skin != NULL);
  return *skin;
}

//-----------------------------------------------------------------------------
// Choose which skin to display (ie. dead skin, swiming skin...)
bool Character::SetSkin(std::string skin_name)
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

    SetSize (image->GetWidth(), image->GetHeight());
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

    SetSize (image->GetWidth(), image->GetHeight());
    PutOutOfGround();     
    StopWalking();

    //Restore skins direction
    if(current_skin!="" && sc_x<0.0)
      image->Scale( -1.0,1.0);

     
    if(skin_name=="walking" || skin_name=="breathe")
    {
        anim.draw = false;
        anim.time  = RandomLong (ANIM_PAUSE_MIN, ANIM_PAUSE_MAX);
        anim.time += global_time.Read();
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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void Character::Init() {}

//-----------------------------------------------------------------------------

void Character::InitTeam (Team *ptr_equipe, const string &name, 
			  Skin* pskin)
{
  SetMass (game_mode.character.mass);
  SetAirResistFactor (game_mode.character.air_resist_factor);
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
  energy_bar.InitVal (energy, 0, game_mode.character.max_energy);
  energy_bar.InitPos (0,0, LARG_ENERGIE, HAUT_ENERGIE);

  energy_bar.SetBorderColor(0,0,0);
  energy_bar.SetBackgroundColor(100,100,100);

}

//-----------------------------------------------------------------------------

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
    anim.time  = RandomLong (ANIM_PAUSE_MIN, ANIM_PAUSE_MAX);
    anim.time += global_time.Read();    
  }

  // Initialise l'image
  SetDirection( RandomBool()?1:-1 );

  if(!full_walk)
    image->SetCurrentFrame ( RandomLong(0, image->GetFrameCount()-1) );
  else
    StartBreathing();

  m_image_frame = image->GetCurrentFrame();   


  // Prépare l'image du nom
  if (config.display_name_character && name_text == NULL)
  {
    name_text = new Text(m_name);
  }

  // Energie
  energy = game_mode.character.init_energy-1;
  energy_bar.InitVal (energy, 0, game_mode.character.init_energy);
  SetEnergyDelta (1);
  losted_energy = 0;

  // Initialise la position
  uint bcl=0;
  bool pos_ok;
  do
  {
    // Vérifie qu'on ne tourne pas en rond
    FORCE_ASSERT (++bcl < NBR_BCL_MAX_EST_VIDE);

    // Objet physique dans l'état prêt
    pos_ok = true;
    desactive = false;
    Ready();

    // Prend des coordonnées au hasard
    uint x = RandomLong(0, world.GetWidth() -GetWidth());
    uint y = RandomLong(0, world.GetHeight() -GetHeight());

    SetXY (x, y);

#ifndef NO_POSITION_CHECK
    pos_ok &= !IsGhost() && IsInVacuum(0,0) && (GetY() < static_cast<int>(world.GetHeight() - (WATER_INITIAL_HEIGHT + 30)));
    if (!pos_ok) continue;

    // Chute directe pour le sol
    DirectFall ();
    pos_ok &= !IsGhost() && (GetY() < static_cast<int>(world.GetHeight() - (WATER_INITIAL_HEIGHT + 30)));
#ifdef DEBUG_PLACEMENT
    if (!pos_ok) COUT_PLACEMENT << "Fantome en tombant." << endl;
#endif
    if (!pos_ok) continue;

    // Vérifie que le ver ne fois pas trop près de ses voisins
    POUR_TOUS_VERS_VIVANTS(it_equipe,ver) if (&(*ver) != this)
    {
       Point2i p1 = ver->GetCenter();
       Point2i p2 = GetCenter();
       double dst = p1.Distance( p2 );

      if (dst < world.dst_min_entre_vers) {
	pos_ok = false;
      }
       
    }

    // La position est bonne ?
    pos_ok &= !IsGhost() & IsInVacuum(0,0);
#ifdef DEBUG_PLACEMENT
    if (!pos_ok) COUT_PLACEMENT << "Placement final manqué." << endl;
#endif

#endif // of #ifndef NO_POSITION_CHECK
  } while (!pos_ok);
  assert (!IsDead());
  Ready();
}

//-----------------------------------------------------------------------------

uint Character::GetEnergy() const 
{
  assert (!IsDead());
  return energy; 
}

//-----------------------------------------------------------------------------

// Hand position
void Character::GetHandPosition (int &x, int &y) 
{
   int frame = image->GetCurrentFrame();
   
  assert(walk_skin!=NULL);

  if(current_skin=="breathe")
  {
    //Hand position is first frame of the hand position of the walking skin
    skin_translate_t hand = walk_skin->hand_position.at(0);
    y = GetY() +hand.dy;
    if (GetDirection() == 1)
      x = GetX() +hand.dx;
    else
      x = GetX() +GetWidth() -hand.dx;
    return;
  }

  skin_translate_t hand = walk_skin->hand_position.at(frame);
  y = GetY() +hand.dy;
  if (GetDirection() == 1)
    x = GetX() +hand.dx;
  else
    x = GetX() +GetWidth() -hand.dx;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
void Character::EndTurn()
{
  m_rebounding = true;
  is_walking = false;
}

//-----------------------------------------------------------------------------
void Character::Hide() { hidden = true; }
void Character::Show() { hidden = false; }
//-----------------------------------------------------------------------------
