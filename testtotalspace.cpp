#include <string>
#include <iostream>
#include <stdexcept>

#include "config.h"
#include <futil/WmlDbg.h>
#include <futil/FoundryUtilities.h>

extern "C" {
#include <stdlib.h>
}

using namespace std;
using namespace wml;

ofstream DBGSTREAM;

int main (int argc, char** argv)
{
	DBGOPEN ("/tmp/testtotalspace.log");

	try {
		unsigned int total;
		unsigned int available;
		unsigned int used;
		total = FoundryUtilities::totalSpaceKBytes ("/boot");
		available = FoundryUtilities::freeSpaceKBytes ("/boot");
		used = total - available;

		cout << "Total space:        " << total << "\n";
		cout << "Available space:    " << available << "\n";
		cout << "Used space:         " << used << "\n";
	} catch (const runtime_error& e) {
		cout << "Error: " << e.what() << "\n";
	}

	DBGCLOSE();
	_exit(0);
}
