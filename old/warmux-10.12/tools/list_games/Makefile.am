SUBDIRS = .

bin_PROGRAMS = warmux-list-games

warmux_list_games_SOURCES = \
	main.cpp

warmux_list_games_LDFLAGS = -Wl,--as-needed

AM_CXXFLAGS = -DPACKAGE_VERSION=\"$(PACKAGE_VERSION)\" \
	  @LIBCURL_CPPFLAGS@ -I$(top_srcdir)/lib/warmux-server/include \
	-I$(top_srcdir)/lib/warmux/include \
	-I$(top_srcdir)/lib/fixedpoint

LIBS += @LIBCURL@ @LIBINTL@

warmux_list_games_LDADD=$(top_srcdir)/lib/warmux/libwarmux.a
warmux_list_games_DEPENDENCIES=$(top_srcdir)/lib/warmux/libwarmux.a
