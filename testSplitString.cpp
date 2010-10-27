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

		// Test splitString()
		vector<string> tokens;
		string s ("'this','is','a','string'");
		string delim ("','");

		FoundryUtilities::splitString (tokens, s, delim);

		cout << "FoundryUtilities::splitString():" << endl;
		vector<string>::iterator iter;
		for (iter = tokens.begin();
		     iter != tokens.end();
		     iter++) {
			cout << *iter << "\n";
		}

		cout << endl << "FoundryUtilities::stringToVector():" << endl;
		// Test stringToVector()
		//delim = ",";
		vector<string> toks2 = FoundryUtilities::stringToVector (s, delim);
		for (iter = toks2.begin();
		     iter != toks2.end();
		     iter++) {
			cout << *iter << "\n";
		}

		string str2 = FoundryUtilities::vectorToString (toks2, delim);
		cout << str2 << endl;

		// Yet another way to deal with values, is to have a
		// delimiting character and an enclosing
		// char. Often, delimiting char is ',' and enclosing
		// char is '"'.

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
