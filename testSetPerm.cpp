/*
 * This test program is part of WML futil - a library containing
 * static public utility functions and classes used across WML code.
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

#include <iostream>
#include <fstream>

#include "config.h"
#include "WmlDbg.h"
#include "futil.h"

using namespace std;
using namespace wml;

std::ofstream DBGSTREAM;

int main ()
{
        futil::setPermissions ("/etc/wml/file.file", 0664);
        futil::setOwnership ("/etc/wml/file.file", 17833, 528);

        return 0;
}
