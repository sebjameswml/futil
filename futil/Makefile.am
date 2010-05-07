CLEANFILES = *~

INCLUDES = -I$(top_srcdir) -I$(top_builddir)

lib_LTLIBRARIES = libfutil.la

libfutil_la_SOURCES = FoundryUtilities.cpp Process.cpp FileTypeIdentifier.cpp

libfutil_la_LDFLAGS = -version-info 2:0:0

pkginclude_HEADERS = FoundryUtilities.h Process.h WmlDbg.h FileTypeIdentifier.h
