/*
 *  This test program is part of WML futil - a library containing
 *  static public utility functions and classes used across WML code.
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

#include "futil/config.h"
#include "WmlDbg.h"
#include "FoundryUtilities.h"

using namespace std;
using namespace wml;

std::ofstream DBGSTREAM;

int main(int argc, char** argv)
{
        DBGOPEN ("testfutil.log");

	cout << "Running this program at time: "
	     << FoundryUtilities::timeNow() << endl;

	FoundryUtilities::createDir ("/tmp/seb/another");

	string s ("\\\\\\\\"); // Should be "\\\\" - 4 backslashes
	cout << "s at beginning is         '" << s << "'\n";
	FoundryUtilities::searchReplace ("\\\\", "\\", s, true);
	cout << "After searchReplace, s is '" << s << "'\n";

	s  = "This has  a \\\\ in it.";
	cout << "s at beginning is         '" << s << "'\n";
	FoundryUtilities::searchReplace ("\\\\", "\\", s, true);
	cout << "After searchReplace, s is '" << s << "'\n";

	s  = "\\\\ search text at start.";
	cout << "s at beginning is         '" << s << "'\n";
	FoundryUtilities::searchReplace ("\\\\", "\\", s, true);
	cout << "After searchReplace, s is '" << s << "'\n";

	s = "abc._-d&";
	cout << "Original string: '"<<s<<"'\n";
	FoundryUtilities::sanitize (s, CHARS_NUMERIC_ALPHA"_", true);
	cout << "After sanitize, s is '" << s << "'\n";

	s = "abc._-d&";
	FoundryUtilities::sanitizeReplace (s, CHARS_NUMERIC_ALPHA"_", '_');
	cout << "After sanitizeReplace, s is '" << s << "'\n";

	cout << "Load av: " << FoundryUtilities::getLoadAverage() << '\n';

	string into("");
	ifstream fromS;
	fromS.open ("testFile");
	FoundryUtilities::copyFileToString (fromS, into);
	cout << "into is:\n" << into;

	string fromFile ("/tmp/B108642.pdf");
	stringstream ff;
	FoundryUtilities::copyFile (fromFile, ff);
	string toPath ("/tmp/new.pdf");
	FoundryUtilities::copyFile (ff, toPath);

	DBGCLOSE();

	return 0;
}
