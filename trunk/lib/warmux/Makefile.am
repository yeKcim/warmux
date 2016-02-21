SUBDIRS = .

noinst_LIBRARIES = libwarmux.a

libwarmux_a_SOURCES = \
        ./include/config_android.h \
        ./include/config_symbian.h \
        ./include/config_visual.h \
        ./include/WARMUX_config.h \
	./include/WARMUX_action.h \
	./include/WARMUX_action_handler.h \
        ./include/WARMUX_base.h \
	./include/WARMUX_debug.h \
	./include/WARMUX_distant_cpu.h \
	./include/WARMUX_download.h \
	./include/WARMUX_error.h \
	./include/WARMUX_euler_vector.h \
	./include/WARMUX_file_tools.h \
	./include/WARMUX_i18n.h \
	./include/WARMUX_index_server.h \
	./include/WARMUX_index_svr_msg.h \
	./include/WARMUX_network.h \
	./include/WARMUX_player.h \
	./include/WARMUX_point.h \
	./include/WARMUX_random.h \
	./include/WARMUX_rectangle.h \
	./include/WARMUX_singleton.h \
	./include/WARMUX_socket.h \
	./include/WARMUX_team_config.h \
	./include/WARMUX_types.h \
	./include/WARMUX_vector2.h \
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

AM_CXXFLAGS = -I$(top_srcdir)/lib/warmux/include \
	-I$(top_srcdir)/lib/fixedpoint           \
	@LIBCURL_CPPFLAGS@
