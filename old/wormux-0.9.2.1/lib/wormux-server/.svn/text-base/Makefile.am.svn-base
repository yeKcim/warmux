SUBDIRS = .

noinst_LIBRARIES = libwormux-server.a

libwormux_server_a_SOURCES = \
	./include/WSERVER_config.h \
	./include/WSERVER_clock.h \
	./include/WSERVER_env.h \
	./include/WSERVER_debug.h \
	./include/WSERVER_index_msg.h \
	./src/clock.cpp \
	./src/config.cpp \
	./src/env.cpp

AM_CXXFLAGS = -I$(top_srcdir)/lib/wormux-server/include    \
	-I$(top_srcdir)/lib/fixedpoint