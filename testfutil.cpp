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
#include <stdlib.h>

#include "futil/config.h"
#include "WmlDbg.h"
#include "futil.h"

using namespace std;
using namespace wml;

std::ofstream DBGSTREAM;

int main(int argc, char** argv)
{
        DBGOPEN ("testfutil.log");

        cout << "Running this program at time: "
             << futil::timeNow() << endl;

        futil::createDir ("/tmp/seb/another");

        string s ("\\\\\\\\"); // Should be "\\\\" - 4 backslashes
        cout << "s at beginning is         '" << s << "'\n";
        futil::searchReplace ("\\\\", "\\", s, true);
        cout << "After searchReplace, s is '" << s << "'\n";

        s  = "This has  a \\\\ in it.";
        cout << "s at beginning is         '" << s << "'\n";
        futil::searchReplace ("\\\\", "\\", s, true);
        cout << "After searchReplace, s is '" << s << "'\n";

        s  = "\\\\ search text at start.";
        cout << "s at beginning is         '" << s << "'\n";
        futil::searchReplace ("\\\\", "\\", s, true);
        cout << "After searchReplace, s is '" << s << "'\n";

        s = "abc._-d&";
        cout << "Original string: '"<<s<<"'\n";
        futil::sanitize (s, CHARS_NUMERIC_ALPHA"_", true);
        cout << "After sanitize, s is '" << s << "'\n";

        s = "abc._-d&";
        futil::sanitizeReplace (s, CHARS_NUMERIC_ALPHA"_", '_');
        cout << "After sanitizeReplace, s is '" << s << "'\n";

        cout << "Load av: " << futil::getLoadAverage() << '\n';

        string into("");
        ifstream fromS;
        fromS.open ("testFile");
        if (fromS.is_open()) {
                futil::copyFileToString (fromS, into);
                cout << "into is:\n" << into;
        }

        system ("echo \"contents\" > /tmp/B108642.pdf");

        string fromFile ("/tmp/B108642.pdf");
        string toPath ("/tmp/new.pdf");
        try {
                stringstream ff;
                futil::copyFile (fromFile, ff);
                futil::copyFile (ff, toPath);
        } catch (const exception& e) {
                cout << "Exception: " << e.what() << endl;
        }

        string uPath ("wmloutbatch:///tmp/outbatch/hp9300/");
        futil::stripUnixFile (uPath);
        cout << "Just the path is '" << uPath << "'\n";

        uPath = "wmloutbatch:///tmp/outbatch/hp9300/";
        futil::stripUnixPath (uPath);
        cout << "Just the file is '" << uPath << "'\n";

        ofstream appendTo;
        appendTo.open (toPath.c_str(), ios::out|ios::app|ios::ate);
        if (appendTo.is_open()) {
                // /tmp/new.pdf should now have /tmp/B108642.pdf twice.
                futil::appendFile (fromFile, appendTo);
                appendTo.close();
        }

        cout << "Free space: " << futil::freeSpaceFraction ("/tmp") << endl;

        string withHexEscapes("\\x41\\x42\\x43");
        cout << "Unconverted string: " << withHexEscapes << endl;
        futil::convertCHexCharSequences (withHexEscapes);
        cout << "Converted string: " << withHexEscapes << endl;

        // When testing, change this number to something which will work:
        cout << futil::filesOpen (0) << " files are open for this process\n";

        DBGCLOSE();

        return 0;
}
