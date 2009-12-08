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
	string inputString("A SIMPLE ascii string this is. A simple ascii string this is. A simple ascii string this is. A simple ascii string this is. A simple ascii string this is.");
	string outputString("");

	try {
		FoundryUtilities::doIconv ("ISO-8859-1", "UTF-8", inputString, outputString);
	} catch (const exception& e) {
		cerr << "Exception in FoundryUtilities::doIconv(): " << e.what() << endl;
		return -1;
	}

	cerr << "Transcoded: '" << outputString << "'\n";



	string inputString2("It's pounds: £.");
	string outputString2("");

	try {
		FoundryUtilities::doIconv ("ISO-8859-1", "UTF-8", inputString2, outputString2);
	} catch (const exception& e) {
		cerr << "Exception in FoundryUtilities::doIconv(): " << e.what() << endl;
		return -1;
	}

	cerr << "Transcoded: '" << outputString2 << "'\n";

	string::size_type i = 0;
	while (i<outputString2.size()) {
		cerr << "char " << i << " is " << (int)outputString2[i] << "(" << (char)outputString2[i] << ")" << endl;
		i++;
	}

	cout << outputString2;

	return 0;
}
