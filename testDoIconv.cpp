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
        string inputString("A SIMPLE ascii string this is. A simple ascii string this is. A simple ascii string this is. A simple ascii string this is. A simple ascii string this is.");
        string outputString("");

        try {
                futil::doIconv ("ISO-8859-1", "UTF-8", inputString, outputString);
        } catch (const exception& e) {
                cerr << "Exception in futil::doIconv(): " << e.what() << endl;
                return -1;
        }

        cerr << "Transcoded: '" << outputString << "'\n";



        string inputString2("It's pounds: £.");
        string outputString2("");

        try {
                futil::doIconv ("ISO-8859-1", "UTF-8", inputString2, outputString2);
        } catch (const exception& e) {
                cerr << "Exception in futil::doIconv(): " << e.what() << endl;
                return -1;
        }

        cerr << "Transcoded: '" << outputString2 << "'\n";

        string::size_type i = 0;
        while (i<outputString2.size()) {
                cerr << "char " << i << " is " << (int)outputString2[i] << "(" << (char)outputString2[i] << ")" << endl;
                i++;
        }

        cout << outputString2;

        return 0;
}
