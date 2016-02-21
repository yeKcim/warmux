SUBDIRS = data lib src

if NLS
SUBDIRS += po
endif

EXTRA_DISTDIRS = build/android/jni build/android/src build/android/res build/symbian
EXTRA_DIST = build/visual/gettext.rules            \
             build/visual/warmux.sln               \
             build/visual/warmux.vcproj            \
             data/warmux.desktop                   \
             data/warmux_files.desktop             \
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
             build/android/AndroidManifest.xml     \
             build/android/build.xml               \
             build/android/default.properties      \
             $(foreach D, $(EXTRA_DISTDIRS), $(shell find $(D) -name .svn -a -type d -prune -o -type f -print))

if WIN32
install: all
	@echo "Building Win32 .exe installer in build/win32_setup"
	@(cd build/win32_setup && ./create_setup.sh 2> installer.log 1>&2 && \
          rm installer.log && echo "Success!") || \
         (echo "Installer compilation failed!"; \
          echo "Check build/win32_setup/installer.log for details"; \
          exit 1)
else
SUBDIRS+=tools doc
endif

# make run: run the game without installing it. Saves you the hassle
#           to run make install or set the correct environment
#           variables.
run: all
	WARMUX_DATADIR=@srcdir@/data/ WARMUX_FONT_PATH=@srcdir@/data/font/Ubuntu-R.ttf @srcdir@/src/warmux

# make debug: run the game in gdb without installing it.
#             requires gdb to be in your $PATH.
debug: all
	WARMUX_DATADIR=@srcdir@/data/ WARMUX_FONT_PATH=@srcdir@/data/font/Ubuntu-R.ttf gdb @srcdir@/src/warmux
