#include "config.h"
#include "WmlDbg.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

std::ofstream DBGSTREAM;

#include "FoundryUtilities.h"

using namespace std;
using namespace wml;

int main (int argc, char **argv)
{
        DBGOPEN ("./testStringToWords.log");

        /*
         * Tests
         */

        Glib::ustring s ("The quick brown fox can't jump 32.3 feet");
        vector<Glib::ustring> v;
        v.push_back ("The");
        v.push_back ("quick");
        v.push_back ("brown");
        v.push_back ("fox");
        v.push_back ("can't");
        v.push_back ("jump");
        v.push_back ("32.3");
        v.push_back ("feet");

        list<pair<Glib::ustring, vector<Glib::ustring> > > tests;

        /*
         * Space
         */
        tests.push_back(make_pair(s,v));

        /*
         * Mixed separator characters
         */
        Glib::ustring nbsp ("");
        nbsp += static_cast<gunichar>(0x00A0);
        s.replace (3,1,nbsp);
        s.replace (19,1,nbsp);

        tests.push_back(make_pair(s,v));

        /*
         * Non-breaking space
         */
        Glib::ustring::size_type pos (s.find(" "));
        while (pos != Glib::ustring::npos) {
                s.replace (pos,1,nbsp);
                pos = s.find(" ", pos + 1);
        }

        tests.push_back(make_pair(s,v));

        list<pair<Glib::ustring, vector<Glib::ustring> > >::const_iterator testsI = tests.begin();
        unsigned int passed = 0;
        unsigned int failed = 0;

        cout << "----- testStringToWords -----" << endl;

        while (testsI != tests.end()) {

                Glib::ustring in (testsI->first);
                vector<Glib::ustring> out;
                try {
                        out = FoundryUtilities::stringToWords (in);
                } catch (runtime_error& e) {
                        DBGSTREAM << "Exception: '" << e.what() << endl;
                        cerr << "Error: " << e.what() << endl;
                }

                vector<Glib::ustring>::const_iterator iOut (out.begin()), iExp (testsI->second.begin());
                while (iOut != out.end() && *iOut == *iExp) {
                        ++iOut;
                        ++iExp;
                }

                if (iOut == out.end()) {
                        ++passed;
                } else {
                        ++failed;
                        cerr << "Test " << passed + failed << " failed!\n\tInput:\t\t \"" << testsI->first << "\"\n\tOutput:\t\t \"" << *iOut << "\"\n\tExpected:\t \"" << *iExp << "\"" << endl;
                }

                ++testsI;
        }

        cout << passed << " tests passed, " << failed << " tests failed." << endl;

        DBGCLOSE();
        return failed ? 1 : 0;
}
