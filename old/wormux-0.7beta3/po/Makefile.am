# Example for use of GNU gettext.
# Copyright (C) 2003 Free Software Foundation, Inc.
# This file is in the public domain.
#
# Makefile configuration - processed by automake.
#

MAINTAINERCLEANFILES = Makefile.in

POTFILES = \
	src/game/config.cpp \
	src/game/game.cpp \
	src/game/game_loop.cpp \
	src/game/game_mode.cpp \
	src/game/time.cpp \
	src/gui/button.cpp \
	src/gui/button_text.cpp \
	src/gui/check_box.cpp \
	src/gui/list_box.cpp \
	src/gui/progress_bar.cpp \
	src/gui/question.cpp \
	src/gui/spin_button.cpp \
	src/main.cpp \
	src/graphic/font.cpp \
	src/graphic/fps.cpp \
	src/graphic/video.cpp \
	src/include/constant.cpp \
	src/interface/cursor.cpp \
	src/interface/game_msg.cpp \
	src/interface/interface.cpp \
	src/interface/keyboard.cpp \
	src/interface/mouse.cpp \
	src/interface/weapon_menu.cpp \
	src/map/camera.cpp \
	src/map/ground.cpp \
	src/map/map.cpp \
	src/map/maps_list.cpp \
	src/map/sky.cpp \
	src/map/tile.cpp \
	src/map/water.cpp \
	src/map/wind.cpp \
	src/menu/game_menu.cpp \
	src/menu/infos_menu.cpp \
	src/menu/main_menu.cpp \
	src/menu/options_menu.cpp \
	src/object/bonus_box.cpp \
	src/object/objects_list.cpp \
	src/object/physical_obj.cpp \
	src/sound/jukebox.cpp \
	src/team/character.cpp \
	src/team/move.cpp \
	src/team/skin.cpp \
	src/team/team.cpp \
	src/team/team_energy.cpp \
	src/team/teams_list.cpp \
	src/tool/error.cpp \
	src/tool/file_tools.cpp \
	src/tool/i18n.cpp \
	src/tool/math_tools.cpp \
	src/tool/random.cpp \
	src/tool/string_tools.cpp \
	src/tool/xml_document.cpp \
	src/weapon/air_attack.cpp \
	src/weapon/auto_bazooka.cpp \
	src/weapon/baseball.cpp \
	src/weapon/bazooka.cpp \
	src/weapon/cluster_bomb.cpp \
	src/weapon/crosshair.cpp \
	src/weapon/dynamite.cpp \
	src/weapon/grenade.cpp \
	src/weapon/gun.cpp \
	src/weapon/mine.cpp \
	src/weapon/suicide.cpp \
	src/weapon/teleportation.cpp \
	src/weapon/weapon.cpp \
	src/weapon/weapon_tools.cpp \
	src/weapon/weapons_list.cpp

DOMAIN = $(PACKAGE)

COPYRIGHT_HOLDER = Wormux Development Team

MSGID_BUGS_ADDRESS = wormux-dev@gna.org

localedir=$(datadir)/@LOCALEDIR@

XGETTEXT = xgettext
MSGMERGE = msgmerge
MSGMERGE_UPDATE = @MSGMERGE@ --quiet --update
MSGINIT = msginit
MSGCONV = msgconv
MSGFILTER = msgfilter

# This is computed as $(foreach file, $(POTFILES), $(top_srcdir)/$(file))
POTFILES_DEPS = @POTFILES_DEPS@

# This is computed as $(foreach lang, $(LINGUAS), $(srcdir)/$(lang).po)
POFILES = @POFILES@
# This is computed as $(foreach lang, $(LINGUAS), $(srcdir)/$(lang).gmo)
GMOFILES = @GMOFILES@
# This is computed as $(foreach lang, $(LINGUAS), $(lang).po-update)
UPDATEPOFILES = @UPDATEPOFILES@
# This is computed as $(foreach lang, $(LINGUAS), $(lang).nop)
DUMMYPOFILES = @DUMMYPOFILES@

# This is computed as
# $(foreach lang, user-specified subset of $(LINGUAS), $(lang).gmo)
CATALOGS = @CATALOGS@

SUFFIXES = .po .gmo .mo .sed .sin .nop .po-update

.po.gmo:
	@lang=`echo $* | sed -e 's,.*/,,'`; \
	test "$(srcdir)" = . && cdcmd="" || cdcmd="cd $(srcdir) && "; \
	echo "Create $${lang}.gmo"; \
	cd $(srcdir) && rm -f $${lang}.gmo && $(GMSGFMT) -c -o t-$${lang}.gmo $${lang}.po && mv t-$${lang}.gmo $${lang}.gmo

.sin.sed:
	@echo "Create $@ file"
	@sed -e '/^#/d' $< > t-$@
	@mv t-$@ $@


all-local: all-local-@USE_NLS@ $(GMOFILES)

all-local-yes: stamp-po
all-local-no:

# stamp-po is a timestamp denoting the last time at which the CATALOGS have
# been loosely updated. Its purpose is that when a developer or translator
# checks out the package via CVS, and the $(DOMAIN).pot file is not in CVS,
# "make" will update the $(DOMAIN).pot and the $(CATALOGS), but subsequent
# invocations of "make" will do nothing. This timestamp would not be necessary
# if updating the $(CATALOGS) would always touch them; however, the rule for
# $(POFILES) has been designed to not touch files that don't need to be
# changed.
stamp-po: $(srcdir)/$(DOMAIN).pot
	test -z "$(GMOFILES)" || $(MAKE) $(GMOFILES)
	@echo "touch stamp-po"
	@echo timestamp > stamp-poT
	@mv stamp-poT stamp-po

# Note: Target 'all' must not depend on target '$(DOMAIN).pot-update',
# otherwise packages like GCC can not be built if only parts of the source
# have been downloaded.

# This target rebuilds $(DOMAIN).pot; it is an expensive operation.
# Note that $(DOMAIN).pot is not touched if it doesn't need to be changed.
$(DOMAIN).pot-update: $(POTFILES_DEPS)
	@echo "Create $(DOMAIN).pot"
	@$(XGETTEXT) \
	  -o $(DOMAIN).pot -k'_' -s --from-code iso-8859-1 \
	  --default-domain=$(DOMAIN) --directory=$(top_srcdir) \
	  --add-comments=TRANSLATORS: $(XGETTEXT_OPTIONS) \
	  --copyright-holder='$(COPYRIGHT_HOLDER)' \
	  --msgid-bugs-address='$(MSGID_BUGS_ADDRESS)' \
	  $(POTFILES)
	@test ! -f $(DOMAIN).po || { \
	  if test -f $(srcdir)/$(DOMAIN).pot; then \
	    sed -e '/^"POT-Creation-Date:/D' < $(srcdir)/$(DOMAIN).pot > $(DOMAIN).1po && \
	    sed -e '/^"POT-Creation-Date:/D' < $(DOMAIN).po > $(DOMAIN).2po && \
	    if cmp $(DOMAIN).1po $(DOMAIN).2po >/dev/null 2>&1; then \
	      rm -f $(DOMAIN).1po $(DOMAIN).2po $(DOMAIN).po; \
	    else \
	      rm -f $(DOMAIN).1po $(DOMAIN).2po $(srcdir)/$(DOMAIN).pot && \
	      mv $(DOMAIN).po $(srcdir)/$(DOMAIN).pot; \
	    fi; \
	  else \
	    mv $(DOMAIN).po $(srcdir)/$(DOMAIN).pot; \
	  fi; \
	}

# This rule has no dependencies: we don't need to update $(DOMAIN).pot at
# every "make" invocation, only create it when it is missing.
# Only "make $(DOMAIN).pot-update" or "make dist" will force an update.
$(srcdir)/$(DOMAIN).pot:
	$(MAKE) $(DOMAIN).pot-update

# This target rebuilds a PO file if $(DOMAIN).pot has changed.
# Note that a PO file is not touched if it doesn't need to be changed.
$(POFILES): $(srcdir)/$(DOMAIN).pot
	@lang=`echo $@ | sed -e 's,.*/,,' -e 's/\.po$$//'`; \
	test "$(srcdir)" = . && cdcmd="" || cdcmd="cd $(srcdir) && "; \
	echo "Update $${lang}.po"; \
	cd $(srcdir) && $(MSGMERGE_UPDATE) $${lang}.po $(DOMAIN).pot && \
	sed -e '/^"POT-Creation-Date:/D' <$${lang}.po >t-$${lang}.po && \
	mv -f t-$${lang}.po $${lang}.po 


install-data-local: install-data-local-@USE_NLS@
install-data-local-no: all-local
install-data-local-yes: all-local
	$(mkinstalldirs) $(DESTDIR)$(datadir)
	for cat in $(CATALOGS); do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed -e 's/\.gmo$$//'`; \
	  dir=$(localedir)/$$lang/LC_MESSAGES; \
	  $(mkinstalldirs) $(DESTDIR)$$dir; \
	  if test -r $$cat; then realcat=$$cat; else realcat=$(srcdir)/$$cat; fi; \
	  $(INSTALL_DATA) $$realcat $(DESTDIR)$$dir/$(DOMAIN).mo; \
	  echo "installing $$realcat as $(DESTDIR)$$dir/$(DOMAIN).mo"; \
	  for lc in '' $(EXTRA_LOCALE_CATEGORIES); do \
	    if test -n "$$lc"; then \
	      if (cd $(DESTDIR)$(localedir)/$$lang && LC_ALL=C ls -l -d $$lc 2>/dev/null) | grep ' -> ' >/dev/null; then \
	        link=`cd $(DESTDIR)$(localedir)/$$lang && LC_ALL=C ls -l -d $$lc | sed -e 's/^.* -> //'`; \
	        mv $(DESTDIR)$(localedir)/$$lang/$$lc $(DESTDIR)$(localedir)/$$lang/$$lc.old; \
	        mkdir $(DESTDIR)$(localedir)/$$lang/$$lc; \
	        (cd $(DESTDIR)$(localedir)/$$lang/$$lc.old && \
	         for file in *; do \
	           if test -f $$file; then \
	             ln -s ../$$link/$$file $(DESTDIR)$(localedir)/$$lang/$$lc/$$file; \
	           fi; \
	         done); \
	        rm -f $(DESTDIR)$(localedir)/$$lang/$$lc.old; \
	      else \
	        if test -d $(DESTDIR)$(localedir)/$$lang/$$lc; then \
	          :; \
	        else \
	          rm -f $(DESTDIR)$(localedir)/$$lang/$$lc; \
	          mkdir $(DESTDIR)$(localedir)/$$lang/$$lc; \
	        fi; \
	      fi; \
	      rm -f $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo; \
	      ln -s ../LC_MESSAGES/$(DOMAIN).mo $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo 2>/dev/null || \
	      ln $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(DOMAIN).mo $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo 2>/dev/null || \
	      cp -p $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(DOMAIN).mo $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo; \
	      echo "installing $$realcat link as $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo"; \
	    fi; \
	  done; \
	done

installdirs-local: installdirs-local-@USE_NLS@
installdirs-local-no:
installdirs-local-yes:
	$(mkinstalldirs) $(DESTDIR)$(datadir)
	@catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed -e 's/\.gmo$$//'`; \
	  dir=$(localedir)/$$lang/LC_MESSAGES; \
	  $(mkinstalldirs) $(DESTDIR)$$dir; \
	  for lc in '' $(EXTRA_LOCALE_CATEGORIES); do \
	    if test -n "$$lc"; then \
	      if (cd $(DESTDIR)$(localedir)/$$lang && LC_ALL=C ls -l -d $$lc 2>/dev/null) | grep ' -> ' >/dev/null; then \
	        link=`cd $(DESTDIR)$(localedir)/$$lang && LC_ALL=C ls -l -d $$lc | sed -e 's/^.* -> //'`; \
	        mv $(DESTDIR)$(localedir)/$$lang/$$lc $(DESTDIR)$(localedir)/$$lang/$$lc.old; \
	        mkdir $(DESTDIR)$(localedir)/$$lang/$$lc; \
	        (cd $(DESTDIR)$(localedir)/$$lang/$$lc.old && \
	         for file in *; do \
	           if test -f $$file; then \
	             ln -s ../$$link/$$file $(DESTDIR)$(localedir)/$$lang/$$lc/$$file; \
	           fi; \
	         done); \
	        rm -f $(DESTDIR)$(localedir)/$$lang/$$lc.old; \
	      else \
	        if test -d $(DESTDIR)$(localedir)/$$lang/$$lc; then \
	          :; \
	        else \
	          rm -f $(DESTDIR)$(localedir)/$$lang/$$lc; \
	          mkdir $(DESTDIR)$(localedir)/$$lang/$$lc; \
	        fi; \
	      fi; \
	    fi; \
	  done; \
	done

uninstall-local: uninstall-local-@USE_NLS@
uninstall-local-no:
uninstall-local-yes:
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed -e 's/\.gmo$$//'`; \
	  for lc in LC_MESSAGES $(EXTRA_LOCALE_CATEGORIES); do \
	    rm -f $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo; \
	  done; \
	done

html ID:

MOSTLYCLEANFILES =
MOSTLYCLEANFILES += stamp-poT
MOSTLYCLEANFILES += core core.* $(DOMAIN).po $(DOMAIN).1po $(DOMAIN).2po *.new.po
MOSTLYCLEANFILES += *.o *.gmo $(srcdir)/$(DOMAIN).pot *.*~

DISTCLEANFILES = *.mo *.gmo

MAINTAINERCLEANFILES += stamp-po $(GMOFILES)

EXTRA_DIST = LINGUAS $(DOMAIN).pot stamp-po $(POFILES)

# Hidden from automake, but really activated. Works around an automake-1.5 bug.
#distdir: distdir1
distdir1:
	$(MAKE) update-po

update-po: Makefile
	$(MAKE) $(DOMAIN).pot-update
	test -z "$(UPDATEPOFILES)" || $(MAKE) $(UPDATEPOFILES)
	$(MAKE) update-gmo

# General rule for updating PO files.

.nop.po-update:
	@lang=`echo $@ | sed -e 's/\.po-update$$//'`; \
	tmpdir=`pwd`; \
	echo "$$lang:"; \
	test "$(srcdir)" = . && cdcmd="" || cdcmd="cd $(srcdir) && "; \
	echo "$${cdcmd}$(MSGMERGE) $$lang.po $(DOMAIN).pot -o $$lang.new.po"; \
	cd $(srcdir); \
	if $(MSGMERGE) $$lang.po $(DOMAIN).pot -o $$tmpdir/$$lang.new.po; then \
	  if cmp $$lang.po $$tmpdir/$$lang.new.po >/dev/null 2>&1; then \
	    rm -f $$tmpdir/$$lang.new.po; \
	  else \
	    if mv -f $$tmpdir/$$lang.new.po $$lang.po; then \
	      :; \
	    else \
	      echo "msgmerge for $$lang.po failed: cannot move $$tmpdir/$$lang.new.po to $$lang.po" 1>&2; \
	      exit 1; \
	    fi; \
	  fi; \
	else \
	  echo "msgmerge for $$lang.po failed!" 1>&2; \
	  rm -f $$tmpdir/$$lang.new.po; \
	fi

$(DUMMYPOFILES):

update-gmo: Makefile $(GMOFILES)
	@:
