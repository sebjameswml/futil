/*
 * This test program is part of WML futil - a library containing
 * static public utility functions and classes used across WML code.
 *
 *  WML futil is Copyright William Matthew Ltd. 2012.
 *
 *  Authors: Seb James <sjames@wmltd.co.uk>
 *           Tamora James <tjames@wmltd.co.uk>
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

#include "config.h"
#include "WmlDbg.h"
#include <iostream>
#include <string>
#include <vector>

std::ofstream DBGSTREAM;

#include "FoundryUtilities.h"

using namespace std;
using namespace wml;

int main (int argc, char **argv)
{
        DBGOPEN ("./testValidMac.log");

        /*
         * Tests
         */

        list<pair<string, bool> > tests;

        tests.push_back(make_pair(
                               "01:23:45:67:89:ab",
                               true));

        tests.push_back(make_pair(
                               "00:00:00:00:00:00",
                               true));

        tests.push_back(make_pair(
                               "RT:01:45:f0:EF:67",
                               false));

        tests.push_back(make_pair(
                               "01:23:45:67:89:a",
                               false));

        unsigned int passed = 0;
        unsigned int failed = 0;

        cout << "----- testValidMac -----" << endl;

        list<pair<string, bool> >::const_iterator iTest = tests.begin();
        while (iTest != tests.end()) {
                bool out (FoundryUtilities::valid_mac (iTest->first));
                if (out == iTest->second) {
                        ++passed;
                } else {
                        ++failed;
                        cerr << "Test " << passed + failed << " failed!\n"
                             << "\tInput:\t\t \"" << iTest->first << "\"\n"
                             << "\tOutput:\t\t \"" << out << "\"\n"
                             << "\tExpected:\t \"" << iTest->second << "\"" << endl;
                }
                ++iTest;
        }
        cout << passed << " tests passed, " << failed << " tests failed." << endl;

        DBGCLOSE();
        return failed ? 1 : 0;
}
