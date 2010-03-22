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

/*
 * A program to convert a date into an epoch number
 */

int main(int argc, char** argv)
{
        DBGOPEN ("dateToEpoch.log");

	if (argc < 2) {
		cerr << "Usage: dateToEpoch datestr\n";
		return -1;
	}

	string d (argv[1]);

	time_t n = 0;

	try {
	n = FoundryUtilities::dateTimeToNum (d);
	cout << n;
	} catch (const exception& e) {
		cerr << "Date format error\n";
		return -1;
	}

	DBGCLOSE();

	return 0;
}
