/*
 * This test program is part of WML futil - a library containing
 * static public utility functions and classes used across WML code.
 *
 *  WML futil is Copyright William Matthew Ltd. 2010.
 *
 *  Authors: Seb James <sjames@wmltd.co.uk>
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
#include <stdexcept>

#include "config.h"
#include "WmlDbg.h"
#include "FoundryUtilities.h"

using namespace std;
using namespace wml;

std::ofstream DBGSTREAM;

int main ()
{
	try {
		cout << "16 characters from a UUID string:\n";
		cout << FoundryUtilities::uuidPortion (16) << endl;

		cout << "\nA string containing upper, lower case and numerals:\n";
		cout << FoundryUtilities::randomString (64) << endl;
		cout << "\nA string containing lower case and numerals:\n";
		cout << FoundryUtilities::randomString (64, false, true, true) << endl;
		cout << "\nA string containing upper case and numerals:\n";
		cout << FoundryUtilities::randomString (64, true, false, true) << endl;
		cout << "\nA string containing only numerals:\n";
		cout << FoundryUtilities::randomString (64, false, false, true) << endl;
		cout << "\nA string containing only upper case:\n";
		cout << FoundryUtilities::randomString (64, true, false, false) << endl;
		cout << "\nA string containing only lower case:\n";
		cout << FoundryUtilities::randomString (64, false, true, false) << endl;

		cout << "\nCause an exception...\n";
		cout << FoundryUtilities::randomString (64, false, false, false) << endl;

	} catch (const exception& e) {
		cout << "Exception: " << e.what() << endl;
	}

	return 0;
}
