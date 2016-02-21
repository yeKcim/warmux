SUBDIRS = .

bin_PROGRAMS = wormux

wormux_SOURCES = \
        ai/ai_engine.cpp ai/ai_engine.h \
        ai/ai_engine_stupid.cpp ai/ai_engine_stupid.h \
        ai/ai_movement_module.cpp ai/ai_movement_module.h \
        ai/ai_shoot_module.cpp ai/ai_shoot_module.h \
        character/body.cpp character/body.h \
        character/body_list.cpp character/body_list.h \
        character/character.cpp character/character.h \
        character/clothe.cpp character/clothe.h \
        character/damage_stats.cpp character/damage_stats.h \
        character/member.cpp character/member.h \
        character/move.cpp character/move.h \
        character/movement.cpp character/movement.h \
        game/config.cpp game/config.h \
        game/game.cpp game/game.h \
        game/game_blitz.cpp game/game_blitz.h \
        game/game_classic.cpp game/game_classic.h \
        game/game_init.cpp game/game_init.h \
        game/game_mode.cpp game/game_mode.h \
        game/time.cpp game/time.h \
        graphic/color.cpp graphic/color.h \
        graphic/colors.h \
        graphic/composite_shape.cpp graphic/composite_shape.h \
        graphic/effects.cpp graphic/effects.h \
        graphic/fading_effect.h \
        graphic/font.cpp graphic/font.h \
        graphic/fps.cpp graphic/fps.h \
        graphic/polygon.cpp graphic/polygon.h \
        graphic/polygon_generator.cpp graphic/polygon_generator.h \
        graphic/spriteanimation.cpp graphic/spriteanimation.h \
        graphic/spritecache.cpp graphic/spritecache.h \
        graphic/sprite.cpp graphic/sprite.h \
        graphic/spriteframe.cpp graphic/spriteframe.h \
        graphic/surface.cpp graphic/surface.h \
        graphic/text.cpp graphic/text.h \
        graphic/text_list.cpp graphic/text_list.h \
        graphic/textured_polygon.h \
        graphic/video.cpp graphic/video.h \
        gui/abstract_spin_button.h \
        gui/box.cpp gui/box.h \
        gui/button.cpp gui/button.h \
        gui/button_text.cpp gui/button_text.h \
        gui/check_box.cpp gui/check_box.h \
        gui/combo_box.cpp gui/combo_box.h \
        gui/container.h \
        gui/energy_bar.cpp gui/energy_bar.h \
        gui/label.cpp gui/label.h \
        gui/list_box.cpp gui/list_box.h \
        gui/list_box_w_label.cpp gui/list_box_w_label.h \
        gui/msg_box.cpp gui/msg_box.h \
        gui/null_widget.h \
        gui/picture_text_cbox.cpp gui/picture_text_cbox.h \
        gui/picture_widget.cpp gui/picture_widget.h \
        gui/progress_bar.cpp gui/progress_bar.h \
        gui/question.cpp gui/question.h \
        gui/spin_button_big.cpp gui/spin_button_big.h \
        gui/spin_button.cpp gui/spin_button.h \
        gui/spin_button_picture.cpp gui/spin_button_picture.h \
	gui/tabs.cpp gui/tabs.h \
	gui/talk_box.cpp gui/talk_box.h \
        gui/text_box.cpp gui/text_box.h \
        gui/widget.cpp gui/widget.h \
        gui/widget_list.cpp gui/widget_list.h \
        gui/big/button_pic.cpp gui/big/button_pic.h \
        include/action.cpp include/action.h \
        include/action_handler.cpp include/action_handler.h \
        include/app.h main.cpp \
        include/base.h \
        include/binreloc.h include/binreloc.c \
        include/constant.cpp include/constant.h \
        include/singleton.cpp include/singleton.h \
        interface/cursor.cpp interface/cursor.h \
        interface/game_msg.cpp interface/game_msg.h \
        interface/interface.cpp interface/interface.h \
        interface/joystick.cpp interface/joystick.h \
        interface/keyboard.cpp interface/keyboard.h \
        interface/loading_screen.cpp interface/loading_screen.h \
        interface/man_machine_interface.cpp interface/man_machine_interface.h \
        interface/mouse.cpp interface/mouse.h \
        interface/mouse_cursor.cpp interface/mouse_cursor.h \
        interface/weapon_menu.cpp interface/weapon_menu.h \
        map/camera.cpp map/camera.h \
        map/ground.cpp map/ground.h \
        map/map.cpp map/map.h \
        map/maps_list.cpp map/maps_list.h \
        map/random_map.cpp map/random_map.h \
        map/sky.cpp map/sky.h \
        map/tile.cpp map/tile.h \
        map/tileitem.cpp map/tileitem.h \
        map/water.cpp map/water.h \
        map/wind.cpp map/wind.h \
        menu/credits_menu.cpp menu/credits_menu.h \
        menu/game_menu.cpp menu/game_menu.h \
	menu/game_mode_editor.cpp menu/game_mode_editor.h \
        menu/help_menu.cpp menu/help_menu.h \
        menu/main_menu.cpp menu/main_menu.h \
        menu/map_selection_box.cpp menu/map_selection_box.h \
        menu/menu.cpp menu/menu.h \
        menu/network_connection_menu.cpp menu/network_connection_menu.h \
        menu/network_menu.cpp menu/network_menu.h \
        menu/network_teams_selection_box.cpp menu/network_teams_selection_box.h \
        menu/options_menu.cpp menu/options_menu.h \
        menu/pause_menu.cpp menu/pause_menu.h \
        menu/results_menu.cpp menu/results_menu.h \
        menu/skin_menu.cpp menu/skin_menu.h \
        menu/team_box.cpp menu/team_box.h \
        menu/teams_selection_box.cpp menu/teams_selection_box.h \
        network/admin_commands.cpp network/admin_commands.h \
        network/chat.cpp network/chat.h \
        network/chatlogger.cpp network/chatlogger.h \
        network/distant_cpu.cpp network/distant_cpu.h \
        network/download.cpp network/download.h \
        network/index_server.cpp network/index_server.h \
        network/index_svr_msg.h \
        network/irc.cpp network/irc.h \
        network/net_error_msg.cpp network/net_error_msg.h \
        network/network_client.cpp network/network_client.h \
        network/network.cpp network/network.h \
        network/network_local.cpp network/network_local.h \
        network/network_server.cpp network/network_server.h \
        network/randomsync.cpp network/randomsync.h \
        network/socket.cpp network/socket.h \
        object/barrel.cpp object/barrel.h \
        object/bonus_box.cpp object/bonus_box.h \
        object/medkit.cpp  object/medkit.h \
        object/objbox.cpp object/objbox.h \
        object/object_cfg.cpp object/object_cfg.h \
        object/objects_list.cpp object/objects_list.h \
        object/physical_obj.cpp object/physical_obj.h \
        object/physics.cpp object/physics.h \
        particles/body_member.cpp particles/body_member.h \
        particles/bullet.cpp particles/bullet.h \
        particles/dark_smoke.cpp particles/dark_smoke.h \
        particles/explosion_smoke.cpp particles/explosion_smoke.h \
        particles/fading_text.cpp particles/fading_text.h \
        particles/fire.cpp particles/fire.h \
        particles/ground_particles.cpp particles/ground_particles.h \
        particles/ill_bubble.cpp particles/ill_bubble.h \
        particles/magic_star.cpp particles/magic_star.h \
        particles/particle.cpp particles/particle.h \
        particles/polecat_fart.cpp particles/polecat_fart.h \
        particles/smoke.cpp particles/smoke.h \
        particles/star.cpp particles/star.h \
        particles/teleport_member.cpp particles/teleport_member.h \
        particles/water_drop.cpp particles/water_drop.h \
        sound/jukebox.cpp sound/jukebox.h \
        sound/sound_sample.cpp sound/sound_sample.h \
        sound/sample_cache.cpp sound/sample_cache.h \
        team/custom_team.cpp team/custom_team.h \
        team/custom_teams_list.cpp team/custom_teams_list.h \
		team/macro.h \
        team/results.cpp team/results.h \
        team/team_config.h \
        team/team.cpp team/team.h \
        team/team_energy.cpp team/team_energy.h \
        team/teams_list.cpp team/teams_list.h \
        tool/affine_transform.cpp tool/affine_transform.h \
        tool/copynpaste.cpp tool/copynpaste.h \
        tool/debug.cpp tool/debug.h \
        tool/error.cpp tool/error.h \
        tool/euler_vector.cpp tool/euler_vector.h \
        tool/file_tools.cpp tool/file_tools.h \
        tool/i18n.cpp tool/i18n.h \
        tool/isnan.h \
        tool/math_tools.h \
        tool/random.cpp tool/random.h \
        tool/rectangle.h tool/point.h \
        tool/resource_manager.cpp tool/resource_manager.h \
        tool/stats.cpp tool/stats.h \
        tool/string_tools.cpp tool/string_tools.h \
        tool/text_handling.cpp tool/text_handling.h \
        tool/vector2.h \
        tool/xml_document.cpp tool/xml_document.h \
        weapon/air_attack.cpp weapon/air_attack.h \
        weapon/airhammer.cpp weapon/airhammer.h \
        weapon/all.h \
        weapon/anvil.cpp weapon/anvil.h \
        weapon/auto_bazooka.cpp weapon/auto_bazooka.h \
        weapon/baseball.cpp weapon/baseball.h \
        weapon/bazooka.cpp weapon/bazooka.h \
        weapon/blowtorch.cpp weapon/blowtorch.h \
        weapon/bounce_ball.cpp weapon/bounce_ball.h \
        weapon/cluzooka.cpp weapon/cluzooka.h \
        weapon/cluster_bomb.cpp weapon/cluster_bomb.h \
        weapon/footbomb.cpp weapon/footbomb.h \
        weapon/construct.cpp weapon/construct.h \
        weapon/crosshair.cpp weapon/crosshair.h \
        weapon/disco_grenade.cpp weapon/disco_grenade.h \
        weapon/dynamite.cpp weapon/dynamite.h \
        weapon/explosion.cpp weapon/explosion.h \
        weapon/flamethrower.cpp weapon/flamethrower.h \
        weapon/gnu.cpp weapon/gnu.h \
        weapon/grapple.cpp weapon/grapple.h \
        weapon/grenade.cpp weapon/grenade.h \
        weapon/gun.cpp weapon/gun.h \
        weapon/jetpack.cpp weapon/jetpack.h\
        weapon/lowgrav.cpp weapon/lowgrav.h \
        weapon/mine.cpp weapon/mine.h \
        weapon/parachute.cpp weapon/parachute.h \
        weapon/polecat.cpp weapon/polecat.h \
        weapon/riot_bomb.cpp weapon/riot_bomb.h \
        weapon/shotgun.cpp weapon/shotgun.h \
        weapon/skip_turn.cpp weapon/skip_turn.h \
        weapon/slap.cpp weapon/slap.h \
        weapon/snipe_rifle.cpp weapon/snipe_rifle.h \
        weapon/submachine_gun.cpp weapon/submachine_gun.h \
        weapon/suicide.cpp weapon/suicide.h \
        weapon/supertux.cpp weapon/supertux.h \
        weapon/syringe.cpp weapon/syringe.h \
        weapon/teleportation.cpp weapon/teleportation.h \
        weapon/weapon_cfg.cpp weapon/weapon_cfg.h \
        weapon/weapon.cpp weapon/weapon.h \
        weapon/weapon_launcher.cpp weapon/weapon_launcher.h \
        weapon/weapons_list.cpp weapon/weapons_list.h \
        weapon/weapon_strength_bar.h

if AUTOPACKAGE
DEFINE_AUTOPACKAGE=-DUSE_AUTOPACKAGE
endif

AM_CXXFLAGS = -I $(top_srcdir)/src -DINSTALL_DATADIR=\"@DATADIR@\"      \
  -DINSTALL_LOCALEDIR=\"@LOCALEDIR@\" -DFONT_FILE=\"@FONTFILE@\"        \
  -DPACKAGE_VERSION=\"$(PACKAGE_VERSION)\" $(DEFINE_AUTOPACKAGE)        \
  @LIBCURL_CPPFLAGS@

if WIN32
wormux_SOURCES += wormux.rc wormux.ico
.rc.o:
	@WINDRES@ -o $@ -i $<
endif
AM_CXXFLAGS += -Werror

if !INCLUDEDINTL
LIBS+=@LIBINTL@
endif

if STATIC
LIBS += -lssl -lcrypto -lz -lpthread
else
LIBS += @LIBCURL@
endif
