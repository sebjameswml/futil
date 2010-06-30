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
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WML futil is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WML futil.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "futil/config.h"
#include "WmlDbg.h"
#include "FoundryUtilities.h"

using namespace std;
using namespace wml;

std::ofstream DBGSTREAM;

/*
 * A program to convert a date into an epoch number
 */

int main(int argc, char** argv)
{
        DBGOPEN ("dateToEpoch.log");

	string d;
	cin >> d;

	if (d.empty() && argc < 2) {
		cerr << "Usage: dateToEpoch datestr or echo datestr | dateToEpoch\n";
		return -1;
	}

	if (d.empty()) {
		d = argv[1];
	}

	time_t n = 0;

	try {
	n = FoundryUtilities::dateTimeToNum (d);
	cout << n;
	} catch (const exception& e) {
		cerr << "Date format error\n";
		return -1;
	}

	DBGCLOSE();

	return 0;
}
