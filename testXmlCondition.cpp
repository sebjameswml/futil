/*
 * This test program is part of WML futil - a library containing
 * static public utility functions and classes used across WML code.
 *
 *  WML futil is Copyright William Matthew Ltd. 2010.
 *
 *  Authors: Seb James <sjames@wmltd.co.uk>
 *           Tamora James <tjames@wmltd.co.uk>
 *           Mark Richardson <mrichardson@wmltd.co.uk>
 *
 *  WML futil is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WML futil is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WML futil.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
