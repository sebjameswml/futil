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
        DBGOPEN ("./testFilesDiffer.log");

        /*
         * Create test files.
         */
        fstream f;
        string fp1 ("/tmp/difftest1"), fp2 ("/tmp/difftest2"),
                fp3 ("/tmp/difftest3");

        FoundryUtilities::openFilestreamForOverwrite (f, fp1);
        f << "This is a test" << endl;
        FoundryUtilities::closeFilestream (f);

        FoundryUtilities::openFilestreamForOverwrite (f, fp2);
        f << "This is a test" << endl;
        FoundryUtilities::closeFilestream (f);

        FoundryUtilities::openFilestreamForOverwrite (f, fp3);
        f << "This is another test" << endl;
        FoundryUtilities::closeFilestream (f);

        /*
         * Tests
         */

        list<pair<pair<string, string>, bool> > tests;
        tests.push_back(make_pair (make_pair(fp1, fp2), false));
        tests.push_back(make_pair (make_pair(fp1, fp3), true));

        tests.push_back(make_pair (make_pair("/usr/share/wmlpp/wml_masthead.pdf",
                                             "/usr/share/wmlpp/wml_masthead.pdf"), false));
        tests.push_back(make_pair (make_pair("/usr/share/wmlpp/wml_masthead.pdf",
                                             "/usr/share/wmlpp/wml_image_unavailable.png"), true));

        // This one should throw an error
        tests.push_back(make_pair (make_pair("/usr/share/wmlpp/wml_masthead.pdf",
                                             "/usr/share/wmlpp"), true));

        unsigned int passed (0), failed (0), invalid (0);

        cout << "----- testFilesDiffer -----" << endl;

        list<pair<pair<string, string>, bool> >::const_iterator iTest = tests.begin();
        while (iTest != tests.end()) {
                try {
                        bool out (FoundryUtilities::filesDiffer (iTest->first.first, iTest->first.second));
                        if (out == iTest->second) {
                                ++passed;
                        } else {
                                ++failed;
                                cerr << "Test " << passed + failed << " failed!\n"
                                     << "\tInput:\t\t \"" << iTest->first.first
                                     << ", " << iTest->first.second << "\"\n"
                                     << "\tOutput:\t\t \"" << out << "\"\n"
                                     << "\tExpected:\t \"" << iTest->second << "\"" << endl;
                        }
                } catch (const exception& e) {
                        ++invalid;
                        cerr << "Caught an exception: " << e.what() << endl;
                }

                ++iTest;
        }
        cout << passed << " tests passed, " << failed << " tests failed." << endl;
        cout << invalid << " tests were invalid." << endl;

        DBGCLOSE();
        return failed ? 1 : 0;
}
