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

	cout << "Load av: " << FoundryUtilities::getLoadAverage() << '\n';

	UINT64_TYPE n;
	string b52("");
	n = (UINT64_TYPE)181440;
	b52 = FoundryUtilities::formatUint64InBaseN (n, 52);
	cout << "181440 (21 days of wmllicence eval) is " << b52 << '\n';

	n = (UINT64_TYPE)267840;
	b52 = FoundryUtilities::formatUint64InBaseN (n, 52);
	cout << "267840 (31 days of wmllicence eval) is " << b52 << '\n';

	string into("");
	ifstream fromS;
	fromS.open ("testFile");
	FoundryUtilities::copyFileToString (fromS, into);
	cout << "into is:\n" << into;

	DBGCLOSE();

	return 0;
}
