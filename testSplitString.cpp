#include "config.h"
#include "WmlDbg.h"
#include <iostream>
#include <string>
#include <vector>

std::ofstream DBGSTREAM;

#include "FoundryUtilities.h"

using namespace std;
using namespace wml;

int main (int argc, char **argv)
{
	DBGOPEN ("/tmp/testSplitString.log");
	try {

		vector<string> tokens;
		string s ("'this','is','a','string'");
		string delim ("','");

		FoundryUtilities::splitString (tokens, s, delim);

		vector<string>::iterator iter;
		for (iter = tokens.begin();
		     iter != tokens.end();
		     iter++) {
			cout << *iter << "\n";
		}

	} catch (const exception& e) {
                // handle error condition; should catch any
                // exception thrown above
                DBGSTREAM << "Exception: '" << e.what() << "'\n";
                cerr << "Exception: '" << e.what() << "'\n";
                DBGSTREAM.flush();
	}

	DBGCLOSE();
	_exit(0);
}
