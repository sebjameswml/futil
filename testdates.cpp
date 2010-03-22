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
        DBGOPEN ("testdates.log");

	cout << "Running this program at time: "
	     << FoundryUtilities::timeNow() << endl;

	string d("2009-01-21 23:59:55");
	time_t n = FoundryUtilities::dateTimeToNum (d);
	cout << d << " as unix epoch is: " << n << endl;

	cout << "Converted back to datestr: "
	     << FoundryUtilities::numToDateTime (n, '-', ':') << endl;

	DBGCLOSE();

	return 0;
}
