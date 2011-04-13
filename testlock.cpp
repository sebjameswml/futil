/*
 * This test program is part of WML futil - a library containing
 * static public utility functions and classes used across WML code.
 *
 *  WML futil is Copyright William Matthew Ltd. 2010.
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
        DBGOPEN ("testlock.log");

	string fpath ("/tmp/my_file");
	fstream f;
	if (!FoundryUtilities::fileExists (fpath)) {
		FoundryUtilities::touchFile (fpath);
	}

	FoundryUtilities::openFilestreamForOverwrite (f, fpath);
	if (f.is_open()) {
		cout << "Calling getLock (f)" << endl;
		FoundryUtilities::getLock (f);
		cout << "Sleeping" << endl;
		sleep (5);
		cout << "Releasing lock" << endl;
		FoundryUtilities::releaseLock (f);

		FoundryUtilities::closeFilestream (f);
	} else {
		cout << "Couldn't open file '" << fpath << "'" << endl;
	}

	DBGCLOSE();

	return 0;
}