
#ifndef _DEBUGLOG_H_
#define _DEBUGLOG_H_

#ifndef DEBUG
#define DEBUG 0
#endif
#ifndef DEBUG2
#define DEBUG2 0
#endif


#if DEBUG
# if DEBUG2
#  define debugf(format, ...) printf(format, __VA_ARGS__)
#  define debuglog2(type, format, ...) syslog(type, format, __VA_ARGS__)
# else
#  define debugf(format, ...)
#  define debuglog2(type, format, ...)
# endif
# define debuglog(type, format, ...) syslog(type, format, __VA_ARGS__)
#else
# define debugf(format, ...)
# define debuglog(type, format, ...)
# define debuglog2(type, format, ...)
#endif

#endif // _DEBUGLOG_H_

