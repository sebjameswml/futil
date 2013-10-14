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

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <list>
#include <vector>

#include "config.h"
#include "WmlDbg.h"
#include "futil.h"
#include "RC4.h"

using namespace std;
using namespace wml;

ofstream DBGSTREAM;

int main (int argc, char** argv)
{
        DBGOPEN("testRC4.log");

        /*
         * Tests
         */

        list<pair<pair<string, string>, string> > tests;

        tests.push_back(make_pair(make_pair ("Key", "Plaintext"),
                                  "\xBB\xF3\x16\xE8\xD9\x40\xAF\x0A\xD3"));

        tests.push_back(make_pair(make_pair ("Wiki", "pedia"),
                                  "\x10\x21\xBF\x04\x20"));

        tests.push_back(make_pair(make_pair ("Secret", "Attack at dawn"),
                                  "\x45\xA0\x1F\x64\x5F\xC3\x5B\x38\x35\x52\x54\x4B\x9B\xF5"));

        unsigned int passed = 0;
        unsigned int failed = 0;

        cout << "----- testRC4 -----" << endl;

        string expected ("");
        list<pair<pair<string, string>, string> >::const_iterator iTest = tests.begin();
        while (iTest != tests.end()) {
                string out (RC4::rc4crypt (iTest->first.first, iTest->first.second));
                expected = iTest->second;
                futil::convertCHexCharSequences (expected);
                if (out == expected) {
                        ++passed;
                } else {
                        ++failed;
                        cerr << "Test " << passed + failed << " failed!\n"
                             << "\tInput:\t\t \"" << iTest->first.first << ", "
                                                  << iTest->first.second << "\"\n"
                             << "\tOutput:\t\t \"" << out << "\"\n"
                             << "\tExpected:\t \"" << iTest->second << "\"" << endl;
                }

                string out2 (RC4::rc4crypt (iTest->first.first, out));
                if (out2 == iTest->first.second) {
                        ++passed;
                } else {
                        ++failed;
                        cerr << "Test " << passed + failed << " failed!\n"
                             << "\tInput:\t\t \"" << iTest->first.first << ", "
                                                  << out << "\"\n"
                             << "\tOutput:\t\t \"" << out2 << "\"\n"
                             << "\tExpected:\t \"" << iTest->first.second << "\"" << endl;
                }

                ++iTest;
        }
        cout << passed << " tests passed, " << failed << " tests failed." << endl;

        DBGCLOSE();
        return 0;
}
