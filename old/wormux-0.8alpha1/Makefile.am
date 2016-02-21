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
