#include <iostream>
#include <fstream>

#include "config.h"
#include "WmlDbg.h"
#include "FoundryUtilities.h"

using namespace std;
using namespace wml;

std::ofstream DBGSTREAM;

int main ()
{
	FoundryUtilities::setPermissions ("/etc/wml/file.file", 0664);
	FoundryUtilities::setOwnership ("/etc/wml/file.file", 17833, 528);

	return 0;
}
