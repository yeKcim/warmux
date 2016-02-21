SUBDIRS = .

EXTRA_DIST = warmux_server.conf

bin_PROGRAMS = warmux-server

warmux_server_SOURCES = \
	config.h config.cpp \
	server.h server.cpp \
	main.cpp

warmux_server_LDFLAGS = -Wl,--as-needed

AM_CXXFLAGS = -DPACKAGE_VERSION=\"$(PACKAGE_VERSION)\" \
	  @LIBCURL_CPPFLAGS@ -I$(top_srcdir)/lib/warmux-server/include \
	-I$(top_srcdir)/lib/warmux/include \
	-I$(top_srcdir)/lib/fixedpoint

LIBS += @LIBCURL@

warmux_server_LDADD=$(top_srcdir)/lib/warmux-server/libwarmux-server.a $(top_srcdir)/lib/warmux/libwarmux.a
