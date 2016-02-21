SUBDIRS = .

EXTRA_DIST = wormux_server.conf

bin_PROGRAMS = wormux-server

wormux_server_SOURCES = \
	config.h config.cpp \
	server.h server.cpp \
	main.cpp

wormux_server_LDFLAGS = -Wl,--as-needed

AM_CXXFLAGS = -DPACKAGE_VERSION=\"$(PACKAGE_VERSION)\" \
	  @LIBCURL_CPPFLAGS@ -I$(top_srcdir)/lib/wormux-server/include \
	-I$(top_srcdir)/lib/wormux/include \
	-I$(top_srcdir)/lib/fixedpoint

LIBS += @LIBCURL@

wormux_server_LDADD=$(top_srcdir)/lib/wormux-server/libwormux-server.a $(top_srcdir)/lib/wormux/libwormux.a
wormux_server_DEPENDENCIES=$(top_srcdir)/lib/wormux-server/libwormux-server.a $(top_srcdir)/lib/wormux/libwormux.a
