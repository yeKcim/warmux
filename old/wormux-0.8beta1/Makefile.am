SUBDIRS = data src po doc

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
	WORMUX_DATADIR=@srcdir@/data WORMUX_FONT_PATH=@srcdir@/data/font/DejaVuSans.ttf @srcdir@/src/wormux

# make debug: run the game in gdb without installing it.
#             requires gdb to be in your $PATH.
debug: all
	WORMUX_DATADIR=@srcdir@/data WORMUX_FONT_PATH=@srcdir@/data/font/DejaVuSans.ttf gdb @srcdir@/src/wormux
