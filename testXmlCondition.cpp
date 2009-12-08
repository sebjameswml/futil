#include <iostream>
#include <fstream>
#include <sstream>

#include "config.h"
#include "WmlDbg.h"
#include "FoundryUtilities.h"

using namespace std;
using namespace wml;

std::ofstream DBGSTREAM;

int main ()
{
	string test;

	test = "XmlNotAllowed";
	cout << "Before condition, test is '" << test << "'\n";
	FoundryUtilities::conditionAsXmlTag (test);
	cout << "After  condition, test is '" << test << "'\n";

	test = "2NotAllowed";
	cout << "Before condition, test is '" << test << "'\n";
	FoundryUtilities::conditionAsXmlTag (test);
	cout << "After  condition, test is '" << test << "'\n";

	test = "XML2Not&%All.o_wed";
	cout << "Before condition, test is '" << test << "'\n";
	FoundryUtilities::conditionAsXmlTag (test);
	cout << "After  condition, test is '" << test << "'\n";

	return 0;
}
