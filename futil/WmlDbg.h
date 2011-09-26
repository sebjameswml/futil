/* -*-c++-*- */
/*
 * This file is part of WML futil - a library containing static public
 * utility functions and classes used across WML code.
 *
 *  WML futil is Copyright William Matthew Ltd. 2010.
 *
 *  Authors: Seb James <sjames@wmltd.co.uk>
 *           Tamora James <tjames@wmltd.co.uk>
 *           Mark Richardson <mrichardson@wmltd.co.uk>
 *
 *  WML futil is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WML futil is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with WML futil.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*!
 * \file WmlDbg.h
 *
 * \brief The standard WML debugging scheme.
 *
 * Instructions:
 *
 * Include this file, WmlDbg.h, in
 * each .cpp file in your module. Then, if you have DEBUG defined,
 * then you can use DBG() and debuglog() macros in your code.
 *
 * To actually get the DBG messages into a file, any executables built
 * must be linked to libfutil and include these lines:
 *\code
 #include <futil/WmlDbg.h>
 std::ofstream DBGSTREAM;
 \endcode
 *
 * Then, early in the main() function, open
 * the DBGSTREAM ofstream like this:
 *\code
 DBGOPEN ("/tmp/debug.log");
 \endcode
 *
 * The standard way to have DEBUG defined is to have a configure.ac
 * stanza in your client module, which defines DEBUG in
 * mymodule/mymodule/config.h:
 *\verbatim
 dnl enable/disable debug logging, if specified
 AC_ARG_ENABLE(debug-logging,
 [  --enable-debug-logging  enable debug logging for mymodule [default=no]],
 [case "${enableval}" in
 yes) mymodule_debug=yes ;;
 no)  mymodule_debug=no ;;
 *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug-logging) ;;
 esac], mymodule_debug=no)
 AC_MSG_CHECKING(whether to enable debug logging)
 AC_MSG_RESULT($mymodule_debug)
 if test "$mymodule_debug" = yes; then
 AC_DEFINE(DEBUG, 1, [Define if debug logging is enabled])
 fi
 \endverbatim
 *
 * You then call ./configure with
 * --enable-debug-logging if you want DEBUG defined.
 *
 * Note that futil DOESN'T itself need to be compiled with
 * --enable-debug-logging for some other module to generate debug
 * logging using futil/WmlDbg.h.
 *
 * How it works:
 *
 * All debug messages are sent to the DBGSTREAM ofstream. The client
 * code should open this ofstream so that debug messages end up in a
 * file, or sent to stderr, as required.
 *
 * The DBGSTREAM ofstream needs to be declared globally in the same
 * file as the main() function for the executable.
 *
 * Macros are available to open the debug ofstream (DBGOPEN) and to
 * close it (DBGCLOSE). DBG and DBG2 are macros to actually emit a
 * debug message. The argument to DBG() can be used like a stream, so
 * this works:
 *\code
 int i = 1;
 DBG ("The integer is " << i);
 \endcode
 *
 * NB: Where we have previously used the cgicc LOGLN() macro (in our
 * webui stuff) we need to move to the DBG() macro.
 *
 * Extending:
 *
 * If you want to extend the debugging messages, then, as well as
 * including WmlDbg.h, include a new file, moduleDbg.h which adds
 * additional macros something like this:
 *\code
 #define DBGNEWFEATURE(s)  DBGSTREAM << "NEWFEATURE: "
 << __FUNCTION__ << ": "
 << s << endl;
 \endcode
 *
 */

#ifndef _WMLDBG_H_
#define _WMLDBG_H_

#ifndef DBGSTREAM
# define DBGSTREAM wmlDbg
# include <fstream>
extern std::ofstream DBGSTREAM;
#endif

/*
 * Macro to open the debug file. To be called early on in the main()
 * function.
 */
#ifdef DEBUG
# define DBGOPEN(s) DBGSTREAM.open (s, std::ios::out|std::ios::trunc);  \
        DBGSTREAM << "**WML Debug File**\n";
#else
# define DBGOPEN(s)
#endif

/*
 * Macro to open the debug file, but without first truncating it. To
 * be called early on in the main() function.
 */
#ifdef DEBUG
# define DBGAPPEND(s) DBGSTREAM.open (s, std::ios::out|std::ios::app);  \
        DBGSTREAM << "**WML Debug File**\n";
#else
# define DBGAPPEND(s)
#endif

/*
 * Macro to close the debug file. To be called late in the main()
 * function, just before it exits.
 */
#ifdef DEBUG
# define DBGCLOSE() if (DBGSTREAM.is_open()) { DBGSTREAM.close(); }
#else
# define DBGCLOSE()
#endif

#ifdef DEBUG
# define DBG(s)  DBGSTREAM << "DBG: " << __FUNCTION__ << ": " << s << std::endl;
# define debuglog(type, format, ...) syslog(type, format, __VA_ARGS__)
#else
# define DBG(s)
# define debuglog(type, format, ...)
#endif

#ifdef DEBUG2
# define DBG2(s)  DBGSTREAM << "DBG2: " << __FUNCTION__ << ": " << s << std::endl;
# define debuglog2(type, format, ...) syslog(type, format, __VA_ARGS__)
#else
# define DBG2(s)
# define debuglog2(type, format, ...)
#endif

#endif // _WMLDBG_H_
