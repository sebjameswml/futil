/*
 *  This test program is part of WML futil - a library containing
 *  static public utility functions and classes used across WML code.
 *
 *  This particular program is for comparing the performance of
 *  FoundryUtilities::splitString with
 *  FoundryUtilities::stringToVector.
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

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

#include "futil/config.h"
#include "WmlDbg.h"
#include "FoundryUtilities.h"

// Includes a global called stringtosplit. Also used by stringtovector.cpp.
#include "stringtosplit.h"

using namespace std;
using namespace wml;

std::ofstream DBGSTREAM;

int main(int argc, char** argv)
{
        DBGOPEN ("splitstring.log");

        vector<string> theResult;
        int j = 0;
        while (j < NUM_TIMES_TO_SPLIT) {
                theResult.clear();
                string delim (" ");
                FoundryUtilities::splitString (theResult, stringtosplit, delim);
                ++j;
        }

#ifdef INCLUDE_OUTPUT
        vector<string>::iterator i = theResult.begin();
        while (i != theResult.end()) {
                cout << *i << '\n';
                ++i;
        }
#endif
        DBGCLOSE();

        return 0;
}
