SUBDIRS = data lib src po doc

EXTRA_DIST = build/visual/gettext.rules            \
             build/visual/wormux.sln               \
             build/visual/wormux.vcproj            \
             data/wormux.desktop                   \
             doc/license/COPYING.de.txt            \
             doc/license/COPYING.el.txt            \
             doc/license/COPYING.en.txt            \
             doc/license/COPYING.eo.txt            \
             doc/license/COPYING.es.txt            \
             doc/license/COPYING.fr.txt            \
             doc/license/COPYING.nl.txt            \
             doc/license/COPYING.pl.txt            \
             doc/license/COPYING.pt.txt            \
             doc/license/COPYING.ru.txt            \
             doc/license/COPYING.sk.txt            \
             doc/howto_play/de.pdf                 \
             doc/howto_play/en.pdf                 \
             doc/howto_play/fr.pdf                 \
             doc/howto_play/gl.pdf                 \
             doc/howto_play/nl.pdf                 \
             doc/howto_play/pl.pdf                 \
             doc/howto_play/sk.pdf                 \
             doc/howto_play/sv.pdf                 \
             doc/howto_play_online/de.pdf          \
             doc/howto_play_online/en.pdf          \
             doc/howto_play_online/nl.pdf          \
             src/msvc/getopt.c                     \
	     build/mac/Info.plist.in               \
             build/mac/PkgInfo.in                  \
             build/mac/README-mac.txt              \
             build/mac/README-mac.txt              \
             build/mac/cmake_build.sh              \
	     build/win32_setup/create_setup.sh.in  \
             build/win32_setup/English.nsh         \
             build/win32_setup/French.nsh          \
             build/win32_setup/Greek.nsh           \
             build/win32_setup/Polish.nsh          \
             build/win32_setup/langmacros.nsh      \
             build/win32_setup/install.ico         \
             build/win32_setup/uninstall.ico

if WIN32
install: all
	@echo "Building Win32 .exe installer in build/win32_setup"
	@(cd build/win32_setup && ./create_setup.sh 2> installer.log 1>&2 && \
          rm installer.log && echo "Success!") || \
         (echo "Installer compilation failed!"; \
          echo "Check build/win32_setup/installer.log for details"; \
          exit 1)
else
SUBDIRS+=tools
endif

# make run: run the game without installing it. Saves you the hassle
#           to run make install or set the correct environment
#           variables.
run: all
	WORMUX_DATADIR=@srcdir@/data/ WORMUX_FONT_PATH=@srcdir@/data/font/DejaVuSans.ttf @srcdir@/src/wormux

# make debug: run the game in gdb without installing it.
#             requires gdb to be in your $PATH.
debug: all
	WORMUX_DATADIR=@srcdir@/data/ WORMUX_FONT_PATH=@srcdir@/data/font/DejaVuSans.ttf gdb @srcdir@/src/wormux
