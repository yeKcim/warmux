SUBDIRS = .

bin_PROGRAMS = wormux-list-games

wormux_list_games_SOURCES = \
	main.cpp

wormux_list_games_LDFLAGS = -Wl,--as-needed

AM_CXXFLAGS = -DPACKAGE_VERSION=\"$(PACKAGE_VERSION)\" \
	  @LIBCURL_CPPFLAGS@ -I$(top_srcdir)/lib/wormux-server/include \
	-I$(top_srcdir)/lib/wormux/include \
	-I$(top_srcdir)/lib/fixedpoint

LIBS += @LIBCURL@ @LIBINTL@

wormux_list_games_LDADD=$(top_srcdir)/lib/wormux/libwormux.a
wormux_list_games_DEPENDENCIES=$(top_srcdir)/lib/wormux/libwormux.a
