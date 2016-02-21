SUBDIRS = .

noinst_LIBRARIES = libfixedpoint.a

libfixedpoint_a_SOURCES = \
  fixatantab.h \
  fixasintab.h \
  fixed_class.h \
  fixed_func.cpp \
  fixed_func.h \
  fixsintab.h \
  stdint_fallback.h


AM_CXXFLAGS = -I$(top_srcdir)/lib/fixedpoint           \
	@LIBCURL_CPPFLAGS@
