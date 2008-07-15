dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT([futil], [0.1], [sjames@wmltd.co.uk])
AC_CONFIG_SRCDIR(futil/FoundryUtilities.cpp)
AC_REVISION([$Revision: 1.3 $])
AC_PREFIX_DEFAULT(/usr)

AM_INIT_AUTOMAKE([1.9 foreign])

AM_CONFIG_HEADER(futil/config.h)
AM_MAINTAINER_MODE

AC_LANG_CPLUSPLUS

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_CHECK_PROG(DOXYGEN, doxygen, doxygen, /bin/echo)

dnl Checks for library functions.
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_FUNCS(strftime gettimeofday uname)

dnl Determine host system type
dnl AC_CANONICAL_HOST
AC_DEFINE_UNQUOTED(HOST, "$host", [The host system futil was configured for])

dnl enable/disable compiler warnings (using gcc), if specified
AC_ARG_ENABLE(warnings,
  [  --disable-warnings      disable gcc warnings [default=no]],
  [case "${enableval}" in
    yes) futil_warnings=yes ;; 
    no)  futil_warnings=no ;;
    *) AC_MSG_ERROR(bad value ${enableval} for --enable-warnings) ;;
  esac], futil_warnings=yes)
AC_MSG_CHECKING(whether to use gcc verbose warnings)
AC_MSG_RESULT($futil_warnings)
if (test "$futil_warnings" = yes && test "$GCC" = yes); then
  CXXFLAGS="-Wall $CXXFLAGS"
fi

dnl enable/disable debug logging, if specified
AC_ARG_ENABLE(debug-logging,
  [  --enable-debug-logging  enable debug logging [default=no]],
  [case "${enableval}" in
    yes) futil_debug=yes ;;
    no)  futil_debug=no ;;
    *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug-logging) ;;
  esac], futil_debug=no)
AC_MSG_CHECKING(whether to enable debug logging)
AC_MSG_RESULT($futil_debug)
if test "$futil_debug" = yes; then
  AC_DEFINE(DEBUG, 1, [Define if debug logging is enabled])
fi

dnl Always output certain files
AC_CONFIG_FILES([Makefile futil/Makefile doc/Makefile doc/Doxyfile])

AC_OUTPUT
