/*
 * This file is part of WML futil - a class containing static public
 * utility functions used across WML code.
 *
 *  WML futil is Copyright William Matthew Ltd. 2010.
 *
 *  Authors: Seb James <sjames@wmltd.co.uk>
 *           Tamora James <tjames@wmltd.co.uk>
 *           Mark Richardson <mrichardson@wmltd.co.uk>
 *
 *  WML futil is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WML futil is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with WML futil.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef __GNUG__
#  pragma implementation
#endif

#include "config.h"
#include "WmlDbg.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include <list>
#include <vector>
#include <set>
#include <algorithm>

#include "FoundryUtilities.h"
#include "Process.h"
#include "fileno.h"

extern "C" {
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <dirent.h>
#include <syslog.h>
#include <uuid/uuid.h>
#include <string.h>
#include <iconv.h>
#include <magic.h>
#include <regex.h>
}

using namespace std;

/*
 * FoundryUtilities implementation
 */
//@{
wml::FoundryUtilities::FoundryUtilities (void)
{
}

wml::FoundryUtilities::~FoundryUtilities (void)
{
}

int
wml::FoundryUtilities::ensureUnixNewlines (std::string& input)
{
        int num = 0;

        for (unsigned int i=0; i<input.size(); i++) {
                if (input[i] == '\r') {
                        input.erase(i,1);
                        num++;
                }
        }

        return num; // The number of \r characters we found in the string.
}

int
wml::FoundryUtilities::stripTrailingCarriageReturn (std::string& input)
{
        if (input[input.size()-1] == '\r') {
                input.erase(input.size()-1, 1);
                return 1;
        }
        return 0;
}

int
wml::FoundryUtilities::stripTrailingWhitespace (std::string& input)
{
        char c;
        string::size_type len = input.size(), pos = len;
        while (pos > 0 &&
               ((c = input[pos-1]) == ' '
                || c == '\t'
                || c == '\n'
                || c == '\r')) {
                pos--;
        }
        input.erase (pos);
        return (len - pos);
}

int
wml::FoundryUtilities::stripChars (std::string& input, const std::string& charList)
{
        int rtn = 0;
        string::size_type pos;
        while ((pos = input.find_last_of (charList)) != string::npos) {
                input.erase (pos, 1);
                ++rtn;
        }
        return rtn;
}

int
wml::FoundryUtilities::stripChars (std::string& input, const char& charList)
{
        int rtn = 0;
        string::size_type pos;
        while ((pos = input.find_last_of (charList)) != string::npos) {
                input.erase (pos, 1);
                ++rtn;
        }
        return rtn;
}

int
wml::FoundryUtilities::stripChars (Glib::ustring& input, const Glib::ustring& charList)
{
        int rtn;
        string::size_type pos;
        while ((pos = input.find_last_of (charList)) != Glib::ustring::npos) {
                input.erase (pos, 1);
                ++rtn;
        }
        return rtn;
}

int
wml::FoundryUtilities::stripChars (Glib::ustring& input, const gunichar& singleChar)
{
        int rtn;
        string::size_type pos;
        while ((pos = input.find_last_of (singleChar)) != Glib::ustring::npos) {
                input.erase (pos, 1);
                ++rtn;
        }
        return rtn;
}

int
wml::FoundryUtilities::convertCHexCharSequences (std::string& input)
{
        // This converts a string containing C style hex sequences
        // like "\x41\x42\x43" into the corresponding characters
        // ("ABC" for the example).

        string::iterator readPos = input.begin();
        string::iterator writePos = input.begin();
        string::size_type newSize = 0;
        char n1 = '\0', n2 = '\0'; // two Ns in "1xNN"
        char c = 0;
        int count = 0;

        while (readPos != input.end()) {

                c = *readPos;

                if (*readPos == '\\') {
                        // We have a possible hex escape sequence.
                        ++readPos;
                        if (readPos != input.end() && *readPos == 'x') {
                                // We have a hex escape sequence. Read in next two chars
                                ++readPos;
                                if (readPos != input.end()) {
                                        n1 = *readPos;
                                        ++readPos;
                                        if (readPos != input.end()) {
                                                n2 = *readPos;
                                                ++count;
                                                // Now create the replacement for c.
                                                c = 0;
                                                switch (n1) {
                                                case '0':
                                                        // c |= 0 << 4;
                                                        break;
                                                case '1':
                                                        c |= 1 << 4;
                                                        break;
                                                case '2':
                                                        c |= 2 << 4;
                                                        break;
                                                case '3':
                                                        c |= 3 << 4;
                                                        break;
                                                case '4':
                                                        c |= 4 << 4;
                                                        break;
                                                case '5':
                                                        c |= 5 << 4;
                                                        break;
                                                case '6':
                                                        c |= 6 << 4;
                                                        break;
                                                case '7':
                                                        c |= 7 << 4;
                                                        break;
                                                case '8':
                                                        c |= 8 << 4;
                                                        break;
                                                case '9':
                                                        c |= 9 << 4;
                                                        break;
                                                case 'a':
                                                case 'A':
                                                        c |= 10 << 4;
                                                        break;
                                                case 'b':
                                                case 'B':
                                                        c |= 11 << 4;
                                                        break;
                                                case 'c':
                                                case 'C':
                                                        c |= 12 << 4;
                                                        break;
                                                case 'd':
                                                case 'D':
                                                        c |= 13 << 4;
                                                        break;
                                                case 'e':
                                                case 'E':
                                                        c |= 14 << 4;
                                                        break;
                                                case 'f':
                                                case 'F':
                                                        c |= 15 << 4;
                                                        break;
                                                default:
                                                        break;
                                                }

                                                switch (n2) {
                                                case '0':
                                                        // c |= 0;
                                                        break;
                                                case '1':
                                                        c |= 1;
                                                        break;
                                                case '2':
                                                        c |= 2;
                                                        break;
                                                case '3':
                                                        c |= 3;
                                                        break;
                                                case '4':
                                                        c |= 4;
                                                        break;
                                                case '5':
                                                        c |= 5;
                                                        break;
                                                case '6':
                                                        c |= 6;
                                                        break;
                                                case '7':
                                                        c |= 7;
                                                        break;
                                                case '8':
                                                        c |= 8;
                                                        break;
                                                case '9':
                                                        c |= 9;
                                                        break;
                                                case 'a':
                                                case 'A':
                                                        c |= 10;
                                                        break;
                                                case 'b':
                                                case 'B':
                                                        c |= 11;
                                                        break;
                                                case 'c':
                                                case 'C':
                                                        c |= 12;
                                                        break;
                                                case 'd':
                                                case 'D':
                                                        c |= 13;
                                                        break;
                                                case 'e':
                                                case 'E':
                                                        c |= 14;
                                                        break;
                                                case 'f':
                                                case 'F':
                                                        c |= 15;
                                                        break;
                                                default:
                                                        break;
                                                }

                                        } else {
                                                // Nothing following "\xN", step back 3.
                                                --readPos;
                                                --readPos;
                                                --readPos;
                                        }
                                } else {
                                        // Nothing following "\x", step back 2.
                                        --readPos;
                                        --readPos;
                                }

                        } else {
                                // Not an escape sequence, just a '\' character. Step back 1.
                                --readPos;
                        }

                } else {
                        // We already set writePos to readPos and c to *readPos.
                }

                // if need to write
                *writePos = c;
                ++writePos;
                ++newSize;

                ++readPos;
        }

        // Terminate the now possibly shorter string:
        input.resize (newSize);

        return count;
}

int
wml::FoundryUtilities::stripTrailingSpaces (std::string& input)
{
        return FoundryUtilities::stripTrailingChars (input);
}

int
wml::FoundryUtilities::stripTrailingSpaces (Glib::ustring& input)
{
        return FoundryUtilities::stripTrailingChars (input);
}

int
wml::FoundryUtilities::stripTrailingChars (std::string& input, const char c)
{
        int i = 0;
        while (input.size()>0 && input[input.size()-1] == c) {
                input.erase (input.size()-1, 1);
                i++;
        }
        return i;
}

int
wml::FoundryUtilities::stripTrailingChars (Glib::ustring& input, const gunichar c)
{
        int i = 0;
        while (input.size()>0 && input[input.size()-1] == c) {
                input.erase (input.size()-1, 1);
                i++;
        }
        return i;
}

int
wml::FoundryUtilities::stripLeadingWhitespace (std::string& input)
{
        char c;
        string::size_type pos = 0;
        while (pos<input.size() &&
               ((c = input[pos]) == ' '
                || c == '\t'
                || c == '\n'
                || c == '\r')) {
                pos++;
        }
        input.erase (0, pos);
        return pos;
}

int
wml::FoundryUtilities::stripWhitespace (std::string& input)
{
        int n = FoundryUtilities::stripLeadingWhitespace (input);
        n += FoundryUtilities::stripTrailingWhitespace (input);
        return n;
}

int
wml::FoundryUtilities::stripLeadingSpaces (std::string& input)
{
        return FoundryUtilities::stripLeadingChars (input);
}

int
wml::FoundryUtilities::stripLeadingSpaces (Glib::ustring& input)
{
        return FoundryUtilities::stripLeadingChars (input);
}

int
wml::FoundryUtilities::stripLeadingChars (std::string& input, const char c)
{
        int i = 0;
        while (input.size()>0 && input[0] == c) {
                input.erase (0, 1);
                i++;
        }
        return i;
}

int
wml::FoundryUtilities::stripLeadingChars (Glib::ustring& input, const gunichar c)
{
        int i = 0;
        while (input.size()>0 && input[0] == c) {
                input.erase (0, 1);
                i++;
        }
        return i;
}

int
wml::FoundryUtilities::searchReplace (const char* searchTerm,
                                      const char* replaceTerm,
                                      std::string& data,
                                      const bool replaceAll)
{
        int count = 0;
        string::size_type pos = 0;
        string::size_type ptr = string::npos;
        string::size_type stl = strlen (searchTerm);
        if (replaceAll) {
                pos = data.size();
                while ((ptr = data.rfind (searchTerm, pos)) != string::npos) {
                        data.erase (ptr, stl);
                        data.insert (ptr, replaceTerm);
                        count++;
                        if (ptr >= stl) {
                                // This is a move backwards along the
                                // string far enough that we don't
                                // match a substring of the last
                                // replaceTerm in the next search.
                                pos = ptr - stl;
                        } else {
                                break;
                        }
                }
        } else {
                // Replace first only
                if ((ptr = data.find (searchTerm, pos)) != string::npos) {
                        data.erase (ptr, strlen(searchTerm));
                        data.insert (ptr, replaceTerm);
                        count++;
                }
        }

        return count;
}

void
wml::FoundryUtilities::conditionAsXmlTag (std::string& str)
{
        // 1) Replace chars which are disallowed in an XML tag
        string::size_type ptr = string::npos;

        // We allow numeric and alpha chars, the underscore and the
        // hyphen. colon strictly allowed, but best avoided.
        while ((ptr = str.find_last_not_of (CHARS_NUMERIC_ALPHA"_-", ptr)) != string::npos) {
                // Replace the char with an underscore:
                str[ptr] = '_';
                ptr--;
        }

        // 2) Check first 3 chars don't spell xml (in any case)
        string firstThree = str.substr (0,3);
        transform (firstThree.begin(), firstThree.end(),
                   firstThree.begin(), wml::to_lower());
        if (firstThree == "xml") {
                // Prepend 'A'
                string newStr("_");
                newStr += str;
                str = newStr;
        }

        // 3) Prepend an '_' if initial char begins with a numeral or hyphen
        if (str[0] > 0x29 && str[0] < 0x3a) {
                // Prepend '_'
                string newStr("_");
                newStr += str;
                str = newStr;
        }
}

int
wml::FoundryUtilities::searchReplace (const std::string& searchTerm,
                                      const std::string& replaceTerm,
                                      std::string& data,
                                      const bool replaceAll)
{
        const char* st = searchTerm.c_str();
        const char* rt = replaceTerm.c_str();
        return FoundryUtilities::searchReplace (st, rt, data, replaceAll);
}

int
wml::FoundryUtilities::searchReplaceInFile (const std::string searchTerm,
                                            const std::string replaceTerm,
                                            std::string fileName,
                                            const bool replaceAll)
{
        int count(0);

        ifstream f;
        f.open (fileName.c_str(), ios::in);
        if (!f.is_open()) {
                throw runtime_error ("searchReplaceInFile(): Couldn't open the target file");
        }
        ofstream of;
        string tfn = FoundryUtilities::generateRandomFilename ("/tmp/futil_srchrepl_");
        of.open (tfn.c_str(), ios::out|ios::trunc);
        if (!of.is_open()) {
                throw runtime_error ("searchReplaceInFile(): Couldn't open the temp file");
        }

        string line;
        while (getline (f, line, '\n')) {
                count += FoundryUtilities::searchReplace (searchTerm, replaceTerm,
                                                          line, replaceAll);
                of << line << '\n';
        }
        f.close();
        of.close();

        // Finally, move the temp file to the input file.
        try {
                FoundryUtilities::moveFile (tfn, fileName);
        } catch (const std::exception& e) {
                stringstream msg;
                msg << "searchReplaceInFile(): Failed to move "
                    << "temp file onto input file: " << e.what();
                throw runtime_error (msg.str());
        }

        DBG ("Returning " << count);
        return count;
}

int
wml::FoundryUtilities::deleteLinesContaining (const std::string searchTerm,
                                              const std::string fileName,
                                              const bool deleteEndOfLine)
{
        int count(0);

        ifstream f;
        f.open (fileName.c_str(), ios::in);
        if (!f.is_open()) {
                throw runtime_error ("searchReplaceInFile(): Couldn't open the target file");
        }
        ofstream of;
        string tfn = FoundryUtilities::generateRandomFilename ("/tmp/futil_srchrepl_");
        of.open (tfn.c_str(), ios::out|ios::trunc);
        if (!of.is_open()) {
                throw runtime_error ("searchReplaceInFile(): Couldn't open the temp file");
        }

        string line;
        while (getline (f, line, '\n')) {
                if (line.find (searchTerm, 0) == string::npos) {
                        // No match
                        of << line << '\n';
                } else {
                        // Some sort of match.
                        count++;
                        if (deleteEndOfLine) {
                                // Add nothing to ofstream
                        } else {
                                // Add just the EOL char. See if we
                                // need to add a CR before the NL:
                                if (line[line.size()-1] == '\r') {
                                        of << '\r';
                                }
                                of << '\n';
                        }
                }


        }
        f.close();
        of.close();

        // Finally, move the temp file to the input file.
        try {
                FoundryUtilities::moveFile (tfn, fileName);
        } catch (const std::exception& e) {
                stringstream msg;
                msg << "searchReplaceInFile(): Failed to move "
                    << "temp file onto input file: " << e.what();
                throw runtime_error (msg.str());
        }

        DBG ("Returning " << count);
        return count;
}

unsigned int
wml::FoundryUtilities::countChars (const std::string& line, const char c)
{
        unsigned int count(0);
        string::const_iterator i = line.begin();
        while (i != line.end()) {
                if (*i++ == c) { ++count; }
        }
        return count;
}

unsigned int
wml::FoundryUtilities::getMemory (void)
{
        ifstream f ("/proc/meminfo");

        if (!f.is_open()) {
                return 0;
        }

        // MemTotal is the first line of meminfo.
        string line;
        if (getline (f, line, '\n') == false) {
                f.close();
                return 0;
        }
        f.close();

        unsigned int i=0, j=0, k=0;
        char c = line[0];
        while (c != '\0') {
                if (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9') {
                        // It's a numeral
                        break;
                }
                c = line[++i];
        }
        j=i; // i at start of string

        while (line[i] != ' ' && line[i] != '\t' && line[i] != '\0') {
                k++; i++;
        }
        // k is length of string

        stringstream ss;
        unsigned int memTotal = 0;
        ss << line.substr (j, k);
        ss >> memTotal;

        return memTotal<<10; // Left shift 10 to return bytes, not kbytes
}

unsigned int
wml::FoundryUtilities::getCachedMemory (void)
{
        ifstream f ("/proc/meminfo");

        if (!f.is_open()) {
                return 0;
        }

        // Cached is the fourth line of meminfo and is first line with "Cached" in.
        string line;
        while (getline (f, line, '\n') != false) {
                if (line.find ("Cached", 0) != string::npos) {
                        break;
                }
        }
        f.close();

        if (line.find ("Cached", 0) == string::npos) {
                return 0;
        }

        unsigned int i=0, j=0, k=0;
        char c = line[0];
        while (c != '\0') {
                if (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9') {
                        // It's a numeral
                        break;
                }
                c = line[++i];
        }
        j=i; // i at start of string

        while (line[i] != ' ' && line[i] != '\t' && line[i] != '\0') {
                k++; i++;
        }
        // k is length of string

        stringstream ss;
        unsigned int memCached = 0;
        ss << line.substr (j, k);
        ss >> memCached;

        return memCached<<10; // Left shift 10 to return bytes, not kbytes
}

unsigned int
wml::FoundryUtilities::getBufferedMemory (void)
{
        ifstream f ("/proc/meminfo");

        if (!f.is_open()) {
                return 0;
        }

        // Buffers is the 3rd line of meminfo and is first line with "Buffers" in.
        string line;
        while (getline (f, line, '\n') != false) {
                if (line.find ("Buffers", 0) != string::npos) {
                        break;
                }
        }
        f.close();

        if (line.find ("Buffers", 0) == string::npos) {
                return 0;
        }

        unsigned int i=0, j=0, k=0;
        char c = line[0];
        while (c != '\0') {
                if (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9') {
                        // It's a numeral
                        break;
                }
                c = line[++i];
        }
        j=i; // i at start of string

        while (line[i] != ' ' && line[i] != '\t' && line[i] != '\0') {
                k++; i++;
        }
        // k is length of string

        stringstream ss;
        unsigned int memBuffered = 0;
        ss << line.substr (j, k);
        ss >> memBuffered;

        return memBuffered<<10; // Left shift 10 to return bytes, not kbytes
}

unsigned int
wml::FoundryUtilities::getActiveMemory (void)
{
        ifstream f ("/proc/meminfo");

        if (!f.is_open()) {
                return 0;
        }

        string line;
        while (getline (f, line, '\n') != false) {
                if (line.find ("Active", 0) != string::npos) {
                        break;
                }
        }
        f.close();

        if (line.find ("Active", 0) == string::npos) {
                return 0;
        }

        unsigned int i=0, j=0, k=0;
        char c = line[0];
        while (c != '\0') {
                if (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9') {
                        // It's a numeral
                        break;
                }
                c = line[++i];
        }
        j=i; // i at start of string

        while (line[i] != ' ' && line[i] != '\t' && line[i] != '\0') {
                k++; i++;
        }
        // k is length of string

        stringstream ss;
        unsigned int memActive = 0;
        ss << line.substr (j, k);
        ss >> memActive;

        return memActive<<10; // Left shift 10 to return bytes, not kbytes
}

unsigned int
wml::FoundryUtilities::getInactiveMemory (void)
{
        ifstream f ("/proc/meminfo");

        if (!f.is_open()) {
                return 0;
        }

        string line;
        while (getline (f, line, '\n') != false) {
                if (line.find ("Inactive", 0) != string::npos) {
                        break;
                }
        }
        f.close();

        if (line.find ("Inactive", 0) == string::npos) {
                return 0;
        }

        unsigned int i=0, j=0, k=0;
        char c = line[0];
        while (c != '\0') {
                if (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9') {
                        // It's a numeral
                        break;
                }
                c = line[++i];
        }
        j=i; // i at start of string

        while (line[i] != ' ' && line[i] != '\t' && line[i] != '\0') {
                k++; i++;
        }
        // k is length of string

        stringstream ss;
        unsigned int memInactive = 0;
        ss << line.substr (j, k);
        ss >> memInactive;

        return memInactive<<10; // Left shift 10 to return bytes, not kbytes
}

float
wml::FoundryUtilities::getLoadAverage (void)
{
        ifstream f ("/proc/loadavg");
        if (!f.is_open()) {
                return -1.0;
        }
        string line;
        while (getline (f, line, '\n') != false) {
                if (!line.empty()) {
                        break;
                }
        }
        f.close();

        // Now read in the most recent load average.
        string::size_type space = line.find_first_of (" ");
        if (space == string::npos) {
                return -2.0;
        }

        stringstream ss;
        float rtn;
        ss << line.substr (0, space);
        ss >> rtn;

        return rtn;
}

float
wml::FoundryUtilities::getUptime (void)
{
        ifstream f ("/proc/uptime");
        if (!f.is_open()) {
                return -1.0;
        }
        string line;
        while (getline (f, line, '\n') != false) {
                if (!line.empty()) {
                        break;
                }
        }
        f.close();

        // Now read in the uptime - that's the first of the two
        // numbers in the line.
        string::size_type space = line.find_first_of (" ");
        if (space == string::npos) {
                return -2.0;
        }

        stringstream ss;
        float rtn;
        ss << line.substr (0, space);
        ss >> rtn;

        return rtn;
}

string
wml::FoundryUtilities::freeSpace (const string dirPath)
{
        stringstream rtn;
        struct statvfs dir;
        int theError = 0;

        if (statvfs (dirPath.c_str(), &dir)) {
                theError = errno;
                rtn << "statvfs returned errno %d" << theError;
                return rtn.str();
        }

        fsblkcnt_t available = 0;
        char suffix[3] = "KB";
        if (dir.f_bavail > 1400000) {
                // Show in GB
                available = dir.f_bavail >> 20;
                suffix[0] = 'G';

        } else if (dir.f_bavail > 140000) {
                // Show in MB
                available = dir.f_bavail >> 10;
                suffix[0] = 'M';
        } else {
                // Show in KB
                available = dir.f_bavail;
        }

        // available is now number of _blocks_ available, need
        // to multiply by fragment size to get bytes total
        available = available * dir.f_frsize;

        // Now available is the number of bytes/kb/mb not
        // kb/mb/gb, so fix that:
        available = available >> 10;

        // Outputs number of free KB/MB/GB
        rtn <<  available << ' ' << suffix;

        return rtn.str();
}

UINT64_TYPE
wml::FoundryUtilities::freeSpaceKBytes (const string dirPath)
{
        struct statvfs dir;
        if (statvfs (dirPath.c_str(), &dir)) {
                return 0;
        }

        // Note that fsblkcnt_t (used in struct statvfs) is a 32 bit
        // unsigned int in our 32 bit environment.


#ifdef YOU_WANT_TO_SEE_HOW_TO_MULT_TWO_32_BIT_NUMBERS
        UINT64_TYPE ans = 0;
        fsblkcnt_t a = 0x80000001;
        fsblkcnt_t b = 0x2;
        cout << "a is 0x" << hex << a << endl;
        cout << "b is 0x" << hex << b << endl;
        // Crucially, we cast the first number to be a 64 bit type.
        ans = static_cast<UINT64_TYPE>(a) * b;
        cout << "64 bit answer 1 is: 0x" << hex << ans << endl;
        ans = static_cast<UINT64_TYPE>(b) * a;
        cout << "64 bit answer 2 is: 0x" << hex << ans << endl;
#endif

        // dir.f_bavail is number of _blocks_ available; need
        // to multiply by fragment size to get bytes total:
        //
        UINT64_TYPE available = static_cast<UINT64_TYPE>(dir.f_bavail) * dir.f_frsize;

        // Now available is the number of bytes not KB so fix:
        available = available >> 10;

        // Unsigned int is large enough to return a free space amount
        // of up to 4096 GiBytes/4 TerraBytes. We may need to deal
        // with network mounted shares with >4TB, so this function
        // would be better returning a UINT64_TYPE
        return available;
}

UINT64_TYPE
wml::FoundryUtilities::KBytesUsedBy (const vector<string>& fileList)
{
        UINT64_TYPE used = 0;
        vector<string>::const_iterator i = fileList.begin();
        while (i != fileList.end()) {
                used = used + static_cast<UINT64_TYPE>((FoundryUtilities::fileSize (*i) >> 10));
                ++i;
        }
        return used;
}

UINT64_TYPE
wml::FoundryUtilities::KBytesUsedBy (const vector<string>& fileList, const string& dirPath)
{
        UINT64_TYPE used = 0;
        vector<string>::const_iterator i = fileList.begin();
        while (i != fileList.end()) {
                string fpath = dirPath + "/" + *i;
                used = used + static_cast<UINT64_TYPE>((FoundryUtilities::fileSize (fpath) >> 10));
                ++i;
        }
        return used;
}

UINT64_TYPE
wml::FoundryUtilities::totalSpaceKBytes (string dirPath)
{
        struct statvfs dir;
        memset (&dir, 0, sizeof(struct statvfs));
        if (statvfs (dirPath.c_str(), &dir)) {
                return 0;
        }

        // f_blocks is  "size of fs in f_frsize units"
        UINT64_TYPE total = static_cast<UINT64_TYPE>(dir.f_blocks) * dir.f_frsize;
        // Turn the total space in bytes into KB
        total = total >> 10;

        return total;
}

float
wml::FoundryUtilities::freeSpaceFraction (const string dirPath)
{
        struct statvfs dir;
        memset (&dir, 0, sizeof(struct statvfs));
        if (statvfs (dirPath.c_str(), &dir)) {
                return 0;
        }

        UINT64_TYPE total = static_cast<UINT64_TYPE>(dir.f_blocks) * dir.f_frsize;
        UINT64_TYPE available = static_cast<UINT64_TYPE>(dir.f_bavail) * dir.f_frsize;

        float freefrac = static_cast<float>(available);
        freefrac = freefrac / total;

        return freefrac;
}

bool
wml::FoundryUtilities::fileExists (const std::string& path)
{
        struct stat * buf = NULL;

        buf = static_cast<struct stat*> (malloc (sizeof (struct stat)));
        memset (buf, 0, sizeof (struct stat));

        if (stat (path.c_str(), buf)) {
                free (buf);
                return false;
        }

        if (S_ISREG (buf->st_mode)
            || S_ISBLK (buf->st_mode)
            || S_ISSOCK (buf->st_mode)
            || S_ISFIFO (buf->st_mode)
            || S_ISLNK (buf->st_mode)
            || S_ISCHR (buf->st_mode)) {
                free (buf);
                return true;
        }

        free (buf);
        return false;
}

bool
wml::FoundryUtilities::blockdevExists (const std::string& path)
{
        struct stat * buf = NULL;

        buf = static_cast<struct stat*> (malloc (sizeof (struct stat)));
        memset (buf, 0, sizeof (struct stat));

        if (stat (path.c_str(), buf)) {
                free (buf);
                return false;
        }

        if (S_ISBLK (buf->st_mode)) {
                free (buf);
                return true;
        }

        free (buf);
        return false;
}

bool
wml::FoundryUtilities::regfileExists (const std::string& path)
{
        struct stat * buf = NULL;

        buf = static_cast<struct stat*> (malloc (sizeof (struct stat)));
        memset (buf, 0, sizeof (struct stat));

        if (stat (path.c_str(), buf)) {
                free (buf);
                return false;
        }

        if (S_ISREG (buf->st_mode)) {
                free (buf);
                return true;
        }

        free (buf);
        return false;
}

bool
wml::FoundryUtilities::socketExists (const std::string& path)
{
        struct stat * buf = NULL;

        buf = static_cast<struct stat*> (malloc (sizeof (struct stat)));
        memset (buf, 0, sizeof (struct stat));

        if (stat (path.c_str(), buf)) {
                free (buf);
                return false;
        }

        if (S_ISSOCK (buf->st_mode)) {
                free (buf);
                return true;
        }

        free (buf);
        return false;
}

bool
wml::FoundryUtilities::fifoExists (const std::string& path)
{
        struct stat * buf = NULL;

        buf = static_cast<struct stat*> (malloc (sizeof (struct stat)));
        memset (buf, 0, sizeof (struct stat));

        if (stat (path.c_str(), buf)) {
                free (buf);
                return false;
        }

        if (S_ISFIFO (buf->st_mode)) {
                free (buf);
                return true;
        }

        free (buf);
        return false;
}

bool
wml::FoundryUtilities::linkExists (const std::string& path)
{
        struct stat * buf = NULL;

        buf = static_cast<struct stat*> (malloc (sizeof (struct stat)));
        memset (buf, 0, sizeof (struct stat));

        if (stat (path.c_str(), buf)) {
                free (buf);
                return false;
        }

        if (S_ISLNK (buf->st_mode)) {
                free (buf);
                return true;
        }

        free (buf);
        return false;
}

bool
wml::FoundryUtilities::chardevExists (const std::string& path)
{
        struct stat * buf = NULL;

        buf = static_cast<struct stat*> (malloc (sizeof (struct stat)));
        memset (buf, 0, sizeof (struct stat));

        if (stat (path.c_str(), buf)) {
                free (buf);
                return false;
        }

        if (S_ISCHR (buf->st_mode)) {
                free (buf);
                return true;
        }

        free (buf);
        return false;
}

bool
wml::FoundryUtilities::fileExists (const char * path)
{
        string thePath = path;
        return fileExists (thePath);
}

bool
wml::FoundryUtilities::dirExists (const std::string& path)
{
        DIR* d;
        if (!(d = opendir (path.c_str()))) {
                // Dir doesn't exist.
                return false;
        } else {
                // Dir does exist.
                (void) closedir (d);
                return true;
        }
}

void
wml::FoundryUtilities::createDir (const std::string path,
                                  const mode_t mode,
                                  const int uid, const int gid)
{
        DBG2 ("Called for path '" << path << "'");
        if (path.empty()) {
                // Create no directory. Just return.
                return;
        }

        // Set to true if we are provded with an absolute filepath
        bool pathIsAbsolute(false);

        // Set umask to 0000 to stop it interfering with mode
        int oldUmask = umask (0000);
        string::size_type pos, lastPos = path.size()-1;
        vector<string> dirs;
        if ((pos = path.find_last_of ('/', lastPos)) == string::npos) {
                // Path is not absolute, single directory. NB: This
                // will be created in the ROOT of the filesystem tree
                // (if permissions allow)
                dirs.push_back (path);
        } else {
                // Definitely DO have a '/' in the path:
                if (path[0] == '/') {
                        pathIsAbsolute = true;
                }

                while ((pos = path.find_last_of ('/', lastPos)) != 0) {
                        dirs.push_back (path.substr(pos+1, lastPos-pos));
                        DBG2 ("Push back directory " << path.substr(pos+1, lastPos-pos));
                        lastPos = pos-1;
                }
                DBG2 ("Push back directory " << path.substr(1, lastPos));
                dirs.push_back (path.substr(1, lastPos));
        }

        vector<string>::reverse_iterator i = dirs.rbegin();
        string prePath("");
        bool first(true);
        while (i != dirs.rend()) {
                if (first && !pathIsAbsolute) {
                        prePath += "./" + *i;
                        first = true;
                } else if (first && pathIsAbsolute) {
                        prePath += "/" + *i;
                        first = true;
                } else {
                        prePath += "/" + *i;
                }
                DBG2 ("mkdir " << prePath.c_str());
                int rtn = mkdir (prePath.c_str(), mode);
                if (rtn) {
                        int e = errno;
                        stringstream emsg;
                        emsg << "createDir(): mkdir() set error: ";
                        switch (e) {
                        case EACCES:
                                emsg << "Permission is denied";
                                break;
                        case EEXIST:
                                // Path exists, though maybe not as a directory.
                                // Set mode/ownership before moving on:
                                if (uid>-1 && gid>-1) {
                                        chown (prePath.c_str(), static_cast<uid_t>(uid), static_cast<gid_t>(gid));
                                        chmod (prePath.c_str(), mode);
                                }
                                i++;
                                continue;
                                break;
                        case EFAULT:
                                emsg << "Bad address";
                                break;
                        case ELOOP:
                                emsg << "Too many symlinks in " << prePath;
                                break;
                        case ENAMETOOLONG:
                                emsg << "File name (" << prePath << ") too long";
                                break;
                        case ENOENT:
                                emsg << "Path '" << prePath << "' invalid (part or all of it)";
                                break;
                        case ENOMEM:
                                emsg << "Out of kernel memory";
                                break;
                        case ENOSPC:
                                emsg << "Out of storage space/quota exceeded.";
                                break;
                        case ENOTDIR:
                                emsg << "component of the path '" << prePath << "' is not a directory";
                                break;
                        case EPERM:
                                emsg << "file system doesn't support directory creation";
                                break;
                        case EROFS:
                                emsg << "path '" << prePath << "' refers to location on read only filesystem";
                                break;
                        default:
                                emsg << "unknown error";
                                break;
                        }
                        throw runtime_error (emsg.str());
                }
                if (uid>-1 && gid>-1) {
                        chown (prePath.c_str(), static_cast<uid_t>(uid), static_cast<gid_t>(gid));
                }
                i++;
        }

        // Reset umask
        umask (oldUmask);
}

void
wml::FoundryUtilities::removeDir (const std::string path)
{
        int rtn = rmdir (path.c_str());
        if (rtn) {
                int e = errno;
                stringstream emsg;
                emsg << "setPermissions(): chmod() set error: ";
                switch (e) {
                case EACCES:
                        emsg << "Permission is denied";
                        break;
                case EBUSY:
                        emsg << "Path in use";
                        break;
                case EFAULT:
                        emsg << "Bad address";
                        break;
                case EINVAL:
                        emsg << "Path has . as last component";
                        break;
                case ELOOP:
                        emsg << "Too many symlinks";
                        break;
                case ENAMETOOLONG:
                        emsg << "File name too long";
                        break;
                case ENOENT:
                        emsg << "Path invalid (part or all of it)";
                        break;
                case ENOMEM:
                        emsg << "Out of kernel memory";
                        break;
                case ENOTDIR:
                        emsg << "component of the path is not a directory";
                        break;
                case EPERM:
                        emsg << "file system doesn't support directory creation";
                        break;
                case EROFS:
                        emsg << "path refers to location on read only filesystem";
                        break;
                default:
                        emsg << "unknown error";
                        break;
                }
                throw runtime_error (emsg.str());
        }
}

void
wml::FoundryUtilities::setPermissions (const string filepath, const mode_t mode)
{
        int rtn = chmod (filepath.c_str(), mode);
        if (rtn) {
                int e = errno;
                stringstream emsg;
                emsg << "setPermissions(): chmod() set error: ";
                switch (e) {
                case EACCES:
                        emsg << "Permission is denied";
                        break;
                case EFAULT:
                        emsg << "Bad address";
                        break;
                case ELOOP:
                        emsg << "Too many symlinks";
                        break;
                case ENAMETOOLONG:
                        emsg << "File name too long";
                        break;
                case ENOENT:
                        emsg << "Path invalid (part or all of it)";
                        break;
                case ENOMEM:
                        emsg << "Out of kernel memory";
                        break;
                case ENOTDIR:
                        emsg << "component of the path is not a directory";
                        break;
                case EPERM:
                        emsg << "file system doesn't support directory creation";
                        break;
                case EROFS:
                        emsg << "path refers to location on read only filesystem";
                        break;
                case EBADF:
                        emsg << "file descriptor is not valid";
                        break;
                case EIO:
                        emsg << "an i/o error occurred";
                        break;
                default:
                        emsg << "unknown error";
                        break;
                }
                throw runtime_error (emsg.str());
        }
}

bool
wml::FoundryUtilities::checkAccess (const std::string& filepath,
                                    const std::string& accessType)
{
        if (accessType.find("r") != string::npos) {
                ifstream in;
                in.open (filepath.c_str(), ios::in);
                if (!in.is_open()) {
                        return false;
                }
                in.close();
        }
        if (accessType.find("w") != string::npos) {
                ofstream out;
                out.open (filepath.c_str(), ios::out);
                if (!out.is_open()) {
                        return false;
                }
                out.close();
        }
        return true;
}

void
wml::FoundryUtilities::setOwnership (const string filepath, const int uid, const int gid)
{
        int rtn = chown (filepath.c_str(), uid, gid);
        if (rtn) {
                int e = errno;
                stringstream emsg;
                emsg << "setOwnership(): chown() set error: ";
                switch (e) {
                case EACCES:
                        emsg << "Permission is denied";
                        break;
                case EFAULT:
                        emsg << "Bad address";
                        break;
                case ELOOP:
                        emsg << "Too many symlinks";
                        break;
                case ENAMETOOLONG:
                        emsg << "File name too long";
                        break;
                case ENOENT:
                        emsg << "Path invalid (part or all of it)";
                        break;
                case ENOMEM:
                        emsg << "Out of kernel memory";
                        break;
                case ENOTDIR:
                        emsg << "component of the path is not a directory";
                        break;
                case EPERM:
                        emsg << "file system doesn't support directory creation";
                        break;
                case EROFS:
                        emsg << "path refers to location on read only filesystem";
                        break;
                case EBADF:
                        emsg << "file descriptor is not valid";
                        break;
                case EIO:
                        emsg << "an i/o error occurred";
                        break;
                default:
                        emsg << "unknown error";
                        break;
                }
                throw runtime_error (emsg.str());
        }
}

void
wml::FoundryUtilities::touchFile (const std::string path)
{
        ofstream f;
        f.open (path.c_str(), ios::out|ios::app);
        if (!f.is_open()) {
                f.open (path.c_str(), ios::out|ios::trunc);
                if (!f.is_open()) {
                        string emsg = "Failed to create file '" + path + "'";
                        throw runtime_error (emsg);
                } else {
                        f.close();
                }
        } else {
                // File is open, was already there
                f.close();
        }
}

bool
wml::FoundryUtilities::dirExists (const char * path)
{
        string thePath = path;
        return dirExists (thePath);
}

void
wml::FoundryUtilities::copyFile (const string& from, const string& to)
{
        ofstream out;

        out.open (to.c_str(), ios::out|ios::trunc);
        if (!out.is_open()) {
                string emsg = "FoundryUtilities::copyFile(): Couldn't open TO file '" + to + "'";
                throw runtime_error (emsg);
        }

        FoundryUtilities::copyFile (from, out);

        out.close();
}

void
wml::FoundryUtilities::copyFile (const char * from, const char * to)
{
        string fromFile(from);
        string toFile(to);
        FoundryUtilities::copyFile (fromFile, toFile);
}

void
wml::FoundryUtilities::copyFile (const string& from, ostream& to)
{
        ifstream in;

        // Test that "from" is a regular file
        if (!FoundryUtilities::regfileExists (from)) {
                stringstream ee;
                ee << "FoundryUtilities::copyFile(): FROM file '"
                   << from << "' is not a regular file";
                throw runtime_error (ee.str());
        }

        in.open (from.c_str(), ios::in);
        if (!in.is_open()) {
                throw runtime_error ("FoundryUtilities::copyFile(): Couldn't open FROM file");
        }

        if (!to) {
                throw runtime_error ("FoundryUtilities::copyFile(): Error occurred in TO stream");
        }

        char buf[64];
        while (!in.eof()) {
                in.read (buf, 63);
                // Find out how many were read
                unsigned int bytes = in.gcount();
                // and write that many to the output stream
                to.write (buf, bytes);
        }

        // Make sure output buffer is flushed.
        to.flush();

        // Finally, close the input.
        in.close();
}

void
wml::FoundryUtilities::truncateFile (const std::string& from,
                                     const std::string& to,
                                     const unsigned int bytes)
{
        ofstream out;

        out.open (to.c_str(), ios::out|ios::trunc);
        if (!out.is_open()) {
                string emsg = "FoundryUtilities::copyFile(): Couldn't open TO file '" + to + "'";
                throw runtime_error (emsg);
        }

        ifstream in;

        // Test that "from" is a regular file
        if (!FoundryUtilities::regfileExists (from)) {
                stringstream ee;
                ee << "FoundryUtilities::truncateFile(): FROM file '"
                   << from << "' is not a regular file";
                throw runtime_error (ee.str());
        }

        in.open (from.c_str(), ios::in);
        if (!in.is_open()) {
                throw runtime_error ("FoundryUtilities::truncateFile(): Couldn't open FROM file");
        }

        if (!out) {
                throw runtime_error ("FoundryUtilities::truncateFile(): Error occurred in TO stream");
        }

        unsigned int loops(0);
        unsigned int maxLoops = bytes / 63;
        unsigned int remaining = bytes % 63;
        char buf[64];
        while (!in.eof() && loops < maxLoops) {
                in.read (buf, 63);
                // Find out how many were read
                unsigned int bytesCopied = in.gcount();
                // and write that many to the output stream
                out.write (buf, bytesCopied);
                ++loops;
        }
        // Copy remaining
        if (!in.eof()) {
                in.read (buf, remaining);
                // Find out how many were read
                unsigned int bytesCopied = in.gcount();
                if (bytesCopied != remaining) {
                        throw runtime_error ("copy error bytesCopied != remaining");
                }
                // and write that many to the output stream
                out.write (buf, bytesCopied);
        }

        // Make sure output buffer is flushed.
        out.flush();

        // Finally, close the input and output
        in.close();
        out.close();
}

void
wml::FoundryUtilities::copyFile (istream& from, const string& to)
{
        char buf[64];
        ofstream f;
        f.open (to.c_str(), ios::out|ios::trunc);
        if (!f.is_open()) {
                stringstream ee;
                ee << "Failed to open output file '" << to << "'";
                throw runtime_error (ee.str());
        }
        while (!from.eof()) {
                from.read (buf, 63);
                f.write (buf, from.gcount());
        }
}

void
wml::FoundryUtilities::copyFileToString (istream& from, string& to)
{
        char buf[64];
        while (!from.eof()) {
                from.read (buf, 63);
                to.append (buf, from.gcount());
        }
}

void
wml::FoundryUtilities::copyFile (const char * from, ostream& to)
{
        string fromFile(from);
        FoundryUtilities::copyFile (fromFile, to);
}

#define COPYFILE_BUFFERSIZE    32768
#define COPYFILE_BUFFERSIZE_MM 32767 // MM: Minus Minus
void
wml::FoundryUtilities::copyFile (FILE * from, string to)
{
        FILE * ofp = NULL;
        long pos;
        int bytes=0, output=0;
        char inputBuffer[COPYFILE_BUFFERSIZE];

        // Get posn of from as we will return the file pointer there when done
        pos = ftell (from);

        ofp = fopen (to.c_str(), "w");
        if (!ofp) {
                throw runtime_error ("FoundryUtilities::copyFile(): Can't open output for writing");
        }
        while ((bytes = fread (inputBuffer, 1, COPYFILE_BUFFERSIZE_MM, from)) > 0) {
                output = fwrite (inputBuffer, 1, bytes, ofp);
                if (output != bytes) {
                        fseek (from, pos, SEEK_SET); /* reset input */
                        throw runtime_error ("FoundryUtilities::copyFile(): Error writing data");
                }
        }
        fclose (ofp); /* close output */
        fseek (from, pos, SEEK_SET); /* reset input */
}

void
wml::FoundryUtilities::copyFile (FILE * from, FILE * to)
{
        long pos;
        int bytes=0, output=0;
        char inputBuffer[COPYFILE_BUFFERSIZE];

        // Get posn of from as we will return the file pointer there when done
        pos = ftell (from);

        if (!to) {
                throw runtime_error ("FoundryUtilities::copyFile(): output is not open for writing");
        }
        while ((bytes = fread (inputBuffer, 1, COPYFILE_BUFFERSIZE_MM, from)) > 0) {
                output = fwrite (inputBuffer, 1, bytes, to);
                if (output != bytes) {
                        fseek (from, pos, SEEK_SET); /* reset input */
                        throw runtime_error ("FoundryUtilities::copyFile(): Error writing data");
                }
        }
        fseek (from, pos, SEEK_SET); /* reset input */
}

void
wml::FoundryUtilities::copyFile (const char* from, FILE * to)
{
        FILE* ifp = fopen (from, "r");
        FoundryUtilities::copyFile (ifp, to);
        fclose (ifp);
}

void
wml::FoundryUtilities::appendFile (const std::string& from, std::ostream& appendTo)
{
        if (!appendTo.good()) {
                throw runtime_error ("Can't append to appendTo, it's not good()");
        }
        ifstream in;
        in.open (from.c_str(), ios::in);
        if (!in.is_open()) {
                throw runtime_error ("FoundryUtilities::copyFile(): Couldn't open FROM file");
        }

        char buf[64];
        while (!in.eof() && appendTo.good()) {
                in.read (buf, 63);
                appendTo.write (buf, in.gcount());
        }

        in.close();
}

void
wml::FoundryUtilities::moveFile (const string from, const string to)
{
        FoundryUtilities::copyFile (from, to);
        FoundryUtilities::unlinkFile (from);
}

void
wml::FoundryUtilities::unlinkFile (const string fpath)
{
        int rtn = unlink (fpath.c_str());
        if (rtn) {
                int theError = errno;
                string emsg;
                switch (theError) {
                case EPERM:
                case EACCES:
                        emsg = "Write access to '" + fpath + "' is not allowed due to permissions";
                        break;
                case EBUSY:
                        emsg = "'" + fpath + "' cannot be removed as it is in use by another process";
                        break;
                case EFAULT:
                        emsg = "'" + fpath + "' points outside your accessible address space";
                        break;
                case EIO:
                        emsg = "I/O error occurred reading '" + fpath + "'";
                        break;
                case EISDIR:
                        emsg = "'" + fpath + "' is a directory";
                        break;
                case ELOOP:
                        emsg = "Too many symlinks encountered in '" + fpath + "'";
                        break;
                case ENAMETOOLONG:
                        emsg = "'" + fpath + "' is too long a name";
                        break;
                case ENOENT:
                        emsg = "'" + fpath + "' does not exist or is a dangling symlink";
                        break;
                case ENOMEM:
                        emsg = "In sufficient kernel memory to open '" + fpath + "'";
                        break;
                case ENOTDIR:
                        emsg = "'" + fpath + "' contains a component that is not a directory";
                        break;
                case EROFS:
                        emsg = "'" + fpath + "' is on a read-only filesystem";
                        break;
                default:
                        emsg = "Unknown error unlinking file '" + fpath + "'";
                        break;
                }

                throw runtime_error (emsg);
        }
}

std::string
wml::FoundryUtilities::fileModDatestamp (const char* filename)
{
        struct stat * buf = NULL;
        stringstream datestamp;

        buf = (struct stat*) malloc (sizeof (struct stat));
        if (!buf) { // Malloc error.
                cout << "malloc error" << endl;
        }
        memset (buf, 0, sizeof(struct stat));
        if (stat (filename, buf)) {
                datestamp << 0;
        } else {
                datestamp << buf->st_mtime;
        }
        if (buf) { free (buf); }

        string dstr = datestamp.str();
        return dstr;
}

bool
wml::FoundryUtilities::filesDiffer (const string& first, const string& second)
{
        if (!(FoundryUtilities::regfileExists (first)
              && FoundryUtilities::regfileExists (second))) {
                throw runtime_error ("Error: expecting two regular files");
        }
        string diffcmd = "diff " + first + " " + second + " >/dev/null 2>&1";
        DBG ("About to call system() with cmd: " << diffcmd);
        // diff returns zero if files are identical, non-zero if files
        // differ.
        return (system (diffcmd.c_str()) != 0);
}

void
wml::FoundryUtilities::getLock (const int fd)
{
        if (fd > 0) {
                // Ok.
        } else {
                throw runtime_error ("Can't lock fd < 1");
        }

        debuglog2 (LOG_DEBUG,
                   "%s: about to call flock(fd=%d, LOCK_EX)",
                   __FUNCTION__, fd);

        if (flock (fd, LOCK_EX)) {
                int e = errno;
                stringstream msg;
                msg << "Error: getLock(): flock set errno:" << e;
                throw runtime_error (msg.str());
        }

        debuglog2 (LOG_DEBUG,
                   "%s: flock(fd=%d, LOCK_EX) returned",
                   __FUNCTION__, fd);
}

void
wml::FoundryUtilities::getLock (const std::fstream& f)
{
        if (!f.is_open()) {
                throw runtime_error ("Can't lock an un-open fstream");
        }
        int fd = fileno(f);
        FoundryUtilities::getLock (fd);
}

void
wml::FoundryUtilities::releaseLock (const int fd)
{
        debuglog2 (LOG_DEBUG, "%s: Called", __FUNCTION__);

        if (fd < 1) {
                debuglog2 (LOG_DEBUG, "%s: Can't unlock fd=0, returning", __FUNCTION__);
                return;
        }

        debuglog2 (LOG_DEBUG,
                   "%s: about to call flock(fd=%d, LOCK_UN)",
                   __FUNCTION__, fd);

        if (flock (fd, LOCK_UN)) {
                int e = errno;
                stringstream msg;
                msg << "Error: releaseLock(): flock set errno:" << e;
                throw runtime_error (msg.str());
        }

        debuglog2 (LOG_DEBUG,
                   "%s: flock(fd=%d, LOCK_UN) returned",
                   __FUNCTION__, fd);
}

void
wml::FoundryUtilities::releaseLock (const std::fstream& f)
{
        if (!f.is_open()) {
                throw runtime_error ("Can't release lock on an un-open fstream");
        }
        int fd = fileno(f);
        FoundryUtilities::releaseLock (fd);
}

void
wml::FoundryUtilities::backSlashEscape (std::string& str)
{
        for (unsigned int i=0; i<str.size(); i++) {
                if (str[i] == '\\') {
                        str.insert ((i++), "\\");
                }
        }
}

void
wml::FoundryUtilities::slashEscape (std::string& str)
{
        for (unsigned int i=0; i<str.size(); i++) {

                // Shell special chars:
                // \ ' " ` < > | ; <Space> <Tab> <Newline> ( ) [ ] ? # $ ^ & * =
                if (str[i] == '\\' ||
                    str[i] == '\'' ||
                    str[i] == '"'  ||
                    str[i] == '`'  ||
                    str[i] == '<'  ||
                    str[i] == '>'  ||
                    str[i] == '|'  ||
                    str[i] == ';'  ||
                    str[i] == ' '  ||
                    str[i] == '\t' ||
                    str[i] == '\n' ||
                    str[i] == '('  ||
                    str[i] == ')'  ||
                    str[i] == '['  ||
                    str[i] == ']'  ||
                    str[i] == '?'  ||
                    str[i] == '#'  ||
                    str[i] == '$'  ||
                    str[i] == '^'  ||
                    str[i] == '&'  ||
                    str[i] == '*'  ||
                    str[i] == '=') {
                        str.insert ((i++), "\\");
                }
        }
}

void
wml::FoundryUtilities::sqlEscape (std::string& str, const bool forPatternMatching)
{
        for (unsigned int i=0; i<str.size(); i++) {

                if (str[i] == '\\'
                    || str[i] == '\''
                    || str[i] == '"'
                    || (forPatternMatching && str[i] == '\%')
                    || (forPatternMatching && str[i] == '_')) {
                        str.insert ((i++), "\\");
                }
        }
}

string
wml::FoundryUtilities::sqlEscapeRtn (const std::string& str, const bool forPatternMatching)
{
        string rtn(str);
        for (unsigned int i=0; i<rtn.size(); i++) {

                if (rtn[i] == '\\'
                    || rtn[i] == '\''
                    || rtn[i] == '"'
                    || (forPatternMatching && rtn[i] == '\%')
                    || (forPatternMatching && rtn[i] == '_')) {
                        rtn.insert ((i++), "\\");
                }
        }
        return rtn;
}

string
wml::FoundryUtilities::xmlEscape (const std::string& s, bool replaceNonAscii)
{
        ostringstream oss;
        string::const_iterator iS = s.begin(), sEnd = s.end();
        while (iS != sEnd) {
                unsigned char c (static_cast<unsigned char>(*iS));
                switch (c) {
                case '&':
                        oss << "&amp;";
                        break;
                case '<':
                        oss << "&lt;";
                        break;
                case '>':
                        oss << "&gt;";
                        break;
                case '"':
                        oss << "&quot;";
                        break;
                case '\'':
                        oss << "&apos;";
                        break;
                default:
                        if (replaceNonAscii && (c < 0x20 || c > 0x7e)) {
                                oss << "&#" << static_cast<unsigned int>(c) << ";";
                        } else {
                                oss << c;
                        }
                }
                ++iS;
        }
        return oss.str();
}

void
wml::FoundryUtilities::stripDosPath (std::string& dosPath)
{
        string tmp;
        string::size_type pos;

        if ((pos = dosPath.find_last_of ('\\', dosPath.size())) == string::npos) {
                // cerr << "No '\\' character in path apparently" << endl;
                return;
        }

        tmp = dosPath.substr (++pos);
        dosPath = tmp;
}

void
wml::FoundryUtilities::stripUnixPath (std::string& unixPath)
{
        string tmp;
        string::size_type pos;

        if ((pos = unixPath.find_last_of ('/', unixPath.size())) == string::npos) {
                // cerr << "No '/' character in path apparently" << endl;
                return;
        }

        tmp = unixPath.substr (++pos);
        unixPath = tmp;
}

void
wml::FoundryUtilities::stripUnixFile (std::string& unixPath)
{
        string tmp;
        string::size_type pos;

        if ((pos = unixPath.find_last_of ('/', unixPath.size())) == string::npos) {
                // No '/' character in path apparently
                return;
        }

        tmp = unixPath.substr (0, pos);
        unixPath = tmp;
}

void
wml::FoundryUtilities::stripFileSuffix (std::string& unixPath)
{
        string::size_type pos;
        pos = unixPath.rfind('.');
        if (pos == string::npos) {
                return;
        }
        // We have a '.' character
        string tmp = unixPath.substr (0, pos);
        if (!tmp.empty()) {
                unixPath = tmp;
        }
}

std::string
wml::FoundryUtilities::generateRandomFilename (const char* prefixPath)
{
        string rtn(prefixPath);

        // Create unique file name for temporary XML file
        unsigned char uuid[50];
        char uuid_unparsed[37];

        uuid_generate (uuid);
        uuid_unparse (uuid, uuid_unparsed);
        uuid_clear (uuid);

        rtn.append (uuid_unparsed);

        return rtn;
}

std::string
wml::FoundryUtilities::generateRandomFilename (const char* prefixPath,
                                               const unsigned int numChars)
{
        unsigned int length = numChars + strlen (prefixPath);
        string rtn = generateRandomFilename (prefixPath);
        return rtn.substr (0, length);
}

std::string
wml::FoundryUtilities::uuidPortion (unsigned int numChars)
{
        string rtn("");

        if (numChars == 0) {
                return rtn;
        }

        if (numChars > 36) {
                numChars = 36;
        }

        // Create unique string with uuid
        unsigned char uuid[50];
        char uuid_unparsed[37];

        uuid_generate (uuid);
        uuid_unparse (uuid, uuid_unparsed);
        uuid_clear (uuid);

        rtn.append (uuid_unparsed);

        return rtn.substr (0, numChars);
}

std::string
wml::FoundryUtilities::randomString (const unsigned int numChars,
                                     const bool includeUppercase,
                                     const bool includeLowercase,
                                     const bool includeNumerals,
                                     const bool allowSimilars)
{
        if (!includeUppercase && !includeLowercase && !includeNumerals) {
                throw runtime_error ("No characters to return");
        }

        string rtn("");

        unsigned char uuid[50];
        uuid_generate (uuid);
        unsigned int i = uuid[0]
                | uuid[2] << 8
                | uuid[4] << 16
                | uuid[6] << 24;
        srandom (i);

        unsigned int count = 0;
        while (count < numChars) {

                int rn = random()%62;

                char c = (char)rn;

                // if rn in range 0-25, then UC, 26 to 51 then LC, 52 to 61, then NUM.
                if (rn < 26 && includeUppercase) {
                        c += 0x41;
                        if (!allowSimilars && (c == 'O' || c == 'S' || c == 'I')) {
                                // Skip
                        } else {
                                rtn += c;
                                count++;
                        }

                } else if (rn > 25 && rn < 52 && includeLowercase) {
                        c += 0x47;
                        if (!allowSimilars && c == 'l') {
                                // Skip
                        } else {
                                rtn += c;
                                count++;
                        }

                } else if (rn > 51 && includeNumerals) {
                        c -= 4;
                        if (!allowSimilars && (c == '0' || c == '1' || c == '5')) {
                                // Skip
                        } else {
                                rtn += c;
                                count++;
                        }

                } else {
                        // Skip, generate another random number
                }
        }

        return rtn.substr (0, numChars);
}

string
wml::FoundryUtilities::generateMd5sum (const std::string& s)
{
        string ifp (FoundryUtilities::generateRandomFilename ("/tmp/md5in-", 8)),
                ofp (FoundryUtilities::generateRandomFilename ("/tmp/md5-", 8));
        ofstream fout;
        fout.open (ifp.c_str());
        if (fout.is_open()) {
                fout << s;
                fout.close();
        }

        string cmd = "md5sum  " + ifp + " > " + ofp;
        int n (system (cmd.c_str()));
        FoundryUtilities::unlinkFile (ifp);
        if (n != 0) {
                throw runtime_error ("Couldn't generate md5sum");
        }

        string rtn ("");

        ifstream fin;
        fin.open (ofp.c_str());
        if (fin.is_open()) {
                getline (fin, rtn);
                fin.close();
        }
        FoundryUtilities::unlinkFile (ofp);

        // Output contains checksum value and file info, separated by
        // one or more spaces:
        // 6618d4e27c76a0b3b9c1780efbb1117c  /tmp/md5in-0877e1a5
        string::size_type pos (rtn.find (" "));
        if (pos != string::npos) {
                rtn.erase (pos);
        }

        return rtn;
}

bool
wml::FoundryUtilities::vectorContains (const std::vector<unsigned int>& v, const unsigned int i)
{
        for (unsigned int k = 0; k<v.size(); k++) {
                if (v[k] == i) {
                        return true;
                }
        }
        return false;
}

int
wml::FoundryUtilities::strVectorContains (const std::vector<string>& v, const string s)
{
        for (unsigned int k = 0; k<v.size(); k++) {
                if (v[k] == s) {
                        return k;
                }
        }
        return -1;
}

int
wml::FoundryUtilities::strVectorMatches (const std::vector<string>& v, const string s)
{
        for (unsigned int k = 0; k<v.size(); k++) {
                if (s.find (v[k], 0) != string::npos) {
                        return k;
                }
        }
        return -1;
}

int
wml::FoundryUtilities::firstNotMatching (const std::vector<string>& v, const string s)
{
        for (unsigned int k = 0; k<v.size(); k++) {
                if (v[k] != s) {
                        return k;
                }
        }
        return -1;
}

bool
wml::FoundryUtilities::listContains (const std::list<unsigned int>& l, const unsigned int i)
{
        list<unsigned int>::const_iterator it = l.begin();
        while (it != l.end()) {
                if ((*it) == i) {
                        return true;
                }
                it++;
        }
        return false;
}

bool
wml::FoundryUtilities::listContains (const std::list<std::string>& l, const std::string& s)
{
        list<string>::const_iterator it = l.begin();
        while (it != l.end()) {
                if ((*it) == s) {
                        return true;
                }
                it++;
        }
        return false;
}

bool
wml::FoundryUtilities::pidLoaded (const int pid)
{
        stringstream path;
        ifstream f;

        if (pid>0) {
                // Find pid n.
                path << "/proc/" << pid << "/status";
                f.open (path.str().c_str(), ios::in);
                if (!f.is_open()) {
                        // No file, so not running.
                        return false;
                }
                string line;
                getline (f, line, '\n');
                getline (f, line, '\n'); // Second line is the State line.
                f.close();
                if (line.size()>7 && (line[7] == 'S' || line[7] == 'R')) {
                        return true;
                }
        }
        return false;
}

std::string
wml::FoundryUtilities::pidCmdline (const int pid)
{
        stringstream path;
        ifstream f;
        string str("");

        if (pid>0) {
                // Find pid n.
                path << "/proc/" << pid << "/cmdline";
                f.open (path.str().c_str(), ios::in);
                if (!f.is_open()) {
                        // No file, so not running.
                        return str;
                }
                getline (f, str, '\n');
                f.close();
        }
        return str;
}

int
wml::FoundryUtilities::getPid (const std::string& programName)
{
        // From processes, work out if any named processes are
        // running. Return pid of running OSK. If there is none, return -1;

        // Open all subdirectories in /proc which have only numerals
        // in their name within these, get the first line: Name: whatever
        // If the name is programName, then get the Pid line, and return that.

        int pid = -1;

        if (programName.empty()) {
                return pid;
        }

        if (programName.size() > 15) {
                throw runtime_error ("FoundryUtilities::getPid() Use first 15 chars of program name only");
        }

        vector<string> dirs;
        FoundryUtilities::readProcDirs (dirs, "/proc", "");

        vector<string>::iterator i = dirs.begin();
        bool gotPid = false;
        while (gotPid == false && i != dirs.end()) {
                // Looping through each directory in /proc
                string dpath = *i;
                if (dpath.find ("status", 0) != string::npos) {
                        // This path is a status file, open and read it
                        stringstream path;
                        path << "/proc/" << dpath;
                        ifstream f;
                        f.open (path.str().c_str(), ios::in);
                        if (!f.is_open()) {
                                // No file, so not running.
                                i++;
                                continue;
                        }
                        string pidline, name;
                        getline (f, name, '\n'); // First line is the name
                        getline (f, pidline, '\n'); // Second line is the State line.
                        getline (f, pidline, '\n'); // Third line is Tgid.
                        getline (f, pidline, '\n'); // Fourth line is Pid
                        f.close();
                        // Analyse
                        if (name.find (programName, 0) != string::npos) {
                                stringstream pp;
                                pp << pidline.substr(5);
                                pp >> pid;
                                gotPid = true;
                                break;
                        } else {
                                // Not the right process
                        }
                } else {
                        // dpath is not a status file
                }
                i++;
        }

        return pid;
}

int
wml::FoundryUtilities::termKill (const string& programName, int& pid)
{
        if (pid != -1) {

                // First, send SIGTERM
                if (kill (pid, SIGTERM) != 0) {
                        // Failed to send the SIGTERM signal
                        return -1;
                }

                // Now see if that worked
                pid = FoundryUtilities::getPid (programName);
                if (pid != -1) {

                        // Despite accepting SIGTERM signal, process is still running. Try SIGKILL.
                        if (kill (pid, SIGKILL) != 0) {
                                return -1;
                        }

                        pid = FoundryUtilities::getPid (programName);
                        if (pid != -1) {
                                // Despite accepting SIGKILL, process is still running (bad)
                                return -1;
                        }

                }
        }

        return 0;
}

void
wml::FoundryUtilities::readProcDirs (vector<string>& vec,
                                     const char* baseDirPath,
                                     const char* subDirPath)
{
        DIR* d;
        struct dirent *ep;
        size_t entry_len = 0;

        string dirPath ("");
        string bd (baseDirPath);
        string sd (subDirPath);
        dirPath = bd + "/" + sd;

        if (!(d = opendir (dirPath.c_str()))) {
                // Just return.
                return;
        }

        while ((ep = readdir (d))) {
                unsigned char fileType = ep->d_type;
                string fileName = dirPath + "/" + (string)ep->d_name;

                if (fileType == DT_DIR) {

                        // Skip "." and ".." directories
                        if ( ((entry_len = strlen (ep->d_name)) > 0 && ep->d_name[0] == '.') &&
                             (ep->d_name[1] == '\0' || ep->d_name[1] == '.') ) {
                                continue;
                        }

                        // For process directories, recurse.
                        string newPath;
                        if (sd.size() == 0) {
                                newPath = ep->d_name;
                        } else {
                                newPath = sd + "/" + ep->d_name;
                        }

                        string tester(ep->d_name);
                        if (FoundryUtilities::containsOnlyNumerals (tester)) {
                                FoundryUtilities::readProcDirs (vec, baseDirPath, newPath.c_str());
                        }
                } else if (fileType == DT_LNK) {
                        // Do nothing
                } else {
                        // status files (only) are added to the vector
                        string newEntry;
                        if (sd.size() == 0) {
                                newEntry = ep->d_name;
                        } else {
                                newEntry = sd + "/" + ep->d_name;
                        }
                        if (newEntry.find ("status", 0) != string::npos) {
                                vec.push_back (newEntry);
                        }
                }
        }
        (void) closedir (d);
}

std::string
wml::FoundryUtilities::getMacAddr (void)
{
        char mac[32] = "";
        struct ifreq ifr;
        int sd;

        /* Set up network socket to get mac address */
        strcpy(ifr.ifr_name, "eth0");

        sd = socket(AF_INET, SOCK_DGRAM, 0);

        if (sd == -1) {
                throw runtime_error ("Error in call to socket()");
        }

        if (ioctl (sd, SIOCGIFHWADDR, &ifr) < 0) {
                throw runtime_error ("Error in call to ioctl()");

        } else {

                sprintf (mac, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
                         ifr.ifr_hwaddr.sa_data[0]&0xff,
                         ifr.ifr_hwaddr.sa_data[1]&0xff,
                         ifr.ifr_hwaddr.sa_data[2]&0xff,
                         ifr.ifr_hwaddr.sa_data[3]&0xff,
                         ifr.ifr_hwaddr.sa_data[4]&0xff,
                         ifr.ifr_hwaddr.sa_data[5]&0xff);
        }

        close (sd);

        string theMac (mac);

        return theMac;
}

void
wml::FoundryUtilities::getMacAddr (unsigned int* mac)
{
        struct ifreq ifr;
        int sd;

        /* Set up network socket to get mac address */
        strcpy(ifr.ifr_name, "eth0");

        sd = socket(AF_INET, SOCK_DGRAM, 0);

        if (sd == -1) {
                throw runtime_error ("Error in call to socket()");
        }

        if (ioctl (sd, SIOCGIFHWADDR, &ifr) < 0) {
                throw runtime_error ("Error in call to ioctl()");

        } else {
                mac[1] = (((ifr.ifr_hwaddr.sa_data[0]<<8) & 0x00000000ff00) |
                          ((ifr.ifr_hwaddr.sa_data[1])    & 0x0000000000ff)   );

                mac[0] = (((ifr.ifr_hwaddr.sa_data[2]<<24) & 0x0000ff000000) |
                          ((ifr.ifr_hwaddr.sa_data[3]<<16) & 0x000000ff0000) |
                          ((ifr.ifr_hwaddr.sa_data[4]<<8)  & 0x00000000ff00) |
                          ((ifr.ifr_hwaddr.sa_data[5])     & 0x0000000000ff)   );
        }

        close (sd);
}

void
wml::FoundryUtilities::strToMacAddr (const std::string& macStr, unsigned int* mac)
{
        // Initialise mac
        mac[0] = 0;
        mac[1] = 0;

        unsigned int i = 0;
        unsigned int n = 0;
        string::size_type posn = macStr.find (":", 0);
        string nibble ("");
        while (posn != string::npos) {
                nibble = macStr.substr (posn-2, 2);
                stringstream ss;
                ss << nibble;
                ss << hex;
                ss >> n;
                if (i < 2) {
                        // then we're reading first two nibbles of data
                        if (i==0) {
                                mac[1] |= (n&0xff) << 8;
                        } else {
                                mac[1] |= n&0xff;
                        }
                } else {
                        int shift = 5-i;
                        if (shift<0) {
                                throw runtime_error ("error; examine getMacAddr(string& unsigned int*)");
                        }
                        mac[0] |= (n&0xff) << (shift<<3);
                }
                i++;
                posn = macStr.find (":", posn+1);
        }
        posn = macStr.find_last_of (":");
        nibble = macStr.substr (posn+1, 2);
        stringstream sss;
        sss << nibble;
        sss << hex;
        sss >> n;
        mac[0] |= (n&0xff);
}

std::string
wml::FoundryUtilities::macAddrToStr (const unsigned int* mac)
{
        stringstream ss;
        int i = 0;
        // macHi
        for (i=1; i>=0; i--) {
                ss << hex;
                ss.fill ('0');
                if (i<1) { ss << ":"; }
                ss.width (2);
                ss << ((mac[1] >> (i<<3)) & 0xff); // i<<3 == i*8
        }
        // macLo
        for (i=3; i>=0; i--) {
                ss << ":";
                ss << hex;
                ss.width (2);
                ss.fill ('0');
                ss << ((mac[0] >> (i<<3)) & 0xff); // i<<3 == i*8
        }
        return ss.str();
}

void
wml::FoundryUtilities::clearoutDir (const string dirPath,
                                    const unsigned int olderThanSeconds,
                                    const string filePart)
{
        vector<string> files;
        try {
                FoundryUtilities::readDirectoryTree (files, dirPath, olderThanSeconds);
        } catch (const exception& e) {
                DBG ("Failed to read dir tree: " << e.what());
                return;
        }
        vector<string>::iterator i = files.begin();
        while (i != files.end()) {
                string fpath = dirPath + "/" + *i;
                try {
                        if (filePart.empty()) {
                                FoundryUtilities::unlinkFile (fpath);
                        } else {
                                // Must find filePart to unlink
                                if (i->find (filePart, 0) != string::npos) {
                                        FoundryUtilities::unlinkFile (fpath);
                                } // else do nothing
                        }

                } catch (const exception& e) {
                        DBG ("Failed to unlink " << *i << ": " << e.what());
                }
                ++i;
        }
}

void
wml::FoundryUtilities::readDirectoryTree (vector<string>& vec,
                                          const string dirPath,
                                          const unsigned int olderThanSeconds)
{
        FoundryUtilities::readDirectoryTree (vec, dirPath.c_str(), "", olderThanSeconds);
}

void
wml::FoundryUtilities::readDirectoryTree (vector<string>& vec,
                                          const char* baseDirPath,
                                          const char* subDirPath,
                                          const unsigned int olderThanSeconds)
{
        DIR* d;
        struct dirent *ep;
        size_t entry_len = 0;

        string dirPath (baseDirPath);
        string sd (subDirPath);
        if (!sd.empty()) {
                dirPath += "/" + sd;
        }

        if (!(d = opendir (dirPath.c_str()))) {
                string msg = "Failed to open directory " + dirPath;
                throw runtime_error (msg);
        }

        struct stat buf;
        while ((ep = readdir (d))) {

                unsigned char fileType;
                string fileName = dirPath + "/" + (string)ep->d_name;

                if (ep->d_type == DT_LNK) {
                        // Is it a link to a directory or a file?
                        struct stat * buf = NULL;
                        buf = (struct stat*) malloc (sizeof (struct stat));
                        if (!buf) { // Malloc error.
                                throw runtime_error ("Failed to malloc buf; "
                                                     "could not stat link " + fileName);
                        }
                        memset (buf, 0, sizeof(struct stat));
                        if (stat (fileName.c_str(), buf)) {
                                throw runtime_error ("Failed to stat link " + fileName);
                        } else {
                                if (S_ISREG(buf->st_mode)) {
                                        fileType = DT_REG;
                                } else if (S_ISDIR(buf->st_mode)) {
                                        fileType = DT_DIR;
                                } else {
                                        fileType = DT_UNKNOWN;
                                }
                        }
                        if (buf) { free (buf); }
                } else {
                        fileType = ep->d_type;
                }

                if (fileType == DT_DIR) {

                        // Skip "." and ".." directories
                        if ( ((entry_len = strlen (ep->d_name)) > 0 && ep->d_name[0] == '.') &&
                             (ep->d_name[1] == '\0' || ep->d_name[1] == '.') ) {
                                continue;
                        }

                        // For all other directories, recurse.
                        string newPath;
                        if (sd.size() == 0) {
                                newPath = ep->d_name;
                        } else {
                                newPath = sd + "/" + ep->d_name;
                        }
                        FoundryUtilities::readDirectoryTree (vec, baseDirPath,
                                                             newPath.c_str(), olderThanSeconds);
                } else {
                        // Non-directories are simply added to the vector
                        string newEntry;
                        if (sd.size() == 0) {
                                newEntry = ep->d_name;
                        } else {
                                newEntry = sd + "/" + ep->d_name;
                        }

                        // If we have to check the file age, do so here before the vec.push_back()
                        if (olderThanSeconds > 0) {
                                // Stat the file
                                memset (&buf, 0, sizeof (struct stat));

                                if (stat (fileName.c_str(), &buf)) {
                                        // no file to stat
                                        DBG ("stat() error for '" << fileName << "'");
                                        continue;
                                }

                                if (static_cast<unsigned int>(time(NULL)) - buf.st_mtime
                                    <= olderThanSeconds) {
                                        // The age of the last modification is less
                                        // than olderThanSeconds, so skip
                                        // (we're only returning the OLDER
                                        // files)
                                        DBG ("File " << fileName
                                             << " is too new to include, continuing");
                                        continue;
                                } else {
                                        DBG ("File " << fileName << " is older than "
                                             << olderThanSeconds << " s");
                                }
                        }
                        vec.push_back (newEntry);
                }
        }

        (void) closedir (d);
}

void
wml::FoundryUtilities::readDirectoryDirs (std::set<std::string>& dset,
                                          const std::string dirPath)
{
        DIR* d;
        struct dirent *ep;
        size_t entry_len = 0;

        if (!(d = opendir (dirPath.c_str()))) {
                string msg = "Failed to open directory " + dirPath;
                throw runtime_error (msg);
        }

        while ((ep = readdir (d))) {

                if (ep->d_type == DT_DIR) {

                        // Skip "." and ".." directories
                        if ( ((entry_len = strlen (ep->d_name)) > 0 && ep->d_name[0] == '.') &&
                             (ep->d_name[1] == '\0' || ep->d_name[1] == '.') ) {
                                continue;
                        }

                        // All other directories are added to vec
                        dset.insert (ep->d_name);
                }
        }

        (void) closedir (d);
}

void
wml::FoundryUtilities::readDirectoryEmptyDirs (std::set<std::string>& dset,
                                               const std::string& baseDirPath,
                                               const std::string& subDir)
{
        DBG ("Called for baseDirPath '" << baseDirPath << "' and subDir '" << subDir << "'");
        DIR* d;
        struct dirent *ep;
        size_t entry_len = 0;

        string dirPath (baseDirPath);
        if (!subDir.empty()) {
                dirPath += "/" + subDir;
        }

        if (!(d = opendir (dirPath.c_str()))) {
                string msg = "Failed to open directory " + dirPath;
                throw runtime_error (msg);
        }

        unsigned int levelDirCount = 0;
        while ((ep = readdir (d))) {

                if (ep->d_type == DT_DIR) {
                        // Skip "." and ".." directories
                        if ( ((entry_len = strlen (ep->d_name)) > 0 && ep->d_name[0] == '.') &&
                             (ep->d_name[1] == '\0' || ep->d_name[1] == '.') ) {
                                continue;
                        }

                        ++levelDirCount;
                        // Because we found a directory, this current
                        // directory ain't empty - recurse with a new
                        // directory in the subDir path:
                        string newSubDir;
                        if (subDir.empty()) {
                                newSubDir = (const char*)ep->d_name;
                        } else {
                                newSubDir = subDir + "/" + (const char*)ep->d_name;
                        }
                        FoundryUtilities::readDirectoryEmptyDirs (dset, baseDirPath, newSubDir);
                }
        }

        if (levelDirCount == 0) {
                // No directories found here, check for files
                vector<string> foundfiles;
                FoundryUtilities::readDirectoryTree (foundfiles, dirPath);
                DBG ("readDirectoryTree() found " << foundfiles.size() << " files in " << dirPath);

                if (foundfiles.empty()) {
                        DBG ("INSERT " << subDir
                             << " as " << dirPath << " contains no files or dirs");
                        dset.insert (subDir);
                } else {
                        DBG ("NOT adding " << subDir
                             << " as " << dirPath << " contains " << foundfiles.size() << " files");
                }
        }

        (void) closedir (d);
}

void
wml::FoundryUtilities::removeUnusedDirs (std::set<std::string>& dset,
                                         const std::string& dirPath)
{
        set<string> onepass;
        do {
                onepass.clear();
                FoundryUtilities::removeEmptySubDirs (onepass, dirPath);
                dset.insert (onepass.begin(), onepass.end());
        } while (!onepass.empty());
}

void
wml::FoundryUtilities::removeEmptySubDirs (std::set<std::string>& dset,
                                           const std::string& baseDirPath,
                                           const std::string& subDir)
{
        DBG ("Called for baseDirPath '" << baseDirPath << "' and subDir '" << subDir << "'");
        DIR* d;
        struct dirent *ep;
        size_t entry_len = 0;

        string dirPath (baseDirPath);
        if (!subDir.empty()) {
                dirPath += "/" + subDir;
        }

        if (!(d = opendir (dirPath.c_str()))) {
                string msg = "Failed to open directory " + dirPath;
                throw runtime_error (msg);
        }

        unsigned int levelDirCount = 0;
        while ((ep = readdir (d))) {

                if (ep->d_type == DT_DIR) {
                        // Skip "." and ".." directories
                        if ( ((entry_len = strlen (ep->d_name)) > 0 && ep->d_name[0] == '.') &&
                             (ep->d_name[1] == '\0' || ep->d_name[1] == '.') ) {
                                continue;
                        }

                        ++levelDirCount;
                        // Because we found a directory, this current
                        // directory ain't empty - recurse with a new
                        // directory in the subDir path:
                        string newSubDir;
                        if (subDir.empty()) {
                                newSubDir = (const char*)ep->d_name;
                        } else {
                                newSubDir = subDir + "/" + (const char*)ep->d_name;
                        }
                        FoundryUtilities::removeEmptySubDirs (dset, baseDirPath, newSubDir);
                }
        }

        if (levelDirCount == 0) {
                // No directories found here, check for files
                vector<string> foundfiles;
                FoundryUtilities::readDirectoryTree (foundfiles, dirPath);
                DBG ("readDirectoryTree() found " << foundfiles.size() << " files in " << dirPath);

                if (foundfiles.empty()) {
                        if (subDir.empty()) {
                                DBG ("Not removing baseDirPath");
                        } else {
                                DBG ("RMDIR " << subDir
                                     << " as " << dirPath << " contains no files or dirs");
                                FoundryUtilities::removeDir (dirPath);
                                dset.insert (subDir);
                        }
                } else {
                        DBG ("NOT Removing " << dirPath << " which contains " << foundfiles.size() << " files");
                }
        }

        (void) closedir (d);
}

std::string
wml::FoundryUtilities::readHostname (void)
{
        ifstream f;
        string hname("(unknown)");
        f.open ("/etc/hostname", ios::in);
        if (f.is_open()) {
                getline (f, hname, '\n');
                f.close();
        }
        return hname;
}

unsigned int
wml::FoundryUtilities::yearNow (void)
{
        time_t curtime; // Current time
        struct tm * t;
        curtime = time(NULL);
        t = localtime (&curtime);
        unsigned int theYear = static_cast<unsigned int>(t->tm_year+1900);
        return theYear;
}

unsigned int
wml::FoundryUtilities::monthNow (void)
{
        time_t curtime; // Current time
        struct tm * t;
        curtime = time(NULL);
        t = localtime (&curtime);
        unsigned int theMonth = static_cast<unsigned int>(t->tm_mon+1);
        return theMonth;
}

unsigned int
wml::FoundryUtilities::dateNow (void)
{
        time_t curtime; // Current time
        struct tm * t;
        curtime = time(NULL);
        t = localtime (&curtime);
        unsigned int theDate = static_cast<unsigned int>(t->tm_mday);
        return theDate;
}

string
wml::FoundryUtilities::monthStr (const int month, const bool shortFormat)
{
        string rtn("");

        if (shortFormat == true) {
                switch (month) {
                case 1:
                        rtn = "Jan";
                        break;
                case 2:
                        rtn = "Feb";
                        break;
                case 3:
                        rtn = "Mar";
                        break;
                case 4:
                        rtn = "Apr";
                        break;
                case 5:
                        rtn = "May";
                        break;
                case 6:
                        rtn = "Jun";
                        break;
                case 7:
                        rtn = "Jul";
                        break;
                case 8:
                        rtn = "Aug";
                        break;
                case 9:
                        rtn = "Sep";
                        break;
                case 10:
                        rtn = "Oct";
                        break;
                case 11:
                        rtn = "Nov";
                        break;
                case 12:
                        rtn = "Dec";
                        break;
                default:
                        rtn = "unk";
                        break;
                }
        } else {
                switch (month) {
                case 1:
                        rtn = "January";
                        break;
                case 2:
                        rtn = "February";
                        break;
                case 3:
                        rtn = "March";
                        break;
                case 4:
                        rtn = "April";
                        break;
                case 5:
                        rtn = "May";
                        break;
                case 6:
                        rtn = "June";
                        break;
                case 7:
                        rtn = "July";
                        break;
                case 8:
                        rtn = "August";
                        break;
                case 9:
                        rtn = "September";
                        break;
                case 10:
                        rtn = "October";
                        break;
                case 11:
                        rtn = "November";
                        break;
                case 12:
                        rtn = "December";
                        break;
                default:
                        rtn = "unknown";
                        break;
                }
        }

        return rtn;
}

time_t
wml::FoundryUtilities::dateToNum (const std::string& dateStr)
{
        char separator = '\0';

        if (dateStr.empty()) {
                return -2;
        }

        if (dateStr.size() < 8) {
                return -3;
        }

        bool bigEndian (true);

        if (dateStr[2] < '0'
            || dateStr[2] > '9') {
                separator = dateStr[2];
                bigEndian = false;
        } else if (dateStr[4] < '0'
                   || dateStr[4] > '9') {
                separator = dateStr[4];
        }
        if (separator != '\0' && dateStr.size() < 10) {
                return -4;
        }

        string year;
        string month;
        string day;
        unsigned int yearN=0, monthN=0, dayN=0;

        if (bigEndian) {
                year = dateStr.substr (0,4);

                if (separator == '\0') {
                        month = dateStr.substr (4,2);
                        day = dateStr.substr (6,2);
                } else {
                        month = dateStr.substr (5,2);
                        day = dateStr.substr (8,2);
                }

        } else {
                day = dateStr.substr (0,2);

                if (separator == '\0') {
                        month = dateStr.substr (2,2);
                        year = dateStr.substr (4,4);
                } else {
                        month = dateStr.substr (3,2);
                        year = dateStr.substr (6,4);
                }
        }

        stringstream yearss, monthss, dayss;
        yearss << year;
        yearss.width(4);
        yearss.fill ('0');
        yearss >> yearN;

        monthss << month;
        monthss.width(2);
        monthss.fill ('0');
        monthss >> monthN;

        dayss << day;
        dayss.width(2);
        dayss.fill ('0');
        dayss >> dayN;

        struct tm * t;
        t = (struct tm*) malloc (sizeof (struct tm));
        t->tm_year = yearN-1900;
        t->tm_mon = monthN-1;
        t->tm_mday = dayN;
        t->tm_hour = 0;
        t->tm_min = 0;
        t->tm_sec = 0;
        t->tm_isdst = -1;
        time_t rtnTime = mktime (t);
        if (rtnTime == -1) {
                throw runtime_error ("mktime() returned -1");
        }
        free (t);

        return rtnTime;
}

time_t
wml::FoundryUtilities::dateTimeToNum (std::string dateTimeStr)
{
        char dateSeparator = '\0';
        char timeSeparator = '\0';

        if (dateTimeStr.empty()) {
                return -2;
        }

        if (dateTimeStr.size() < 8) {
                return -3;
        }

        if (dateTimeStr[4] < '0'
            || dateTimeStr[4] > '9') {
                dateSeparator = dateTimeStr[4];
                if (dateTimeStr.size() < 10) {
                        return -4;
                }
        }

        string year;
        string month;
        string day;
        unsigned int yearN=0, monthN=0, dayN=0;

        year = dateTimeStr.substr (0,4);

        if (dateSeparator == '\0') {
                month = dateTimeStr.substr (4,2);
                day = dateTimeStr.substr (6,2);
        } else {
                month = dateTimeStr.substr (5,2);
                day = dateTimeStr.substr (8,2);
        }

        stringstream yearss, monthss, dayss;
        yearss << year;
        yearss.width(4);
        yearss.fill ('0');
        yearss >> yearN;

        monthss << month;
        monthss.width(2);
        monthss.fill ('0');
        monthss >> monthN;

        dayss << day;
        dayss.width(2);
        dayss.fill ('0');
        dayss >> dayN;

        string hour;
        string min;
        string sec;
        unsigned int hourN=0, minN=0, secN=0;

        string::size_type spacePos = dateTimeStr.find (" ", 0);
        if (spacePos != string::npos) {
                if (dateTimeStr[spacePos+3] < '0'
                    || dateTimeStr[spacePos+3] > '9') {
                        timeSeparator = dateTimeStr[spacePos+3];
                }

                hour = dateTimeStr.substr (spacePos+1, 2);

                if (timeSeparator != '\0') {
                        min = dateTimeStr.substr (spacePos+4, 2);
                        sec = dateTimeStr.substr (spacePos+7, 2);
                } else {
                        min = dateTimeStr.substr (spacePos+3, 2);
                        sec = dateTimeStr.substr (spacePos+5, 2);
                }

                //DBG ("hour: " << hour << " min: " << min << " sec: " << sec);

                stringstream hourss, minss, secss;
                hourss << hour;
                hourss.width(2);
                hourss.fill ('0');
                hourss >> hourN;

                minss << min;
                minss.width(2);
                minss.fill ('0');
                minss >> minN;

                secss << sec;
                secss.width(2);
                secss.fill ('0');
                secss >> secN;
        }

        struct tm * t;
        t = (struct tm*) malloc (sizeof (struct tm));
        t->tm_year = yearN-1900;
        t->tm_mon = monthN-1;
        t->tm_mday = dayN;
        t->tm_hour = hourN;
        t->tm_min = minN;
        t->tm_sec = secN;
        t->tm_isdst = -1;
        time_t rtnTime = mktime (t);
        if (rtnTime == -1) {
                throw runtime_error ("mktime() returned -1");
        }
        free (t);

        return rtnTime;
}

std::string
wml::FoundryUtilities::numToDateTime (const time_t epochSeconds,
                                      const char dateSeparator,
                                      const char timeSeparator)
{
        if (epochSeconds == 0) {
                return "unknown";
        }

        struct tm * t;
        time_t es = epochSeconds;
        t = (struct tm*) malloc (sizeof (struct tm));
        t = localtime_r (&es, t);
        int theDay = t->tm_mday;
        int theMonth = t->tm_mon+1;
        int theYear = t->tm_year+1900;
        int theHour = t->tm_hour;
        int theMin = t->tm_min;
        int theSec = t->tm_sec;
        free (t);

        stringstream rtn;

        // Date part
        rtn.width(4);
        rtn.fill('0');
        rtn << theYear;
        if (dateSeparator != '\0') {
                rtn << dateSeparator;
        }
        rtn.width(2);
        rtn.fill('0');
        rtn << theMonth;
        if (dateSeparator != '\0') {
                rtn << dateSeparator;
        }
        rtn.width(2);
        rtn.fill('0');
        rtn << theDay;

        rtn << " ";

        // Time part
        rtn.width(2);
        rtn.fill('0');
        rtn << theHour;
        if (timeSeparator != '\0') {
                rtn << timeSeparator;
        }
        rtn.width(2);
        rtn.fill('0');
        rtn << theMin;
        if (timeSeparator != '\0') {
                rtn << timeSeparator;
        }
        rtn.width(2);
        rtn.fill('0');
        rtn << theSec;

        return rtn.str();
}

std::string
wml::FoundryUtilities::numToDate (const time_t epochSeconds,
                                  const char separator)
{
        struct tm * t;
        time_t es = epochSeconds;
        t = (struct tm*) malloc (sizeof (struct tm));
        t = localtime_r (&es, t);
        int theDay = t->tm_mday;
        int theMonth = t->tm_mon+1;
        int theYear = t->tm_year+1900;
        free (t);

        stringstream rtn;
        if (separator == '\0') {
                rtn.width(4);
                rtn.fill('0');
                rtn << theYear;
                rtn.width(2);
                rtn.fill('0');
                rtn << theMonth;
                rtn.width(2);
                rtn.fill('0');
                rtn << theDay;
        } else {
                rtn.width(4);
                rtn.fill('0');
                rtn << theYear << separator;
                rtn.width(2);
                rtn.fill('0');
                rtn << theMonth << separator;
                rtn.width(2);
                rtn.fill('0');
                rtn << theDay;
        }

        return rtn.str();
}

string
wml::FoundryUtilities::timeNow (void)
{
        time_t curtime;
        struct tm *loctime;
        curtime = time (NULL);
        loctime = localtime (&curtime);
        return asctime(loctime);
}

unsigned int
wml::FoundryUtilities::getMonthFromLog (const std::string& filePath,
                                        const unsigned int lineNum)
{
        unsigned int month = 0;

        ifstream f;
        f.open (filePath.c_str(), ios::in);
        if (!f.is_open()) {
                stringstream msg;
                msg << "getMonthFromLog(): Can't open '"
                    << filePath << "' for reading.";
                throw runtime_error (msg.str());
        }

        unsigned int curLine = 0;
        string line;
        while (getline (f, line, '\n')) {
                curLine++;
                if (curLine < lineNum) {
                        continue;
                }
                if (curLine > lineNum) {
                        break;
                }
                // Now read in the date from this syslog line.
                // Lines start like this: Feb  6 17:37:41
                string monthStr = line.substr (0, 3);
                if (monthStr == "Jan") {
                        month = 1;
                } else if (monthStr == "Feb") {
                        month = 2;
                } else if (monthStr == "Mar") {
                        month = 3;
                } else if (monthStr == "Apr") {
                        month = 4;
                } else if (monthStr == "May") {
                        month = 5;
                } else if (monthStr == "Jun") {
                        month = 6;
                } else if (monthStr == "Jul") {
                        month = 7;
                } else if (monthStr == "Aug") {
                        month = 8;
                } else if (monthStr == "Sep") {
                        month = 9;
                } else if (monthStr == "Oct") {
                        month = 10;
                } else if (monthStr == "Nov") {
                        month = 11;
                } else if (monthStr == "Dec") {
                        month = 12;
                } else {
                        stringstream msg;
                        msg << "getMonthFromLog(): Can't get month from line: '"
                            << line << "'";
                        throw runtime_error (msg.str());
                }
        }

        f.close();

        return month;
}

void
wml::FoundryUtilities::getCSS (std::stringstream& rCSS,
                               const std::string cssFile,
                               const bool inlineOutput)
{
        FoundryUtilities::getScript (SCRIPT_CSS, rCSS, cssFile, inlineOutput);
}

void
wml::FoundryUtilities::getJavascript (std::stringstream& rJavascript,
                                      std::string jsFile,
                                      const bool inlineOutput)
{
        if (inlineOutput == true
            && FoundryUtilities::dirExists ("/etc/wml/js/")){
                string::size_type pos = jsFile.find_last_of ("/");
                string jsFileName = jsFile;
                if (pos != string::npos) {
                        jsFileName = jsFile.substr(pos+1);
                }
                jsFileName = "/etc/wml/js/" + jsFileName;
                if (FoundryUtilities::fileExists (jsFileName)) {
                        jsFile = jsFileName;
                }

        } else {
                // Nothing. jsFile is either js/somefile.js or
                // /httpd/js/somefile.js. getScript should strip off
                // any /httpd prefix, then add it back in if necessary
                // (which depends on the value of inlineOutput).
        }
        FoundryUtilities::getScript (SCRIPT_JAVASCRIPT, rJavascript, jsFile, inlineOutput);
}

void
wml::FoundryUtilities::getScript (const SCRIPT_TYPE script,
                                  std::stringstream& rScript,
                                  std::string scriptFile,
                                  const bool inlineOutput)
{
        if (inlineOutput == true) {

                // inlineOutput is true, we're reading the file in
                // from the filesystem to output into the page.

                string::size_type pos = scriptFile.find("/etc/wml");
                if (pos == string::npos) {
                        pos = scriptFile.find("/httpd/");
                        if (pos == string::npos) {
                                // no /httpd/ in scriptFile, prefix it...
                                scriptFile = "/httpd/" + scriptFile;
                        } else if (pos != 0) {
                                // scriptFile HAS /httpd/ in it, but not at start, add it in...
                                scriptFile = "/httpd/" + scriptFile;
                        } else {
                                // /httpd/ present and at start, no need to modify scriptFile.
                        }
                }

                ifstream f;
                f.open (scriptFile.c_str(), ios::in);

                if (f.is_open()) {
                        string openingTag;
                        string closingTag;
                        switch (script) {
                        case SCRIPT_JAVASCRIPT:
                                openingTag = "<script type=\"text/javascript\" >//<![CDATA[";
                                closingTag = "//]]></script>";
                                break;
                        case SCRIPT_CSS:
                                openingTag = "<style type=\"text/css\" ><!--/*--><![CDATA[/*><!--*/";
                                closingTag = "/*]]>*/--></style>";
                                break;
                        default:
                                openingTag = "<!--";
                                closingTag = "-->";
                        }

                        // Add the text which tells the browser this is script:
                        rScript << "<!-- " << scriptFile << " -->" << openingTag << endl;

                        string line;
                        while (getline (f, line, '\n')) {
                                rScript << line << endl;
                        }

                        rScript << closingTag << endl;

                        f.close();
                } else {
                        rScript << "<!-- Could not open " << scriptFile << " -->" << endl;
                }
        } else {

                // We're just going to place a <script type=... tag
                // into the javascript, which should NOT have the
                // /httpd prefix to the path - as far as the client
                // browser is concerned, javascript files are in
                // address/js/somefile.js and css in
                // address/css/somefile.css.
                string::size_type pos = scriptFile.find("/httpd/");
                if (pos != string::npos && pos == 0) {
                        // We have /httpd/ at the start of the scriptFile, so remove it.
                        scriptFile = scriptFile.substr (7); // 7 is length of "/httpd/"
                }

                string openingTag;
                string closingTag;
                switch (script) {
                case SCRIPT_JAVASCRIPT:
                        openingTag = "<script type=\"text/javascript\" src=\"";
                        closingTag = "\"></script>";
                        break;
                case SCRIPT_CSS:
                        openingTag = "<link type=\"text/css\" rel=\"stylesheet\" href=\"";
                        closingTag = "\" />";
                        break;
                default:
                        openingTag = "<!--";
                        closingTag = "-->";
                }
                rScript << openingTag << scriptFile << closingTag << endl;
        }
}

void
wml::FoundryUtilities::unicodeize (std::string& str)
{
        stringstream utf8;

        // For each character in str, turn it into its unicode
        // representation.
        for (unsigned int i=0; i<str.size(); i++) {

                unsigned int n;
                n = static_cast<unsigned int>(str[i]);

                utf8 << "&#x";
                utf8.width(4);
                utf8.fill('0');
                utf8 << hex;
                utf8 << n;
                utf8 << ';';
        }

        str = utf8.str();
}

bool
wml::FoundryUtilities::containsOnlyNumerals (const std::string& str)
{
        for (unsigned int i=0; i<str.size(); i++) {
                if (static_cast<unsigned int>(str[i]) < 48 ||
                    static_cast<unsigned int>(str[i]) > 57) {
                        // Not ascii '0' to '9'
                        return false;
                }
        }
        // NB: An EMPTY string contains NO numerals, but we return true in that case.
        return true;
}

void
wml::FoundryUtilities::sanitize (std::string& str,
                                 const char* allowed,
                                 const bool eraseForbidden)
{
        string allowedStr(allowed);
        FoundryUtilities::sanitize (str, allowedStr, eraseForbidden);
}

void
wml::FoundryUtilities::sanitize (std::string& str,
                                 const std::string& allowed,
                                 const bool eraseForbidden)
{
        unsigned int i=0;
        while (i<str.size()) {
                if (allowed.find(str[i], 0) == string::npos) {
                        // str[i] is forbidden
                        if (eraseForbidden == true) {
                                str.erase (i,1);
                        } else {
                                stringstream ss;
                                ss << "Forbidden char '" << str[i]
                                   << "' found while sanitising input.";
                                throw runtime_error (ss.str());
                        }
                } else {
                        i++;
                }
        }
}

void
wml::FoundryUtilities::sanitizeReplace (std::string& str,
                                        const std::string allowed,
                                        const char replaceChar)
{
        unsigned int i=0;
        while (i<str.size()) {
                if (allowed.find(str[i], 0) == string::npos) {
                        // str[i] is forbidden
                        str[i] = replaceChar;
                }
                i++;
        }
}

void
wml::FoundryUtilities::coutFile (const char* filePath)
{
        ifstream f;
        f.open (filePath, ios::in);

        if (!f.is_open()) {
                stringstream msg;
                msg << "Couldn't open file '" << filePath << "'";
                throw runtime_error (msg.str());
        }

        string line;
        while (getline (f, line, '\n')) {
                cout << line << endl;
        }

        f.close();
}

void
wml::FoundryUtilities::coutFile (const std::string filePath)
{
        ifstream f;
        f.open (filePath.c_str(), ios::in);

        if (!f.is_open()) {
                stringstream msg;
                msg << "Couldn't open file '" << filePath << "'";
                throw runtime_error (msg.str());
        }

        string line;
        while (getline (f, line, '\n')) {
                cout << line << endl;
        }

        f.close();
}

int
wml::FoundryUtilities::fileSize (const string filePath)
{
        struct stat buf;
        memset (&buf, 0, sizeof(struct stat));
        if (stat (filePath.c_str(), &buf)) {
#ifdef DEBUG
                int the_error = errno;
                debuglog (LOG_ERR,
                          "%s: Failed to stat %s, errno=%d",
                          __FUNCTION__, filePath.c_str(), the_error);
                DBG ("Failed to stat " << filePath << ", errno=" << the_error);
#endif
                return 0;
        }
        int size = buf.st_size;
        return size;
}

int
wml::FoundryUtilities::incFileCount (const char * filePath)
{
        fstream f;
        f.open (filePath, ios::in);
        if (!f.is_open()) {
                return -1;
        }
        int n;
        f >> n;
        f.close();
        n++;
        f.open (filePath, ios::out|ios::trunc);
        if (!f.is_open()) {
                return -1;
        }
        f << n << endl;
        f.close();
        return n;
}

int
wml::FoundryUtilities::decFileCount (const char * filePath)
{
        fstream f;
        f.open (filePath, ios::in);
        if (!f.is_open()) {
                return -1;
        }
        int n;
        f >> n;
        f.close();
        n--;
        // This is intended for unsigned file counts only:
        if (n<0) { n=0; }
        f.open (filePath, ios::out|ios::trunc);
        if (!f.is_open()) {
                return -1;
        }
        f << n << endl;
        f.close();
        return n;
}

int
wml::FoundryUtilities::zeroFileCount (const char * filePath)
{
        FoundryUtilities::unlinkFile (filePath);
        fstream f;
        f.open (filePath, ios::out|ios::trunc);
        if (!f.is_open()) {
                return -1;
        }
        f << "0" << endl;
        f.close();
        return 0;
}

vector<Glib::ustring>
wml::FoundryUtilities::stringToWords (const Glib::ustring& s, bool ignoreTrailingEmptyVal)
{
        vector<Glib::ustring> theVec;
        Glib::ustring entry("");
        Glib::ustring separators ("");
        separators += static_cast<gunichar>(0x0020); // space
        separators += static_cast<gunichar>(0x00A0); // U+00A0  NO-BREAK SPACE
        separators += static_cast<gunichar>(0x1680); // U+1680 OGHAM SPACE MARK
        separators += static_cast<gunichar>(0x180E); // U+180E MONGOLIAN VOWEL SEPARATOR
        separators += static_cast<gunichar>(0x2000); // U+2000 EN QUAD
        separators += static_cast<gunichar>(0x2001); // U+2001 EM QUAD
        separators += static_cast<gunichar>(0x2002); // U+2002 EN SPACE
        separators += static_cast<gunichar>(0x2003); // U+2003 EM SPACE
        separators += static_cast<gunichar>(0x2004); // U+2004 THREE-PER-EM SPACE
        separators += static_cast<gunichar>(0x2005); // U+2005 FOUR-PER-EM SPACE
        separators += static_cast<gunichar>(0x2006); // U+2006 SIX-PER-EM SPACE
        separators += static_cast<gunichar>(0x2007); // U+2007 FIGURE SPACE
        separators += static_cast<gunichar>(0x2008); // U+2008 PUNCTUATION SPACE
        separators += static_cast<gunichar>(0x2009); // U+2009 THIN SPACE
        separators += static_cast<gunichar>(0x200A); // U+200A HAIR SPACE
        separators += static_cast<gunichar>(0x202F); // U+202F NARROW NO-BREAK SPACE
        separators += static_cast<gunichar>(0x205F); // U+205F MEDIUM MATHEMATICAL SPACE
        separators += static_cast<gunichar>(0x3000); // U+3000 IDEOGRAPHIC SPACE

        Glib::ustring::size_type a=0, b=0;
        while (a < s.size() && (b = s.find_first_of (separators, a)) != Glib::ustring::npos) {
                entry = s.substr (a, b-a);
                theVec.push_back (entry);
                Glib::ustring sep("");
                sep += s.at(b);
                a=b+sep.size();
        }
        // Last one has no separator
        if (a < s.size()) {
                b = s.size();
                entry = s.substr (a, b-a);
                theVec.push_back (entry);
        } else {
                if (!ignoreTrailingEmptyVal) {
                        theVec.push_back ("");
                }
        }
        return theVec;
}

vector<Glib::ustring>
wml::FoundryUtilities::stringToVector (const Glib::ustring& s,
                                       const Glib::ustring& separator,
                                       const bool ignoreTrailingEmptyVal)
{
        if (separator.empty()) {
                throw runtime_error ("Can't split the string; the separator is empty.");
        }
        vector<Glib::ustring> theVec;
        Glib::ustring entry("");
        Glib::ustring::size_type sepLen = separator.size();
        Glib::ustring::size_type a=0, b=0;
        while (a < s.size()
               && (b = s.find (separator, a)) != Glib::ustring::npos) {
                entry = s.substr (a, b-a);
                theVec.push_back (entry);
                a=b+sepLen;
        }
        // Last one has no separator
        if (a < s.size()) {
                b = s.size();
                entry = s.substr (a, b-a);
                theVec.push_back (entry);
        } else {
                if (!ignoreTrailingEmptyVal) {
                        theVec.push_back ("");
                }
        }

        return theVec;
}

// Similiar to FoundryUtilities::splitString() but FASTER.
vector<string>
wml::FoundryUtilities::stringToVector (const string& s, const string& separator,
                                       const bool ignoreTrailingEmptyVal)
{
        if (separator.empty()) {
                throw runtime_error ("Can't split the string; the separator is empty.");
        }
        vector<string> theVec;
        string entry("");
        string::size_type sepLen = separator.size();
        string::size_type a=0, b=0;
        while (a < s.size()
               && (b = s.find (separator, a)) != string::npos) {
                entry = s.substr (a, b-a);
                theVec.push_back (entry);
                a=b+sepLen;
        }
        // Last one has no separator
        if (a < s.size()) {
                b = s.size();
                entry = s.substr (a, b-a);
                theVec.push_back (entry);
        } else {
                if (!ignoreTrailingEmptyVal) {
                        theVec.push_back ("");
                }
        }

        return theVec;
}

// Similiar to FoundryUtilities::stringToVector()
void
wml::FoundryUtilities::splitString (vector<string>& tokens,
                                    const string& stringToSplit,
                                    const string& delim)
{
        string::size_type pos;
        pos = stringToSplit.find (delim);
        if (pos != string::npos) {
                tokens.push_back (stringToSplit.substr(0, pos));
                string::size_type newPos;
                newPos = pos + delim.size();
                string newString = stringToSplit.substr(newPos);
                FoundryUtilities::splitString (tokens, newString, delim);
        } else {
                tokens.push_back (stringToSplit);
        }
}

std::string
wml::FoundryUtilities::htmlHighlightTerm (const string& term,
                                          const string& searchTerms,
                                          const string& tag)
{
        string sTerm (term);
        string sTerms (searchTerms);
        transform (sTerms.begin(), sTerms.end(),
                   sTerms.begin(), wml::to_upper());
        string seps(";, "), encs("\"'");
        char escape = '\0';
        //const string& sTermsRef = sTerms;
        vector<string> searchTermsUC (wml::FoundryUtilities::splitStringWithEncs (sTerms, seps, encs, escape));
        vector<string> searchTermsUC_2 = wml::FoundryUtilities::splitStringWithEncs (sTerms);

        string sTag (tag);
        return FoundryUtilities::htmlHighlightTerm (sTerm, searchTermsUC, sTag);
}

std::string
wml::FoundryUtilities::htmlHighlightTerm (const std::string& term,
                                          const vector<string>& searchTermsUC,
                                          const std::string& tag)
{
        string rtn("");
        string termUC = term;
        string::size_type p1 = string::npos, p2 = string::npos;
        transform (termUC.begin(), termUC.end(), termUC.begin(), wml::to_upper());
        vector<string>::const_iterator i = searchTermsUC.begin();
        while (i != searchTermsUC.end()) {
                if ((p1 = termUC.find (*i, 0)) != string::npos) {
                        p2 = p1 + i->size() - 1;
                        break;
                }
                ++i;
        }
        string::size_type p = 0;
        if (p2 != string::npos && p1 != string::npos) {
                DBG2 ("Build highlighted search term");
                while (p < p1 && p < term.size()) {
                        DBG2 ("Adding " << term[p] << " to return string");
                        rtn += term[p];
                        ++p;
                }
                DBG2 ("Adding <tag> to return string");
                rtn += "<" + tag + ">";
                while (p <= p2 && p < term.size()) {
                        DBG2 ("Adding " << term[p] << " to return string");
                        rtn += term[p];
                        ++p;
                }
                DBG2 ("Adding </tag> to return string");
                rtn += "</" + tag + ">";
                while (p < term.size()) {
                        DBG2 ("Adding " << term[p] << " to return string");
                        rtn += term[p];
                        ++p;
                }
                DBG2 ("Finished return string");

        } else {
                DBG2 ("Return non-highlighted search term");
                rtn = term;
        }

        return rtn;
}

vector<string>
wml::FoundryUtilities::wrapLine (const std::string& line, unsigned int maxLength, char wrapAfter)
{
        string::size_type len (line.length()), pos = 0, pos1 = 0;
        vector<string> rtn;
        string tmp("");
        string separators (" "); // space
        if (wrapAfter != '\0') {
                separators += wrapAfter;
        }
        while (pos1 != string::npos) {
                DBG ("Pos: " << pos << ", pos1: " << pos1 << ", len: " << len);

                if (len - pos > maxLength) {
                        pos1 = line.find_last_of (separators, pos + maxLength);
                        if (pos1 < pos) {
                                // Likely found a string with no
                                // spaces and maxLength chars so increment
                                // allowable line length
                                DBG ("Breaking on a non-space/non-wrapping char");
                                pos1 = pos + maxLength;
                                tmp = line.substr(pos, pos1 - pos);
                                tmp += "\\"; // Add a "continues on next line" char
                                pos1--; // Because we don't have a space to skip
                        } else {
                                DBG ("Found a space/wrapping char at: " << pos1);
                                tmp = line.substr(pos, pos1 - pos + 1);
                        }
                } else {
                        pos1 = string::npos;
                        tmp = line.substr(pos, pos1 - pos);
                }
                rtn.push_back (tmp);

                pos = pos1 + 1;
        }
        return rtn;
}

string
wml::FoundryUtilities::vectorToString (const vector<string>& v,
                                       const string& separator)
{
        if (separator.empty()) {
                throw runtime_error ("Can't build the string; the separator is empty.");
        }
        stringstream ss;
        vector<string>::const_iterator i = v.begin();
        bool first(true);
        while (i != v.end()) {
                if (first) {
                        first = false;
                } else {
                        ss << separator;
                }
                ss << *i;
                ++i;
        }
        return ss.str();
}

std::vector<std::string>
wml::FoundryUtilities::csvToVector (const std::string& csvList, const char separator,
                                    const bool ignoreTrailingEmptyVal)
{
        vector<string> theVec;
        string csvl (csvList);
        string entry("");
        string::size_type a=0, b=0;
        while (a < csvl.size()
               && (b = csvl.find (separator, a)) != string::npos) {
                entry = csvl.substr (a, b-a);
                theVec.push_back (entry);
                a=b+1;
        }
        // Last one has no ','
        if (a < csvl.size()) {
                b = csvl.size();
                entry = csvl.substr (a, b-a);
                theVec.push_back (entry);
        } else {
                if (!ignoreTrailingEmptyVal) {
                        theVec.push_back ("");
                }
        }

        return theVec;
}

std::list<std::string>
wml::FoundryUtilities::csvToList (const std::string& csvList, const char separator)
{
        list<string> theList;
        string csvl (csvList);
        string entry("");
        string::size_type a=0, b=0;
        while (a < csvl.size()
               && (b = csvl.find (separator, a)) != string::npos) {
                entry = csvl.substr (a, b-a);
                theList.push_back (entry);
                a=b+1;
        }
        // Last one has no ','
        if (a < csvl.size()) {
                b = csvl.size();
                entry = csvl.substr (a, b-a);
                theList.push_back (entry);
        }

        return theList;
}

std::set<std::string>
wml::FoundryUtilities::csvToSet (const std::string& csvList, const char separator)
{
        set<string> theSet;
        string csvl (csvList);
        string entry("");
        string::size_type a=0, b=0;
        while (a < csvl.size()
               && (b = csvl.find (separator, a)) != string::npos) {
                entry = csvl.substr (a, b-a);
                theSet.insert (entry);
                a=b+1;
        }
        // Last one has no ','
        if (a < csvl.size()) {
                b = csvl.size();
                entry = csvl.substr (a, b-a);
                theSet.insert (entry);
        }

        return theSet;
}

std::string
wml::FoundryUtilities::vectorToCsv (const std::vector<std::string>& vecList, const char separator)
{
        vector<string>::const_iterator i = vecList.begin();
        bool first = true;
        stringstream ss;
        while (i != vecList.end()) {
                if (first) {
                        first = false;
                } else {
                        ss << separator;
                }
                ss << *i;
                i++;
        }
        return ss.str();
}

std::string
wml::FoundryUtilities::listToCsv (const std::list<std::string>& listList, const char separator)
{
        list<string>::const_iterator i = listList.begin();
        bool first = true;
        stringstream ss;
        while (i != listList.end()) {
                if (first) {
                        first = false;
                } else {
                        ss << separator;
                }
                ss << *i;
                i++;
        }
        return ss.str();
}

std::string
wml::FoundryUtilities::setToCsv (const std::set<std::string>& setList, const char separator)
{
        set<string>::iterator i = setList.begin();
        bool first = true;
        stringstream ss;
        while (i != setList.end()) {
                if (first) {
                        first = false;
                } else {
                        ss << separator;
                }
                ss << *i;
                i++;
        }
        return ss.str();
}

map<string, string>
wml::FoundryUtilities::csvToMap (const std::string& csvList, const char relationship, const char separator)
{
        map<string, string> rtn;
        string theList (csvList);
        vector<string> v = FoundryUtilities::csvToVector (theList, separator);
        vector<string>::iterator iV = v.begin(), end = v.end();
        string lastKey ("");
        while (iV != end) {
                // Catch comma in value (key1=val1,val2,key2=val3)
                if (iV->find (relationship) == string::npos) {
                        if (!lastKey.empty()) {
                                rtn[lastKey] += "," + *iV;
                                ++iV;
                                continue;
                        }
                }

                vector<string> tokens = FoundryUtilities::csvToVector (*iV, relationship, false);
                if (tokens.size() != 2) {
                        stringstream errss;
                        errss << "FoundryUtilities::csvToMap: Problem getting key/value pair from '" << *iV << "'";
                        throw runtime_error (errss.str());
                }
                string key (tokens.at(0)), value (tokens.at(1));
                rtn.insert (make_pair (key, value));
                lastKey = key;
                ++iV;
        }
        return rtn;
}

std::string
wml::FoundryUtilities::suffix (const int n)
{
        string suf("th"); // Most numbers end in "th" (in English)
        int leastSig = n%10;     // Right most, least significant numeral
        int leastSigTwo = n%100; // Right most pair of numerals

        switch (leastSig) {
        case 1:
                if (leastSigTwo != 11) {
                        suf = "st";
                }
                break;
        case 2:
                if (leastSigTwo != 12) {
                        suf = "nd";
                }
                break;
        case 3:
                if (leastSigTwo != 13) {
                        suf = "rd";
                }
                break;
        default:
                break;
        }

        return suf;
}

void
wml::FoundryUtilities::pdfConversion (const string inputPath,
                                      const string outputDevice, const string outputPath,
                                      const unsigned int width, const unsigned int height,
                                      const bool wait, const unsigned int resolution)
{
        string widthS, heightS, resS;
        stringstream tempSS, returnSS;

        tempSS << width;
        widthS = tempSS.str();
        tempSS.str("");
        tempSS << height;
        heightS = tempSS.str();
        tempSS.str("");
        tempSS << resolution;
        resS = tempSS.str();
        tempSS.str("");

        Process ghostScript;
        string processPath = "/usr/bin/gs";
        list<string> args;
        args.push_back ("gs");
        args.push_back ("-dNOPAUSE");
        args.push_back ("-dBATCH");
        args.push_back ("-g" + widthS + "x" + heightS);
        if (resolution > 0) {
                args.push_back ("-r" + resS);
        }
        args.push_back ("-sDEVICE=" + outputDevice);
        args.push_back ("-sOutputFile=" + outputPath);
        args.push_back (inputPath);
        int rtn = ghostScript.start (processPath, args);
        if (rtn == PROCESS_FAILURE) {
                DBG ("Failed to start the process for ghostScript");
                return;
        }

        if (wait == true) {
                ghostScript.waitForStarted();
                while (ghostScript.running()) {
                        usleep (500000);
                        ghostScript.probeProcess();
                }
        }
}

void
wml::FoundryUtilities::pdfToJpeg (const string inputPath, const string outputPath,
                                  const unsigned int width, const unsigned int height,
                                  const bool wait, const unsigned int resolution)
{
        pdfConversion (inputPath, "jpeg", outputPath, width, height, wait, resolution);
}

void
wml::FoundryUtilities::pdfToPng (const string inputPath, const string outputPath,
                                 const unsigned int width,const  unsigned int height,
                                 const bool wait, const unsigned int resolution)
{
        pdfConversion (inputPath, "pngalpha", outputPath, width, height, wait, resolution);
}


// Probably "doIconv8to8" cf "doIconv8to16" etc.
#define ICONV_INBUF_SIZE 128
#define ICONV_OUTBUF_SIZE 256
void
wml::FoundryUtilities::doIconv (const char * fromEncoding,
                                const char * toEncoding,
                                const std::string& fromString,
                                std::string& toString)
{
        // We'll read ICONV_INBUF_SIZE bytes at a time from fromString into inbuf...
        char inbuf[ICONV_INBUF_SIZE];
        char* ibp = inbuf;
        // ...and write the transcoded output into outbuf, which is
        // twice as big, in case the output encoding is twice as wide.
        char outbuf[ICONV_OUTBUF_SIZE];
        char* obp = outbuf;

        stringstream ss;

        iconv_t cd = iconv_open (toEncoding, fromEncoding);
        if (cd == (iconv_t)-1) {
                stringstream ee;
                ee << __FUNCTION__ << ": iconv initialization error for conversion from "
                   << fromEncoding << " to " << toEncoding;
                throw runtime_error (ee.str());
        }

        string::size_type pos = 0;
        while (pos < fromString.size()) {

                // Zero buffers and reset pointers
                memset (inbuf, 0, ICONV_INBUF_SIZE);
                memset (outbuf, 0, ICONV_OUTBUF_SIZE);
                ibp = inbuf;
                obp = outbuf;

                string subs = fromString.substr (pos, ICONV_INBUF_SIZE);
                pos += subs.size();
                strncpy (inbuf, subs.c_str(), ICONV_INBUF_SIZE);

                size_t inbufleft = subs.size();
                size_t outbufleft = ICONV_OUTBUF_SIZE;

                //cerr << "Before iconv() call, inbuf is '" << inbuf << "' outbuf is '" << outbuf
                //     << "' inbufleft is " << inbufleft << " and outbufleft is " << outbufleft << endl;

                size_t iconv_rtn = 0;
                iconv_rtn = iconv (cd, &ibp, &inbufleft, &obp, &outbufleft);
                while (iconv_rtn == (size_t)-1) {

                        int theE = errno;

                        if (theE == EINVAL) {
                                throw runtime_error ("Need to deal with this... (memmove?)");
                        } else if (theE == EILSEQ) {
                                // EILSEQ Illegal input
                                // sequence. Indicates an unusable
                                // character. If an input character
                                // does not belong to the input code
                                // set, no conversion is attempted on
                                // the unusable character. The
                                // InBufLeft parameter indicates the
                                // bytes left to be converted,
                                // including the first byte of the
                                // unusable character. InBuf parameter
                                // points to the first byte of the
                                // unusable character sequence.

                                // N.B. It appears that this error is
                                // _also_ emitted when iconv()
                                // encounters a character in the input
                                // buffer that is valid, but for which
                                // an identical character does not
                                // exist in the target codeset.

                                DBG ("Skipping unusable character in input buffer");
                                // Insert placeholder char in output buffer?
                                obp++;
                                outbufleft--;
                                ibp++;
                                inbufleft--;
                                // Call iconv on remainder of input buffer
                                iconv_rtn = iconv (cd, &ibp, &inbufleft, &obp, &outbufleft);
                        } else {
                                DBG ("doIconv() called on string '" << fromString
                                     << "' from " << fromEncoding
                                     << " to " << toEncoding);
                                stringstream ee;
                                ee << __FUNCTION__ << ": Error in iconv(), errno: " << theE;
                                throw runtime_error (ee.str());
                        }

                }
                //cerr << "After iconv call, outbuf is '" << obp << "' and bytes used is "
                //     << ICONV_OUTBUF_SIZE - outbufleft << "\n";

                size_t i = 0;
                while (i<ICONV_OUTBUF_SIZE-outbufleft) {
                        //cerr << "outbuf["<<i<<"]=" << (int)outbuf[i] << "(" << outbuf[i] << ")" << endl;
                        // Seems to be necessary to fill ss char by char (at least after first call to iconv()):
                        ss << outbuf[i];
                        i++;
                }
        }

        toString = ss.str();

        iconv_close (cd);
}

void
wml::FoundryUtilities::openFilestreamForAppend (fstream& f, const char * filepath)
{
        stringstream errss;

        FoundryUtilities::closeFilestream (f);
        FoundryUtilities::clearFilestreamFlags (f);

        try {
                if (access (filepath, R_OK | W_OK) != 0) {
                        int theError = errno;
                        gid_t myGid=getgid();
                        uid_t myUid=getuid();
                        errss << "File access error. access() caused errno=" << theError
                              << ". This process has uid/gid: " << myUid << '/' << myGid;
                        throw runtime_error (errss.str());
                }

                f.open (filepath, ios::out|ios::app);

        } catch (ios_base::failure& e) {
                errss << "exception opening '" << filepath << "' for append: " << e.what();
                throw runtime_error (errss.str());
        }

        if (!f.is_open()) {
                errss << "Failed to open file '" << filepath << "' for appending.";
                throw runtime_error (errss.str());
        }
}

void
wml::FoundryUtilities::openFilestreamForAppend (fstream& f, const string& filepath)
{
        FoundryUtilities::openFilestreamForAppend (f, filepath.c_str());
}

void
wml::FoundryUtilities::openFilestreamForOverwrite (fstream& f, const char * filepath)
{
        stringstream errss;

        FoundryUtilities::closeFilestream (f);
        FoundryUtilities::clearFilestreamFlags (f);
        try {
                f.open (filepath, ios::out|ios::trunc);
        } catch (ios_base::failure& e) {
                errss << "exception opening '" << filepath << "' for overwrite: " << e.what();
                throw runtime_error (errss.str());
        }

        if (!f.is_open()) {
                errss << "Failed to open file '" << filepath << "' for overwriting";
                throw runtime_error (errss.str());
        }
}

void
wml::FoundryUtilities::openFilestreamForOverwrite (fstream& f, const string& filepath)
{
        FoundryUtilities::openFilestreamForOverwrite (f, filepath.c_str());
}

void
wml::FoundryUtilities::closeFilestream (fstream& f)
{
        if (f.is_open()) {
                try {
                        f.clear();
                        f.close();
                } catch (ios_base::failure& e) {
                        stringstream errss;
                        errss << "exception closing filestream: " << e.what();
                        throw runtime_error (errss.str());
                }
        }
}

void
wml::FoundryUtilities::clearFilestreamFlags (fstream& f)
{
        if (f.good()) {
                return;
        }
        try {
                f.clear (ios::goodbit);

        } catch (ios_base::failure& e) {
                stringstream errss;
                errss << "exception clearing filestream flags: " << e.what();
                throw runtime_error (errss.str());
        }
}

void
wml::FoundryUtilities::check_tmp_messages (const int megabytes)
{
        /*
         * /tmp/logrotate.conf is created if the file doesn't exist.
         */

        struct stat buf;
        bool noLogrotateConf = false;

        memset (&buf, 0, sizeof(struct stat));
        if (stat ("/etc/wml/sys/logrotate_tmp.conf", &buf)) {
                // No such path.
                noLogrotateConf = true;
                // Ensure directory is present:
                if (!FoundryUtilities::dirExists("/etc/wml/sys")) {
                        FoundryUtilities::createDir ("/etc/wml/sys");
                }
        } else {
                if (S_ISREG (buf.st_mode)) {
                        noLogrotateConf = false;
                } else {
                        noLogrotateConf = true;
                }
        }

        if (noLogrotateConf == true) {

                // Create logrotate.conf file

                ofstream f;
                f.open ("/etc/wml/sys/logrotate_tmp.conf", ios::out|ios::trunc);
                if (f.is_open()) {
                        f << "/tmp/messages {" << endl;
                        f << "  rotate 1" << endl;
                        f << "  start 1" << endl;
                        f << "  size " << megabytes << "M" << endl;
                        f << "  missingok" << endl;
                        f << "  create" << endl;
                        // Here, if we can't write to a test file, we'd like to defer
                        f << "  firstaction" << endl;
                        f << "    killall syslog-ng" << endl;
                        f << "  endscript" << endl;
                        // Or here, if the log share doesn't remount... what?
                        f << "  lastaction" << endl;
                        f << "    /usr/sbin/syslog-ng" << endl;
                        f << "  endscript" << endl;
                        f << "}" << endl;
                        f.close();
                } else {
                        syslog (LOG_ERR, "%s: Failed to write logrotate.conf", __FUNCTION__);
                        // Attempt to rotate anyway...
                }
        }

        // Can add -v before -s to get more info on what logrotate is up to.
        system ("/usr/sbin/logrotate -s /tmp/logrotate_tmp.status "
                "/etc/wml/sys/logrotate_tmp.conf >/tmp/logrotate_tmp.out 2>&1");
}

bool
wml::FoundryUtilities::valid_ip (const string ip_string)
{
        regex_t * ip_regex = (regex_t*)0;
        int reg_error;
        int valid = 0; /* The result of the validation. */

        ip_regex = (regex_t*) malloc (sizeof(regex_t));

        /* Validation one - does it conform to NNN.NNN.NNN.NNN format? */
        reg_error =
                regcomp (ip_regex,
                         "^(25[0-5]|2[0-4][0-9]|[01]?[0-9]?[0-9])\\."
                         "(25[0-5]|2[0-4][0-9]|[01]?[0-9]?[0-9])\\."
                         "(25[0-5]|2[0-4][0-9]|[01]?[0-9]?[0-9])\\."
                         "(25[0-5]|2[0-4][0-9]|[01]?[0-9]?[0-9])$",
                         REG_EXTENDED);

        if (reg_error) {
                DBG ("Failed compiling basic numeric form regex check");
                if (ip_regex != (regex_t*)0) {
                        regfree (ip_regex);
                        free (ip_regex);
                }
                return false;
        }

        valid = regexec (ip_regex, ip_string.c_str(), 0, NULL, 0) ? 0 : 1;
        regfree (ip_regex); // clean up before second regcomp

        /* Validation two. Make sure the IP is not 0.0.0.0. */
        reg_error =
                regcomp (ip_regex,
                         "^0+\\.0+\\.0+\\.0+$",
                         REG_EXTENDED);

        if (reg_error) {
                DBG ("Failed compiling 0.0.0.0 regex check");
                if (ip_regex != (regex_t*)0) {
                        regfree (ip_regex);
                        free (ip_regex);
                }
                return false;
        }

        if (regexec (ip_regex, ip_string.c_str(), 0, NULL, 0) == 0) {
                valid = 0;
        }

        regfree (ip_regex);
        free (ip_regex);

        if (valid > 0) {
                return true;
        }
        return false;
}

bool
wml::FoundryUtilities::valid_mac (const string& mac_string)
{
        regex_t * ip_regex = (regex_t*)0;
        int reg_error;
        int valid = 0; /* The result of the validation. */

        ip_regex = (regex_t*) malloc (sizeof(regex_t));

        /* Validation one - does it conform to XX:XX:XX:XX:XX:XX format? */
        reg_error =
                regcomp (ip_regex,
                         "^([0-9a-fA-F][0-9a-fA-F]):"
                         "([0-9a-fA-F][0-9a-fA-F]):"
                         "([0-9a-fA-F][0-9a-fA-F]):"
                         "([0-9a-fA-F][0-9a-fA-F]):"
                         "([0-9a-fA-F][0-9a-fA-F]):"
                         "([0-9a-fA-F][0-9a-fA-F])$",
                         REG_EXTENDED);

        if (reg_error) {
                DBG ("Failed compiling basic format regex check");
                if (ip_regex != (regex_t*)0) {
                        regfree (ip_regex);
                        free (ip_regex);
                }
                return false;
        }

        valid = regexec (ip_regex, mac_string.c_str(), 0, NULL, 0) ? 0 : 1;

        regfree (ip_regex);
        free (ip_regex);

        if (valid > 0) {
                return true;
        }
        return false;
}

bool
wml::FoundryUtilities::getlineWithCopy (std::istream* istrm,
                                        std::string& line,
                                        std::ofstream& copystrm,
                                        bool& inputComplete,
                                        const char eolChar)
{
#ifdef DEBUG2
        if (istrm->good()) { DBG2 ("good is set in istrm"); }
        if (istrm->eof()) { DBG2 ("eof is set in istrm"); }
        if (istrm->bad()) { DBG2 ("bad is set in istrm (non-recoverable error)"); }
        if (istrm->fail()) { DBG2 ("fail is set in istrm (recoverable error)"); }
#endif
        line = "";
        bool gotline(false);
        if (!inputComplete) {
                if ((gotline = getline (*istrm, line, eolChar))) {
                        DBG2 ("line is: '" << line << "'");
                        if (copystrm.is_open()) {
                                copystrm << line << eolChar;
                        }
                }
                if (istrm->eof()) {
                        inputComplete = true;
                }
        }
        return gotline;
}

void
wml::FoundryUtilities::decodeURIComponent (string& s)
{
        if (s.empty()) { return; }

        stringstream ss;
        // Decode %NN to ascii(0xNN)
        for (string::iterator i = s.begin(); i != s.end(); i++) {
                if (*i == '%') {
                        string hex ="";
                        hex += *(++i);
                        if (i == s.end()) {
                                // Fell off end of string. String mal-formed.
                                break;
                        }
                        hex += *(++i);
                        if (i == s.end()) {
                                // Fell off end of string after second char.
                                break;
                        }
                        unsigned long ascii = strtoul(hex.c_str(), NULL, 16);
                        if (ascii >= 0 && ascii < 256) {
                                ss << (char)ascii;
                        }
                } else {
                        ss << *i;
                }
        }
        s = ss.str();
}

void
wml::FoundryUtilities::encodeURIComponent (string& s)
{
        if (s.empty()) { return; }

        stringstream ss;
        // Set stream to use uppercase for hex strings
        ss << uppercase;
        ss.fill('0');
        string charset = URI_UNRESERVED_CHARS;
        for (string::iterator i = s.begin(); i != s.end(); i++) {
                if (charset.find(*i) != string::npos) {
                        ss << *i;
                } else {
                        ss << hex << "%";
                        ss.width(2);
                        ss << (0xff & static_cast<int>(*i)) << dec;
                        ss.width(1);
                }

        }
        s = ss.str();
}
//@}
