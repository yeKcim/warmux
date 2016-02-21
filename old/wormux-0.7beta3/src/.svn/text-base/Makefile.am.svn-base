SUBDIRS = .

bin_PROGRAMS = wormux

pkgdatadir=$(datadir)/@DATADIR@
localedir=$(datadir)/@LOCALEDIR@

wormux_SOURCES = \
	include/base.h include/enum.h \
	include/constant.cpp include/constant.h \
	include/global.cpp include/global.h \
	include/action.cpp include/action.h \
	include/action_handler.cpp include/action_handler.h \
	include/app.h main.cpp \
	game/config.cpp game/config.h \
	game/game_mode.cpp game/game_mode.h \
	game/game.cpp game/game.h \
	game/game_loop.cpp game/game_loop.h \
	graphic/colors.h \
	graphic/fps.cpp graphic/fps.h \
	graphic/video.cpp graphic/video.h \
	graphic/text.cpp graphic/text.h \
	graphic/effects.cpp graphic/effects.h \
	gui/button.cpp gui/button.h \
	gui/button_text.cpp gui/button_text.h \
	gui/check_box.cpp gui/check_box.h \
	gui/label.cpp gui/label.h \
	gui/list_box.cpp gui/list_box.h \
	gui/spin_button.cpp gui/spin_button.h \
	gui/box.cpp gui/box.h \
	gui/widget.cpp gui/widget.h \
	gui/null_widget.cpp gui/null_widget.h \
	tool/string_tools.cpp tool/string_tools.h \
	tool/xml_document.cpp tool/xml_document.h \
	tool/i18n.cpp tool/i18n.h \
	tool/error.cpp tool/error.h \
	tool/file_tools.cpp tool/file_tools.h \
	tool/math_tools.cpp tool/math_tools.h \
	tool/Rectangle.h tool/Point.h \
	tool/resource_manager.cpp tool/resource_manager.h \
	tool/stats.cpp tool/stats.h \
	tool/random.cpp tool/random.h \
	tool/debug.cpp tool/debug.h \
	sound/jukebox.cpp sound/jukebox.h \
	graphic/font.cpp graphic/font.h \
	graphic/sprite.cpp graphic/sprite.h \
	interface/weapon_menu.cpp interface/weapon_menu.h \
	menu/main_menu.cpp menu/main_menu.h \
	menu/menu.cpp menu/menu.h \
	menu/infos_menu.cpp menu/infos_menu.h \
	menu/options_menu.cpp menu/options_menu.h \
	menu/game_menu.cpp menu/game_menu.h \
	game/time.cpp game/time.h \
	gui/progress_bar.cpp gui/progress_bar.h \
	gui/question.cpp gui/question.h \
	interface/interface.cpp interface/interface.h \
	interface/cursor.cpp interface/cursor.h \
	interface/game_msg.cpp interface/game_msg.h \
	interface/mouse.cpp interface/mouse.h \
	interface/keyboard.cpp interface/keyboard.h \
	object/physical_obj.cpp object/physical_obj.h \
	object/physics.cpp object/physics.h \
	object/bonus_box.cpp object/bonus_box.h \
	object/objects_list.cpp object/objects_list.h \
	object/particle.cpp object/particle.h \
	map/map.cpp map/map.h \
	map/maps_list.cpp map/maps_list.h \
	map/sky.cpp map/sky.h \
	map/tile.cpp map/tile.h \
	map/water.cpp map/water.h \
	map/wind.cpp map/wind.h \
	map/camera.cpp map/camera.h \
	map/ground.cpp map/ground.h \
	team/macro.h \
	team/character.cpp team/character.h \
	team/move.cpp team/move.h \
	team/skin.cpp team/skin.h \
	team/team.cpp team/team.h \
	team/team_energy.cpp team/team_energy.h \
	team/teams_list.cpp team/teams_list.h \
	weapon/all.h \
	weapon/weapon.cpp weapon/weapon.h \
	weapon/weapon_cfg.cpp weapon/weapon_cfg.h \
	weapon/weapons_list.cpp weapon/weapons_list.h \
	weapon/weapon_tools.cpp weapon/weapon_tools.h \
	weapon/crosshair.cpp weapon/crosshair.h \
	weapon/air_attack.cpp weapon/air_attack.h \
	weapon/auto_bazooka.cpp weapon/auto_bazooka.h \
	weapon/baseball.cpp weapon/baseball.h \
	weapon/bazooka.cpp weapon/bazooka.h \
	weapon/cluster_bomb.cpp weapon/cluster_bomb.h \
	weapon/dynamite.cpp weapon/dynamite.h \
	weapon/gnu.cpp weapon/gnu.h \
	weapon/grenade.cpp weapon/grenade.h \
	weapon/gun.cpp weapon/gun.h \
	weapon/mine.cpp weapon/mine.h \
	weapon/skip_turn.cpp weapon/skip_turn.h \
	weapon/suicide.cpp weapon/suicide.h \
	weapon/teleportation.cpp weapon/teleportation.h \
	weapon/holly_grenade.cpp weapon/holly_grenade.h \
	weapon/airhammer.cpp weapon/airhammer.h \
	weapon/uzi.cpp weapon/uzi.h \
	weapon/jetpack.cpp weapon/jetpack.h\
	weapon/lowgrav.cpp weapon/lowgrav.h \
	weapon/ninja_rope.cpp weapon/ninja_rope.h \
	weapon/parachute.cpp weapon/parachute.h \
	weapon/supertux.cpp weapon/supertux.h

#AM_CXXFLAGS = -pg @SDL_CFLAGS@ @LIBXMLPP_CFLAGS@ @PARAGUI_CFLAGS@ -DINSTALL_DATADIR=\"$(pkgdatadir)\" -DINSTALL_LOCALEDIR=\"$(localedir)\"
#LIBS = $(SDL_LIBS) $(LIBXMLPP_LIBS) $(PARAGUI_LIBS)

AM_CXXFLAGS = -DINSTALL_DATADIR=\"$(pkgdatadir)\" -DINSTALL_LOCALEDIR=\"$(localedir)\"
