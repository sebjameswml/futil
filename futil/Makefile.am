CLEANFILES = *~

INCLUDES = -I$(top_srcdir) -I$(top_builddir) $(GLIBMM_CFLAGS) $(UUID_CFLAGS)

lib_LTLIBRARIES = libfutil.la

libfutil_la_SOURCES = futil.cpp Process.cpp FileTyper.cpp fileno.cpp RC4.cpp

libfutil_la_LDFLAGS = -version-info 2:0:0

pkginclude_HEADERS = futil.h Process.h WmlDbg.h FileTyper.h fileno.h RC4.h
