SUBDIRS = .

noinst_LIBRARIES = libwarmux-server.a

libwarmux_server_a_SOURCES = \
	./include/WSERVER_config.h \
	./include/WSERVER_clock.h \
	./include/WSERVER_env.h \
	./include/WSERVER_debug.h \
	./include/WSERVER_index_msg.h \
	./include/WSERVER_logfile.h \
	./src/clock.cpp \
	./src/config.cpp \
	./src/env.cpp \
	./src/logfile.cpp

AM_CXXFLAGS = -I$(top_srcdir)/lib/warmux-server/include    \
	-I$(top_srcdir)/lib/fixedpoint