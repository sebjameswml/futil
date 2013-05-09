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
#include <sstream>

#include "config.h"
#include "WmlDbg.h"
#include "FoundryUtilities.h"

using namespace std;
using namespace wml;

std::ofstream DBGSTREAM;

int main ()
{

        string test("A J character is: &#x4a;"); // should be 'J'
        cout << "Before: " << test << endl;
        FoundryUtilities::numericCharRefsToUTF8 (test);
        cout << "After: " << test << endl;

        // 2 bytes
        test = "The price is &#x61;100";
        cout << "Before: " << test << endl;
        FoundryUtilities::numericCharRefsToUTF8 (test);
        cout << "After: " << test << endl;

        // 2 bytes
        test = "A char is &#xA2;";
        cout << "Before: " << test << endl;
        FoundryUtilities::numericCharRefsToUTF8 (test);
        cout << "After: " << test << endl;

        // 3 bytes
        test = "A 3 byte utf8 char is &#x20AC;";
        cout << "Before: " << test << endl;
        FoundryUtilities::numericCharRefsToUTF8 (test);
        cout << "After: " << test << endl;

        // 4 bytes
        test = "A 4 byte utf8 char is &#x24b62;";
        cout << "Before: " << test << endl;
        FoundryUtilities::numericCharRefsToUTF8 (test);
        cout << "After: " << test << endl;


        return 0;
}
