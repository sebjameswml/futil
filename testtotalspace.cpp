#include <string>
#include <iostream>
#include <stdexcept>

#include "config.h"
#include <futil/WmlDbg.h>
#include <futil/futil.h>

extern "C" {
#include <stdlib.h>
#include <unistd.h>
}

using namespace std;
using namespace wml;

ofstream DBGSTREAM;

int main (int argc, char** argv)
{
        DBGOPEN ("/tmp/testtotalspace.log");

        try {
                unsigned int total;
                unsigned int available;
                unsigned int used;
                total = futil::totalSpaceKBytes ("/boot");
                available = futil::freeSpaceKBytes ("/boot");
                used = total - available;

                cout << "Total space:        " << total << "\n";
                cout << "Available space:    " << available << "\n";
                cout << "Used space:         " << used << "\n";

                vector<string> flist;
                flist.push_back ("1.file");
                flist.push_back ("2.file");
                flist.push_back ("3.file");
                unsigned int filelistused  = futil::KBytesUsedBy (flist);
                cout << "File list uses      " << filelistused << "\n";

        } catch (const runtime_error& e) {
                cout << "Error: " << e.what() << "\n";
        }

        DBGCLOSE();
        _exit(0);
}
