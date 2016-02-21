SUBDIRS = .

EXTRA_DIST = wormux_index_server.conf

bin_PROGRAMS = wormux-index-server

wormux_index_server_SOURCES =    	\
	client.h client.cpp      	\
	clock.h clock.cpp        	\
	config.h config.cpp      	\
	download.h download.cpp  	\
	main.cpp      			\
	net_data.h net_data.cpp		\
	server.h server.cpp 		\
	stat.h stat.cpp			\
	sync_slave.h sync_slave.cpp

wormux_index_server_LDFLAGS = -Wl,--as-needed

AM_CXXFLAGS = -DPACKAGE_VERSION=\"$(PACKAGE_VERSION)\" \
	  @LIBCURL_CPPFLAGS@ -I$(top_srcdir)/lib/wormux-server/include \
	-I$(top_srcdir)/lib/fixedpoint

LIBS += @LIBCURL@

wormux_index_server_LDADD=$(top_srcdir)/lib/wormux-server/libwormux-server.a
wormux_index_server_DEPENDENCIES=$(top_srcdir)/lib/wormux-server/libwormux-server.a
