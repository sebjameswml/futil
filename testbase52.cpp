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
        DBGOPEN ("testbase52.log");

	cout << "0 in WML base 52 is: "
	     << FoundryUtilities::formatUint64InBaseN ((UINT64_TYPE)0, 52)
	     << endl;

	cout << "86400 in WML base 52 (10 days of a WML eval) is: "
	     << FoundryUtilities::formatUint64InBaseN ((UINT64_TYPE)86400, 52)
	     << endl;

	if (argc>1) {
        stringstream ss;
	UINT64_TYPE x;
	ss << argv[1];
	ss >> x;
	cout << x << " in WML base 52 is: "
	     << FoundryUtilities::formatUint64InBaseN (x, 52)
	     << endl;
        }
	DBGCLOSE();

	return 0;
}
