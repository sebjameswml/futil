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
        DBGOPEN ("./testXmlEscape.log");

        /*
         * Tests
         */

        list<pair<string, string> > tests;


        tests.push_back(make_pair(
                               "B&B",
                               "B&amp;B"));

        tests.push_back(make_pair(
                               "&<>'\"",
                               "&amp;&lt;&gt;&apos;&quot;"));

        tests.push_back(make_pair(
                               "£50",
                               "&#163;50"));

        unsigned int passed = 0;
        unsigned int failed = 0;

        cout << "----- testXmlEscape -----" << endl;

        list<pair<string, string> >::const_iterator iTest = tests.begin();
        while (iTest != tests.end()) {
                string out (FoundryUtilities::xmlEscape (iTest->first));
                if (out == iTest->second) {
                        ++passed;
                } else {
                        ++failed;
                        cerr << "Test " << passed + failed << " failed!\n"
                             << "\tInput:\t\t \"" << iTest->first << "\"\n"
                             << "\tOutput:\t\t \"" << out << "\"\n"
                             << "\tExpected:\t \"" << iTest->second << "\"" << endl;
                }
                ++iTest;
        }
        cout << passed << " tests passed, " << failed << " tests failed." << endl;

        DBGCLOSE();
        return failed ? 1 : 0;
}
