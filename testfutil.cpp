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

	DBGCLOSE();

	return 0;
}
