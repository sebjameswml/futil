/*!
 * The standard WML debugging scheme. include this file, WmlDbg.h, in
 * your program, make sure it links to libfutil, and early in the
 * main() function, open the DBGSTREAM ofstream. Then, if you have
 * DEBUG defined, then you can use DBG() and debuglog() macros in your
 * code.
 *
 * The standard way to have DEBUG defined is to have a configure.ac
 * stanza in your client module, which #defines DEBUG in
 * mymodule/mymodule/config.h. You then
 *
 * All debug messages are sent to the DBGSTREAM ofstream.
 *
 * wmlDbg is a global and is compiled into futil.
 *
 * Macros are available - DBG, DBG2 and so on. modules can define
 * specialised versions of the macros, DBGOUTPUT and so on, which
 * generate a useful prefix, but all output to the wmlDbg
 * ofstream. Client programs have to open a file into which to send
 * the ofstream data. The name of that file is up to the program to
 * decide.
 *
 * Where we have previously used the cgicc LOGLN() macro we need to
 * move to the DBG() macro.
 *
 * If you want to extend the debugging messages, then, as well as
 * #including WmlDbg.h, #include a new file, moduleDbg.h which adds
 * additional macros something like this:
 *
 * #define DBGNEWFEATURE(s)  DBGSTREAM << "NEWFEATURE: "
 *                                     << __FUNCTION__ << ": "
 *                                     << s << endl;
 */

#ifndef _WMLDBG_H_
#define _WMLDBG_H_

#ifdef DEBUG
# include <fstream>
# ifndef DBGSTREAM
#  define DBGSTREAM wmlDbg
extern std::ofstream DBGSTREAM;
# endif
#endif

/*
 * Macro to open the debug file. To be called early on in the main()
 * function.
 */
#ifdef DEBUG
# define DBGOPEN(s) DBGSTREAM.open (s, std::ios::out|std::ios::trunc);
#else
# define DBGOPEN(s)
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
# define DBG(s)  DBGSTREAM << "DBG: " << __FUNCTION__ << ": " << s << endl;
# define debuglog(type, format, ...) syslog(type, format, __VA_ARGS__)
#else
# define DBG(s)
# define debuglog(type, format, ...)
#endif

#ifdef DEBUG2
# define DBG2(s)  DBGSTREAM << "DBG2: " << __FUNCTION__ << ": " << s << endl;
#else
# define DBG2(s)
#endif

#endif // _WMLDBG_H_
