SUBDIRS = .

noinst_LIBRARIES = libwormux.a

libwormux_a_SOURCES = \
	./include/WORMUX_action.h \
	./include/WORMUX_action_handler.h \
	./include/WORMUX_debug.h \
	./include/WORMUX_distant_cpu.h \
	./include/WORMUX_download.h \
	./include/WORMUX_error.h \
	./include/WORMUX_euler_vector.h \
	./include/WORMUX_file_tools.h \
	./include/WORMUX_i18n.h \
	./include/WORMUX_index_server.h \
	./include/WORMUX_index_svr_msg.h \
	./include/WORMUX_network.h \
	./include/WORMUX_player.h \
	./include/WORMUX_point.h \
	./include/WORMUX_random.h \
	./include/WORMUX_rectangle.h \
	./include/WORMUX_singleton.h \
	./include/WORMUX_socket.h \
	./include/WORMUX_team_config.h \
	./include/WORMUX_types.h \
	./include/WORMUX_vector2.h \
	./action/action.cpp \
	./action/action_handler.cpp \
	./base/error.cpp \
	./base/i18n.cpp \
	./base/singleton.cpp \
	./maths/euler_vector.cpp \
	./net/distant_cpu.cpp \
	./net/download.cpp \
	./net/index_server.cpp \
	./net/network.cpp \
	./net/player.cpp \
	./net/socket.cpp \
	./net/extSDL_net.cpp \
	./net/extSDL_net.h \
	./tools/debug.cpp \
	./tools/file_tools.cpp \
	./tools/random.cpp

AM_CXXFLAGS = -I$(top_srcdir)/lib/wormux/include \
	-I$(top_srcdir)/lib/fixedpoint           \
	@LIBCURL_CPPFLAGS@
