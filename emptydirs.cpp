/*
 * This test program is part of WML futil - a library containing
 * static public utility functions and classes used across WML code.
 *
 * This test program finds empty directories from a starting directory.
 *
 *  WML futil is Copyright William Matthew Ltd. 2011.
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

#include <set>
#include <string>
#include <iostream>
#include <fstream>

#include "futil/config.h"
#include "WmlDbg.h"
#include "FoundryUtilities.h"

using namespace std;
using namespace wml;

std::ofstream DBGSTREAM;

int main(int argc, char** argv)
{
        if (argc < 2) {
                cerr << "Usage: emptydirs startDir\n";
                return 0;
        }

        DBGOPEN ("emptydirs.log");

        string basePath(argv[1]);
        set<string> empties;
        FoundryUtilities::readDirectoryEmptyDirs (empties, basePath);

        set<string>::const_iterator i = empties.begin();
        while (i != empties.end()) {
                cout << "Empty dir: " << basePath << "/" << *i << endl;
                ++i;
        }

        DBGCLOSE();
        return 0;
}
