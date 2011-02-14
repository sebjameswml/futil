/*
 * A test of string-splitting functions
 */

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
	DBGOPEN ("./testSplitString.log");
	try {

		// Test splitString()
		vector<string> tokens;
		string s ("'this','is','a','string'");
		string delim ("','");

		FoundryUtilities::splitString (tokens, s, delim);

		cout << "FoundryUtilities::splitString():" << endl;
		vector<string>::iterator i;
		for (i = tokens.begin(); i != tokens.end(); i++) {
			cout << *i << "\n";
		}

		cout << endl << "FoundryUtilities::stringToVector():" << endl;
		// Test stringToVector()
		//delim = ",";
		vector<string> toks2 = FoundryUtilities::stringToVector (s, delim);
		for (i = toks2.begin(); i != toks2.end(); i++) {
			cout << *i << "\n";
		}

		string str2 = FoundryUtilities::vectorToString (toks2, delim);
		cout << str2 << endl;

		// Yet another way to deal with values, is to have a
		// delimiting character and an enclosing
		// char. Often, delimiting char is ',' and enclosing
		// char is '"'.

		// Use s for this first test
		string seps(" ,");
		string encs("\"'");
		vector<string> toks3 = FoundryUtilities::splitStringWithEncs (s, seps, encs);
		cout << "Output of splitStringWithEncs for the string '" << s << "':\n";
		for (i = toks3.begin(); i != toks3.end(); i++) {
			cout << *i << "\n";
		}
		s = "'this\"' is, a, \"string";
		toks3 = FoundryUtilities::splitStringWithEncs (s, seps, encs);
		cout << "Output of splitStringWithEncs for the string '" << s << "':\n";
		for (i = toks3.begin(); i != toks3.end(); i++) {
			cout << *i << "\n";
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
