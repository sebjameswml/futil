/*
 * A test of string-splitting functions
 */

#include "config.h"
#include "WmlDbg.h"
#include <iostream>
#include <string>
#include <vector>

extern "C" {
#include <unistd.h>
}

std::ofstream DBGSTREAM;

#include "futil.h"

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

                futil::splitString (tokens, s, delim);

                cout << "futil::splitString():" << endl;
                vector<string>::iterator i;
                for (i = tokens.begin(); i != tokens.end(); i++) {
                        cout << *i << "\n";
                }

                cout << endl << "futil::stringToVector():" << endl;
                // Test stringToVector()
                //delim = ",";
                vector<string> toks2 = futil::stringToVector (s, delim);
                for (i = toks2.begin(); i != toks2.end(); i++) {
                        cout << *i << "\n";
                }

                string str2 = futil::vectorToString (toks2, delim);
                cout << str2 << endl;

                // Yet another way to deal with values, is to have a
                // delimiting character and an enclosing
                // char. Often, delimiting char is ',' and enclosing
                // char is '"'.

                // Use s for this first test
                string seps(" ,");
                string encs("\"'");
                vector<string> toks3 = futil::splitStringWithEncs (s, seps, encs);
                cout << "Output of splitStringWithEncs for the string '" << s << "':\n";
                for (i = toks3.begin(); i != toks3.end(); i++) {
                        cout << *i << "\n";
                }
                s = "'this\"' is, a, \"string";
                toks3 = futil::splitStringWithEncs (s, seps, encs);
                cout << "Output of splitStringWithEncs for the string '" << s << "':\n";
                for (i = toks3.begin(); i != toks3.end(); i++) {
                        cout << *i << "\n";
                }

                Glib::ustring gs ("'this\"' is, a, \"string");
                vector<Glib::ustring> toks4 = futil::splitStringWithEncs (gs);
                vector<Glib::ustring>::const_iterator gi;
                cout << "Output of splitStringWithEncs for the Glib ustring '" << gs.raw() << "':\n";
                for (gi = toks4.begin(); gi != toks4.end(); gi++) {
                        cout << gi->raw() << "\n";
                }

                string forCount ("ababa");
                cout << "In string '" << forCount << "':\n";
                cout << "Number of a chars:" << futil::countChars (forCount, 'a') << endl;;
                cout << "Number of b chars:" << futil::countChars (forCount, 'b') << endl;;

                string emptyStr("");
                cout << "Number of b chars in an empty string:"
                     << futil::countChars (emptyStr, 'b') << endl;;

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
