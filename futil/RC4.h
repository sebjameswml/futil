/* -*-c++-*- */
/*
 * \file RC4.h
 *
 * \brief A class for performing RC4 encryption/decryption.
 *
 *  WML futil is Copyright William Matthew Ltd. 2012.
 *
 *  Author: Tamora James <tjames@wmltd.co.uk>
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

#ifndef _RC4_H_
#define _RC4_H_

#include <string>

using namespace std;

namespace wml {

        class RC4
        {
        public:

                /*!
                 * \brief RC4 encrypt/decrypt the input string using
                 * the specified key.
                 */
                static std::string rc4crypt (const std::string& key, const std::string& input);
        };

} // namespace wml

#endif // _RC4_H_
