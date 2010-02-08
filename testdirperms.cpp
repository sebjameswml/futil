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
        DBGOPEN ("testdirperms.log");

	// Suppose /etc/inetd.conf is a hanging symlink?
	FoundryUtilities::createDir ("/etc/wml/pp/blah", 0775, 0, 7);
	
	DBGCLOSE();

	return 0;
}
