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
 * Boucle de jeu : dessin et gestion des données.
 *****************************************************************************/

#include "game_loop.h"
//-----------------------------------------------------------------------------
#include <SDL.h>
#include <sstream>
#include <iostream>
#include "config.h"
#include "game.h"
#include "game_mode.h"
#include "time.h"
#include "../graphic/fps.h"
#include "../graphic/video.h"
#include "../include/action_handler.h"
#include "../include/app.h"
#include "../include/constant.h"
#include "../interface/cursor.h"
#include "../interface/game_msg.h"
#include "../interface/interface.h"
#include "../interface/keyboard.h"
#include "../interface/mouse.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../map/maps_list.h"
#include "../map/wind.h"
#include "../object/bonus_box.h"
#include "../object/objects_list.h"
#include "../object/particle.h"
#include "../sound/jukebox.h"
#include "../team/macro.h"
#include "../tool/debug.h"
#include "../tool/i18n.h"
#include "../tool/stats.h"
#include "../weapon/weapons_list.h"

using namespace Wormux;

#define ENABLE_LIMIT_FPS    

GameLoop game_loop;
bool game_initialise = false;
bool game_fin_partie;

#ifdef TODO_NETWORK 
void InitGameData_NetServer()
{
  //        action_handler.NewAction(Action(ACTION_ASK_TEAM));
  do
    {
      action_handler.NewAction(Action(ACTION_ASK_VERSION));
      std::string msg=_("Wait for clients");
      action_handler.ExecActions();
      std::cout << msg << std::endl;
      CL_Display::clear(CL_Color::black);
      police_grand.WriteCenterTop (video.GetWidth()/2, video.GetHeight()/2, msg);
      CL_Display::flip();
      CL_System::keep_alive(500);
    } while (network.state != Network::NETWORK_SERVER_INIT_GAME);
  std::cout << "Server init game." << std::endl;
        
  std::cout << "o " << _("Load map") << std::endl;
  action_handler.NewAction (ActionString(ACTION_SET_MAP, TerrainActif().name));
  world.Reset();

  std::cout << "o " << _("Initialise teams") << std::endl;
  teams_list.Reset();

  // For cliens : Create teams
  action_handler.NewAction (Action(ACTION_CLEAR_TEAMS));
  
  TeamsList::iterator 
    it=teams_list.playing_list.begin(),
    end=teams_list.playing_list.end();

  for (; it != end; ++it)
    {
      Team& team = **it;
                
      // cliens : Create teams
      action_handler.NewAction (ActionString(ACTION_NEW_TEAM, team.GetId()));
                
      // cliens : Place characters
      action_handler.NewAction (ActionString(ACTION_CHANGE_TEAM, team.GetId()));
      Team::iterator
        tit = team.begin(),
        tend = team.end();
      int i=0;
      for (; tit != tend; ++tit, ++i)
        {
          Character &character = *tit;
          action_handler.NewAction (ActionInt(
                                              ACTION_CHANGE_CHARACTER, i));
          action_handler.NewAction (ActionInt2(
                                               ACTION_MOVE_CHARACTER, 
                                               character.GetX(), character.GetY()));
          action_handler.NewAction (ActionInt(
                                              ACTION_SET_CHARACTER_DIRECTION, 
                                              character.GetDirection()));
        }

      // Select first character
      action_handler.NewAction (ActionInt(ACTION_CHANGE_CHARACTER, 0));
    }
        
  action_handler.NewAction (ActionString(ACTION_CHANGE_TEAM, ActiveTeam().GetId()));
  action_handler.NewAction (ActionInt(ACTION_CHANGE_CHARACTER, ActiveTeam().ActiveCharacterIndex()));

  // Create objects
  lst_objets.Reset();
   // @@@ TODO : send objects ... @@@@
        
  // Remise à zéro
  std::cout << "o " << _("Initialise data") << std::endl;
  curseur_ver.Reset();
  global_time.Reset();
  mouse.Reset();
  image_par_seconde.Reset();
  interface.Reset();
  game_messages.Reset();
  
  action_handler.NewAction (Action(ACTION_START_GAME));
}

void InitGameData_NetClient()
{
  do
    {
      std::string msg=_("Wait for server informations");
      switch(network.state)
        {
        case Network::NETWORK_WAIT_SERVER:
          msg = _("Wait for server");
          break;
        case Network::NETWORK_WAIT_MAP:
          msg = _("Wait for map");
          break;
        case Network::NETWORK_WAIT_TEAMS:
          msg = _("Wait teams");
          break;
        default:
          msg = _("Unknow action");
          std::cout << "Unknow action for network in game_loop.cpp" << std::endl;
          break;
        }
      action_handler.ExecActions();
      std::cout << network.state << std::endl;
      std::cout << msg << std::endl;
      CL_Display::clear(CL_Color::black);
      police_grand.WriteCenterTop (video.GetWidth()/2, video.GetHeight()/2, msg);
      CL_Display::flip();
      CL_System::keep_alive(300);
    } while (network.state != Network::NETWORK_PLAYING);
  std::cout << network.state << " : Run game !" << std::endl;
 
  // @@@ TODO @@@
  lst_objets.Reset();
 
  //Set the second team as a team played from the client
  ActiveTeam().is_local = false;
}

#endif // TODO_NETWORK

void InitGameData_Local()
{
  // Placement des vers
  std::cout << "o " << _("Find a random position for characters") << std::endl;
  world.Reset();
  lst_terrain.TerrainActif().FreeData();
  teams_list.Reset();

  // Remise à zéro
  std::cout << "o " << _("Initialise objects") << std::endl;
  lst_objets.Reset();
}

void InitGameData()
{
  global_time.Reset();
  
#ifdef TODO_NETWORK 
  if (network.is_server())
    InitGameData_NetServer();
  else if (network.is_client())
    InitGameData_NetClient();
  else        
#endif
  InitGameData_Local();

  curseur_ver.Reset();
  mouse.Reset();
  clavier.Reset();
   
  image_par_seconde.Reset();
  interface.Reset();
  game_messages.Reset();
  caisse.Init();
}

void InitGame ()
{
  game.MessageLoading();

  // Init all needed data
  if (!game_initialise)
  {
    std::cout << "o " << _("Initialisation") << std::endl;
    interface.Init();
    curseur_ver.Init();
    lst_objets.Init();
    game_initialise = true;
  }

  InitGameData();

  // Init teams

  // Teams' creation
  if (teams_list.playing_list.size() < 2)
    Error(_("You need at least two teams to play: "
             "change this in 'Options menu' !"));
  assert (teams_list.playing_list.size() <= game_mode.max_teams);

  // Initialization of teams' energy
  teams_list.InitEnergy();

  // Load teams' sound profiles
  jukebox.LoadXML("default");
  FOR_EACH_TEAM(team) 
    if ( (**team).GetSoundProfile() != "default" )
      jukebox.LoadXML((**team).GetSoundProfile()) ; 
   
  // Begin to play !!
  // Music -> sound should be choosed in map.Init and then we just have to call jukebox.PlayMusic()
  if (jukebox.UseMusic()) jukebox.Play ("share", "music/grenouilles", -1);
   
  if (!config.display_wind_particles) {
    TerrainActif().wind.nb_sprite = 0;
  } else {
    TerrainActif().wind.nb_sprite = TerrainActif().wind.default_nb_sprite;
  }

  game.SetEndOfGameStatus( false );
  game_loop.SetState (gamePLAYING, true);
}

GameLoop::GameLoop()
{
  state = gamePLAYING;
  interaction_enabled = true;
}

void GameLoop::Refresh()
{  
  RefreshClock();
  game_messages.Refresh();
  camera.Refresh();

  // Mise à jeu des entrées (clavier / mouse)
   // Poll and treat events
   SDL_Event event;
   
   while( SDL_PollEvent( &event) ) 
     {      
        if ( event.type == SDL_QUIT) 
          {  
             std::cout << "SDL_QUIT received ===> exit TODO" << std::endl;
             game.SetEndOfGameStatus( true );
             std::cout << "FIN PARTIE" << std::endl;
             return;
          }
        if ( event.type == SDL_MOUSEBUTTONDOWN )
          {
             mouse.TraiteClic( &event);
          }
        if ( event.type == SDL_KEYDOWN 
        ||   event.type == SDL_KEYUP)
          {               
             if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE)
             {
                  game.SetEndOfGameStatus( true );
                  std::cout << "FIN PARTIE" << std::endl;
                  return;
             }
             
             clavier.HandleKeyEvent( &event);
          }
     }
   
  // How many frame by seconds ?
  image_par_seconde.Refresh();

  //--- D'abord ce qui pourrait modifier les données d'un ver ---

  if (!global_time.IsGamePaused())
  {
     
    // Keyboard and mouse refresh
    if ( 
        (interaction_enabled && state != gameEND_TURN)
        || (ActiveTeam().GetWeapon().IsActive() && ActiveTeam().GetWeapon().override_keys) // for driving supertux for example
        )
    {
      mouse.Refresh();
      clavier.Refresh();
    }
   
    action_handler.ExecActions();
    POUR_TOUS_VERS(equipe,ver) ver -> Refresh();

    // Recalcule l'energie des equipes
    FOR_EACH_TEAM(team)
      (**team).Refresh();
    teams_list.RefreshEnergy();

    //--- Ensuite, actualise le reste du jeu ---

    ActiveTeam().AccessWeapon().Manage();
    lst_objets.Refresh();
    global_particle_engine.Refresh();
    curseur_ver.Refresh();
  }
  
  // Refresh the map
  world.Refresh();
}

void GameLoop::Draw ()
{
  // Draw the sky 
  StatStart("GameDraw:sky");
  world.DrawSky();
  StatStop("GameDraw:sky");

  // Draw the map
  StatStart("GameDraw:world");
  world.Draw();
  StatStop("GameDraw:world");

  // Draw the characters 
  StatStart("GameDraw:characters");
  POUR_TOUS_VERS(equipe,ver) {
    if (&(*ver) != &ActiveCharacter()) {
      ver -> Draw();
    }
  }
  ActiveCharacter().Draw();
  if (!ActiveCharacter().IsDead() && state != gameEND_TURN) {
        ActiveTeam().crosshair.Draw();
        ActiveTeam().AccessWeapon().Draw();
  }
  StatStop("GameDraw:characters");

  StatStart("GameDraw:other");
  lst_objets.Draw();
  global_particle_engine.Draw();
  curseur_ver.Draw();

  world.DrawWater();

  FOR_EACH_TEAM(team)
    (**team).Draw();

  game_messages.Draw();

  world.DrawAuthorName();

  image_par_seconde.Draw();

  StatStop("GameDraw:other");

  // Draw the interface (current team's information, weapon's ammo)
  StatStart("GameDraw:interface");
  interface.Draw ();
  StatStop("GameDraw:interface");

  StatStart("GameDraw:end");

  // Display wind bar
  wind.Draw();
  StatStop("GameDraw:end");

  // Add one frame to the fps counter ;-)
  image_par_seconde.AjouteUneImage();
}

void GameLoop::CallDraw()
{
  Draw();
  StatStart("GameDraw:flip()");
  SDL_Flip( app.sdlwindow);
  StatStop("GameDraw:flip()");
}

void GameLoop::Run()
{
#ifdef ENABLE_LIMIT_FPS   
  uint sleep_fps=0;
  uint delay=0;
#endif 

  // boucle until game is finished
  do
  {
#ifdef ENABLE_LIMIT_FPS    
    unsigned int start = SDL_GetTicks();
#endif    
     
    game.SetEndOfGameStatus( false );

    // one loop
    StatStart("GameLoop:Refresh()");
    Refresh();
    StatStop("GameLoop:Refresh()");
    StatStart("GameLoop:Draw()");
    CallDraw ();
    StatStop("GameLoop:Draw()");

    // try to adjust to max Frame by seconds
#ifdef ENABLE_LIMIT_FPS    
    delay = SDL_GetTicks()-start;
     
    if (delay < video.GetSleepMaxFps())
      sleep_fps = video.GetSleepMaxFps() - delay;
    else
      sleep_fps = 0;
    SDL_Delay(sleep_fps);
#endif
  } while( !game.GetEndOfGameStatus() ); 

  global_particle_engine.Stop();
}

void GameLoop::RefreshClock()
{
  if (global_time.IsGamePaused()) return;

  if (1000 < global_time.Read() - pause_seconde) 
    {
      pause_seconde = global_time.Read();

      switch (state) {

      case gamePLAYING:
        if (duration <= 1) {
           jukebox.Play("share", "end_turn");
           SetState (gameEND_TURN);
        } else {
          duration--;
          interface.UpdateTimer(duration);
        }
        break;

      case gameHAS_PLAYED:
        if (duration <= 1) {
          SetState (gameEND_TURN);
        } else {
          duration--;
          interface.UpdateTimer(duration);
        }
        break;

      case gameEND_TURN:
        if (duration <= 1) {
          if (IsAnythingMoving()) break;

          if (game.IsGameFinished()) 
            game.SetEndOfGameStatus( true );
          else { 
            ActiveTeam().AccessWeapon().Deselect();    
            caisse.FaitApparaitre();
            SetState (gamePLAYING);
            break;
          }
        } else {
          duration--;
        }
        break;
      } // switch
    }// if
}

void GameLoop::SetState(game_state new_state, bool begin_game)
{
  // already in good state, nothing to do 
  if ((state == new_state) && !begin_game) return;

  state = new_state;
  action_handler.ExecActions();

  //
  interface.weapons_menu.Hide();

  switch (state)
  {
  // Début d'un tour
  case gamePLAYING:
    MSG_DEBUG("game.statechange", "Playing" );

    // Init. le compteur
    duration = game_mode.duration_turn;
    interface.UpdateTimer(duration);
    interface.EnableDisplayTimer(true);
    pause_seconde = global_time.Read();

#ifdef TODO_NETWORK 
    if (network.is_server() || network.is_local())
#endif
     wind.ChooseRandomVal();
    
     character_already_chosen = false;

    // Prépare un tour pour un ver
    POUR_TOUS_VERS_VIVANTS(equipe,ver) ver -> PrepareTour();

    // Changement d'équipe
    assert (!game.IsGameFinished());    
    do
    {
      teams_list.NextTeam (begin_game);
      action_handler.ExecActions();
    } while (ActiveTeam().NbAliveCharacter() == 0);
    if( game_mode.allow_character_selection==GameMode::CHANGE_ON_END_TURN
     || game_mode.allow_character_selection==GameMode::BEFORE_FIRST_ACTION_AND_END_TURN)
    {
            action_handler.NewAction(ActionInt(ACTION_CHANGE_CHARACTER,
                                     ActiveTeam().NextCharacterIndex()));
    }

    action_handler.ExecActions();

    assert (!ActiveCharacter().IsDead());
    camera.ChangeObjSuivi (&ActiveCharacter(), true, true);
    interaction_enabled = true; // Be sure that we can play !
    break;

  // Un ver a joué son arme, mais peut encore se déplacer
  case gameHAS_PLAYED:
    MSG_DEBUG("game.statechange", "Has played, now can move");
    duration = game_mode.duration_move_player;
    pause_seconde = global_time.Read();
    interface.UpdateTimer(duration);
    curseur_ver.Cache();
    break;

  // Fin du tour : petite pause
  case gameEND_TURN:
    MSG_DEBUG("game.statechange", "End of turn");
    ActiveTeam().AccessWeapon().SignalTurnEnd();
    curseur_ver.Cache();
    duration = game_mode.duration_exchange_player;
    interface.UpdateTimer(duration);
    interface.EnableDisplayTimer(false);
    pause_seconde = global_time.Read();

    interaction_enabled = false; // Be sure that we can NOT play !

    if (!IsAnythingMoving()) ActiveTeam().AccessWeapon().Deselect();
    break;
  }
}

PhysicalObj* GameLoop::GetMovingObject()
{
  if (!ActiveCharacter().IsReady()) return &ActiveCharacter();

  POUR_TOUS_VERS(equipe,ver)
  {
    if (!ver -> IsReady() && !ver -> IsGhost())
    {
      MSG_DEBUG("game.endofturn", Format("%s is not ready", (*ver).m_name.c_str()))
      return &(*ver);
    }
  }

  POUR_CHAQUE_OBJET(objet)
  {
    if (!objet -> ptr -> IsReady())
    {
      MSG_DEBUG("game.endofturn", Format("%s is not ready", objet->ptr->m_name.c_str() ))
      return objet -> ptr;
    }
  }

  return NULL;
}

bool GameLoop::IsAnythingMoving()
{
  // Is the weapon still active or an object still moving ??
  bool object_still_moving = false;

  if (ActiveTeam().GetWeapon().IsActive()) object_still_moving = true;
  
  if (!object_still_moving)
  {
    PhysicalObj *obj = GetMovingObject();
    if (obj != NULL)
    {
      camera.ChangeObjSuivi (obj, true, true);
      object_still_moving = true;
    } 
  }

  return object_still_moving;
}

// Signal death of a character
void GameLoop::SignalCharacterDeath (Character *character)
{
  std::string txt;

  if (!game.IsGameLaunched())
    return;

  if (character -> IsDrowned()) {
    txt = Format(_("%s has fallen in water."), character -> m_name.c_str());
    
  } else if (&ActiveCharacter() == character) { // Active Character is dead 
    curseur_ver.Cache();

    // Is this a suicide ?
    if (ActiveTeam().GetWeaponType() == WEAPON_SUICIDE) {
      txt = Format(_("%s commits suicide !"), character -> m_name.c_str());
      
      // Dead in moving ?
    } else if (state == gamePLAYING) {
      txt = Format(_("%s has fallen off the map!"),
                   character -> m_name.c_str());
       jukebox.Play(ActiveTeam().GetSoundProfile(), "out");
       
      // Mort en se faisant toucher par son arme / la mort d'un ennemi ?
    } else {
      txt = Format(_("%s is dead because he is clumsy!"), 
                   character -> m_name.c_str());
    }
  } else if ((!ActiveCharacter().IsDead())
             && (&character -> GetTeam() == &ActiveTeam())) {
    txt = Format(_("%s is a psychopath, he has killed a member of %s team!"),
                 ActiveCharacter().m_name.c_str(), character -> m_name.c_str());
  } else if (ActiveTeam().GetWeaponType() == WEAPON_GUN) {
    txt = Format(_("What a shame for %s - he was killed by a simple gun!"),
                 character -> m_name.c_str());
  } else {
    // Affiche la mort du ver
    txt = Format(_("%s (%s team) has died."),
                 character -> m_name.c_str(), 
                 character -> GetTeam().GetName().c_str());
  }
  
  game_messages.Add (txt);
  
  // Si c'est le ver actif qui est mort, fin du tour
  if (character == &ActiveCharacter()) SetState (gameEND_TURN);
}

// Signal falling (with damage) of a character
void GameLoop::SignalCharacterDamageFalling (Character *character)
{
  if (character == &ActiveCharacter())
    {
      SetState (gameEND_TURN);
    }
}
