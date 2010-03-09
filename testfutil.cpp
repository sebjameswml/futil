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
