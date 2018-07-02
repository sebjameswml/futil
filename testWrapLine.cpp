/*
 * This test program is part of WML futil - a library containing
 * static public utility functions and classes used across WML code.
 *
 *  WML futil is Copyright William Matthew Ltd. 2012.
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

#include "config.h"
#include "WmlDbg.h"
#include <iostream>
#include <string>
#include <vector>

std::ofstream DBGSTREAM;

#include "futil.h"

using namespace std;
using namespace wml;

int main (int argc, char **argv)
{
        DBGOPEN ("./testWrapLine.log");

        /*
         * Tests
         */

        list<pair<string, string> > tests;
        unsigned int testLineLength (96);

        tests.push_back(make_pair(
                               "[mediatype=MediaType,mediaposition=MediaPosition (Tray number),plex=duplex|simplex|unset*,tumble=yes|no|unset*,targetpage=N,replaceexisting=yes/no*]",
                               "[mediatype=MediaType,mediaposition=MediaPosition (Tray number),plex=duplex|simplex|unset*,<br/>\ntumble=yes|no|unset*,targetpage=N,replaceexisting=yes/no*]"));

        tests.push_back(make_pair(
                                "tq2/2013-04-23_16-37-04_2013_04_15_11_42_54_2013-04-23_16-37-04_2013_04_15_11_42_54_Held_document_A14789788_pdf.pdf",
                                "tq2/2013-04-23_16-37-04_2013_04_15_11_42_54_2013-04-23_16-37-04_2013_04_15_11_42_54_Held_documen\\<br/>\nt_A14789788_pdf.pdf"));

        unsigned int passed = 0;
        unsigned int failed = 0;

        cout << "----- testWrapLine -----" << endl;

        list<pair<string, string> >::const_iterator iTest = tests.begin();
        while (iTest != tests.end()) {
                vector<string> out (futil::wrapLine (iTest->first, testLineLength, ','));

                stringstream ssOut;
                vector<string>::const_iterator iOut (out.begin()), end (out.end());
                bool first (true);
                while (iOut != end) {
                        if (first) {
                                first = false;
                        } else {
                                ssOut << "<br/>\n";
                        }
                        ssOut << *iOut;
                        ++iOut;
                }

                if (ssOut.str() == iTest->second) {
                        ++passed;
                } else {
                        ++failed;
                        cerr << "Test " << passed + failed << " failed!\n"
                             << "\tInput:\t\t \"" << iTest->first << "\"\n"
                             << "\tOutput:\t\t \"" << ssOut.str() << "\"\n"
                             << "\tExpected:\t \"" << iTest->second << "\"" << endl;
                }
                ++iTest;
        }
        cout << passed << " tests passed, " << failed << " tests failed." << endl;

        DBGCLOSE();
        return failed ? 1 : 0;
}
