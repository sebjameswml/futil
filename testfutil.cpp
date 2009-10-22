#include <string>
#include <iostream>
#include <fstream>

#include "futil/config.h"
#include "futil/debuglog.h"
#include "futil/FoundryUtilities.h"

using namespace std;
using namespace wml;

int main(int argc, char** argv)
{
	cout << "Running this program at time: "
	     << FoundryUtilities::timeNow() << endl;

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

	return 0;
}
