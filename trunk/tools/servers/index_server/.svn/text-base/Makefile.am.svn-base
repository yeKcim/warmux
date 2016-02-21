SUBDIRS = .

EXTRA_DIST = warmux_index_server.conf

bin_PROGRAMS = warmux-index-server

warmux_index_server_SOURCES =    	\
	client.h client.cpp      	\
	clock.h clock.cpp        	\
	config.h config.cpp      	\
	download.h download.cpp  	\
	main.cpp      			\
	net_data.h net_data.cpp		\
	server.h server.cpp 		\
	stat.h stat.cpp			\
	sync_slave.h sync_slave.cpp

warmux_index_server_LDFLAGS = -Wl,--as-needed

AM_CXXFLAGS = -DPACKAGE_VERSION=\"$(PACKAGE_VERSION)\" \
	  @LIBCURL_CPPFLAGS@ -I$(top_srcdir)/lib/warmux-server/include \
	-I$(top_srcdir)/lib/fixedpoint

LIBS += @LIBCURL@

warmux_index_server_LDADD=$(top_srcdir)/lib/warmux-server/libwarmux-server.a
warmux_index_server_DEPENDENCIES=$(top_srcdir)/lib/warmux-server/libwarmux-server.a
