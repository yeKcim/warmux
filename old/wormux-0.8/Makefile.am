SUBDIRS = data src po doc

EXTRA_DIST = data/wormux.desktop                   \
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
             doc/howto_play/pl.pdf                 \
             doc/howto_play/sk.pdf                 \
             doc/howto_play/sv.pdf                 \
			 mac/Info.plist.in					   \
			 mac/PkgInfo.in						   \
			 mac/README-mac.txt					   \
			 mac/build_wormux_fink.sh			   \
			 mac/build_wormux_first.sh			   \
			 mac/build_wormux_macports.sh		   \
             visual/wormux.vcproj                  \
             src/msvc/config.h                     \
             src/msvc/getopt.c                     \
             src/msvc/getopt.h                     \
             tools/win32_setup/create_setup.sh     \
             tools/win32_setup/English.nsh         \
             tools/win32_setup/French.nsh          \
             tools/win32_setup/langmacros.nsh      \
             tools/win32_setup/install.ico         \
             tools/win32_setup/uninstall.ico

if WIN32
install: all
	@echo "Building Win32 .exe installer in tools/win32_setup"
	@(cd tools/win32_setup && sh create_setup.sh 2> installer.log 1>&2 && \
          rm installer.log && echo "Success!") || \
         (echo "Installer compilation failed!"; \
          echo "Check tools/win32_setup/installer.log for details"; \
          exit 1)
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
