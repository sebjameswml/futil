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
        DBGOPEN ("testsymlinks.log");

	// Suppose /etc/inetd.conf is a hanging symlink?
	if (FoundryUtilities::fileExists ("/etc/inetd.conf")) {
		cout << "true" << endl;
	} else {
		cout << "false" << endl;
	}
	// Answer: If it's a hanging symlink, fileExists returns
	// false, if it's NOT a hanging symlink, fileExists returns
	// true.

	DBGCLOSE();

	return 0;
}
