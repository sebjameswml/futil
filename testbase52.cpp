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

	if (argc>1) {
        stringstream ss;
	UINT64_TYPE x;
	ss << argv[1];
	ss >> x;
	cout << x << " in WML base 52 (64 bit conversion) is: "
	     << FoundryUtilities::formatUint64InBaseN (x, 52)
	     << endl;

	UINT32_TYPE* n;
	UINT32_TYPE num[4];
        stringstream sss;
	sss << argv[1];
	sss >> num[0];
	num[1] = 0;
	num[2] = 0;
	num[3] = 0;
	n = &(num[0]);
	cout << *n << " in WML base 52 (128 bit conversion) is: "
	     << FoundryUtilities::formatUint128InBaseN (n, 4, 52)
	     << endl;
        } else {
		cerr << "Usage: " << argv[0] << " N, where N is a decimal number\n";
	}
	DBGCLOSE();

	return 0;
}
