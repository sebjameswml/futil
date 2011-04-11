/* -*-c++-*- */
/*!
 * A solution to obtaining POSIX filenos in C++ programs. Public
 * domain code written by Richard B. Krekel with thanks.
 */

#ifndef _FILENO_H_
#define _FILENO_H_

#include <iosfwd>

template <typename charT, typename traits>
int fileno(const std::basic_ios<charT, traits>& stream);

#endif // _FILENO_H_
