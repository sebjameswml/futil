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
#include <algorithm>

#include "FoundryUtilities.h"
#include "Process.h"

extern "C" {
#include <errno.h>
#include <sys/types.h>
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
}

/*!
 * A single global variable to hold the value of the file we need to
 * unlock. Used by FoundryUtilities::getLock() and
 * FoundryUtilities::releaseLock()
 */
int lock_fd_glob = 0;

using namespace std;

// Constructor
wml::FoundryUtilities::FoundryUtilities (void)
{
}

// Destructor
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
wml::FoundryUtilities::stripTrailingSpaces (std::string& input)
{
	return FoundryUtilities::stripTrailingChars (input);
}

int
wml::FoundryUtilities::stripTrailingChars (std::string& input, char c)
{
	int i = 0;
	while (input.size()>0 && input[input.size()-1] == c) {
		input.erase (input.size()-1, 1);
		i++;
	}
	return i;
}

int
wml::FoundryUtilities::stripLeadingSpaces (std::string& input)
{
	return FoundryUtilities::stripLeadingChars (input);
}

int
wml::FoundryUtilities::stripLeadingChars (std::string& input, char c)
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
				      bool replaceAll)
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
			if (ptr >= stl) {
				// This is a move backwards along the
				// string far enough that we don't
				// match a substring of the last
				// replaceTerm in the next search.
				pos = ptr - stl;
			} else {
				break;
			}
			count++;
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

	return;
}

int
wml::FoundryUtilities::searchReplace (std::string& searchTerm,
				      std::string& replaceTerm,
				      std::string& data,
				      bool replaceAll)
{
	const char* st = searchTerm.c_str();
	const char* rt = replaceTerm.c_str();
	return FoundryUtilities::searchReplace (st, rt, data, replaceAll);
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

string
wml::FoundryUtilities::freeSpace (string dirPath)
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

bool
wml::FoundryUtilities::fileExists (std::string& path)
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
wml::FoundryUtilities::fileExists (const char * path)
{
	string thePath = path;
	return fileExists (thePath);
}

bool
wml::FoundryUtilities::dirExists (std::string& path)
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
wml::FoundryUtilities::createDir (std::string path,
				  mode_t mode,
				  int uid, int gid)
{
	if (path.empty()) {
		// Create no directory. Just return.
		return;
	}

	// Set umask to 0000 to stop it interfering with mode
	int oldUmask = umask (0000);

	string::size_type pos, lastPos = path.size()-1;
	vector<string> dirs;
	while ((pos = path.find_last_of ('/', lastPos)) != 0) {
		dirs.push_back (path.substr(pos+1, lastPos-pos));
		DBG2 ("Push back directory " << path.substr(pos+1, lastPos-pos));
		lastPos = pos-1;
	}
	DBG2 ("Push back directory " << path.substr(1, lastPos));
	dirs.push_back (path.substr(1, lastPos));

	vector<string>::reverse_iterator i = dirs.rbegin();
	string prePath("");
	while (i != dirs.rend()) {
		prePath += "/" + *i;
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
				//emsg << "Path exists, maybe not as a directory";
				i++;
				continue;
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
			case ENOSPC:
				emsg << "Out of storage space/quota exceeded.";
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
		if (uid>-1 && gid>-1) {
			chown (prePath.c_str(), static_cast<uid_t>(uid), static_cast<gid_t>(gid));
		}
		i++;
	}

	// Reset umask
	umask (oldUmask);
}

void
wml::FoundryUtilities::setPermissions (string filepath, mode_t mode)
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

void
wml::FoundryUtilities::setOwnership (string filepath, int uid, int gid)
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
wml::FoundryUtilities::touchFile (std::string path)
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
wml::FoundryUtilities::copyFile (string& from, string& to)
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
wml::FoundryUtilities::copyFile (string& from, ostream& to)
{
	ifstream in;

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
wml::FoundryUtilities::copyFile (istream& from, string& to)
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
wml::FoundryUtilities::moveFile (string from, string to)
{
	FoundryUtilities::copyFile (from, to);
	FoundryUtilities::unlinkFile (from);
}

void
wml::FoundryUtilities::unlinkFile (string fpath)
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
wml::FoundryUtilities::getFileType (string filePath)
{
	struct magic_set* magic;
	magic = magic_open (MAGIC_CHECK); // Can OR in MAGIC_MIME,
	                                  // MAGIC_RAW, etc for
	                                  // additional features. See
	                                  // file command's file.c for
	                                  // details.

	if (magic_load (magic, "/usr/share/wmlpp/wmlmagic.mgc") == -1) {
		// If the WML specific magic file is not found, fall
		// back to the one shipped with "file":
		if (magic_load (magic, "/usr/share/file/magic.mgc") == -1) {
			stringstream ee;
			ee << "Error loading file-typing info: " << magic_error (magic);
			throw runtime_error (ee.str());
		}
	}

	const char* type = magic_file (magic, filePath.c_str());
	if (type == (const char*)0) {
		stringstream ee;
		ee << "Error typing file: " << magic_error (magic);
		throw runtime_error (ee.str());
	}

	string fileType = type;
	magic_close (magic);

	return fileType;
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

// Same as get_lock() in wmlppctrl (libwmlppfilt)
void
wml::FoundryUtilities::getLock (int fd)
{
	if (fd > 0) {
		lock_fd_glob = fd;
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

	return;
}

void
wml::FoundryUtilities::releaseLock (int fd)
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

	return;
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
	return;
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
	return;
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
wml::FoundryUtilities::generateRandomFilename (const char* prefixPath, unsigned int numChars)
{
	unsigned int length = numChars + strlen (prefixPath);
	string rtn = generateRandomFilename (prefixPath);
	return rtn.substr (0, length);
}

bool
wml::FoundryUtilities::vectorContains (std::vector<unsigned int>& v, unsigned int i)
{
	for (unsigned int k = 0; k<v.size(); k++) {
		if (v[k] == i) {
			return true;
		}
	}
	return false;
}

int
wml::FoundryUtilities::strVectorContains (std::vector<string>& v, string s)
{
	for (unsigned int k = 0; k<v.size(); k++) {
		if (v[k] == s) {
			return k;
		}
	}
	return -1;
}

int
wml::FoundryUtilities::strVectorMatches (std::vector<string>& v, string s)
{
	for (unsigned int k = 0; k<v.size(); k++) {
		if (s.find (v[k], 0) != string::npos) {
			return k;
		}
	}
	return -1;
}

int
wml::FoundryUtilities::firstNotMatching (std::vector<string>& v, string s)
{
	for (unsigned int k = 0; k<v.size(); k++) {
		if (v[k] != s) {
			return k;
		}
	}
	return -1;
}

bool
wml::FoundryUtilities::listContains (std::list<unsigned int>& l, unsigned int i)
{
	list<unsigned int>::iterator it = l.begin();
	while (it != l.end()) {
		if ((*it) == i) {
			return true;
		}
		it++;
	}
	return false;
}

bool
wml::FoundryUtilities::listContains (std::list<std::string>& l, std::string& s)
{
	list<string>::iterator it = l.begin();
	while (it != l.end()) {
		if ((*it) == s) {
			return true;
		}
		it++;
	}
	return false;
}

bool
wml::FoundryUtilities::pidLoaded (int pid)
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
	return;
}

void
wml::FoundryUtilities::strToMacAddr (std::string& macStr, unsigned int* mac)
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

	return;
}

std::string
wml::FoundryUtilities::macAddrToStr (unsigned int* mac)
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
wml::FoundryUtilities::readDirectoryTree (vector<string>& vec,
					  string dirPath)
{
	FoundryUtilities::readDirectoryTree (vec, dirPath.c_str(), "");
}

void
wml::FoundryUtilities::readDirectoryTree (vector<string>& vec,
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
		string msg = "Failed to open directory " + dirPath;
		throw runtime_error (msg);
	}

	while ((ep = readdir (d))) {
	        unsigned char fileType;
	        string fileName = dirPath + "/" + (string)ep->d_name;
	        if (ep->d_type == DT_LNK) {
	                // Is it a link to a directory or a file?
		        struct stat * buf = NULL;
		        buf = (struct stat*) malloc (sizeof (struct stat));
		        if (!buf) { // Malloc error.
		                throw runtime_error ("Failed to malloc buf; could not stat link " + fileName);
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
			FoundryUtilities::readDirectoryTree (vec, baseDirPath, newPath.c_str());
		} else {
			// Non-directories are simply added to the vector
			string newEntry;
			if (sd.size() == 0) {
				newEntry = ep->d_name;
			} else {
				newEntry = sd + "/" + ep->d_name;
			}
			vec.push_back (newEntry);
		}
	}

	(void) closedir (d);
	return;
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
wml::FoundryUtilities::monthStr (int month, bool shortFormat)
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
wml::FoundryUtilities::dateToNum (std::string& dateStr)
{
	char separator = '\0';

	if (dateStr.empty()) {
		return -2;
	}

	if (dateStr.size() < 8) {
		return -3;
	}

	if (dateStr[4] < '0'
	    || dateStr[4] > '9') {
		separator = dateStr[4];
		if (dateStr.size() < 10) {
			return -4;
		}
	}

	string year;
	string month;
	string day;
	unsigned int yearN=0, monthN=0, dayN=0;

	year = dateStr.substr (0,4);

	if (separator == '\0') {
		month = dateStr.substr (4,2);
		day = dateStr.substr (6,2);
	} else {
		month = dateStr.substr (5,2);
		day = dateStr.substr (8,2);
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

std::string
wml::FoundryUtilities::numToDateTime (time_t epochSeconds,
				      char dateSeparator,
				      char timeSeparator)
{
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
wml::FoundryUtilities::numToDate (time_t epochSeconds,
				  char separator)
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
wml::FoundryUtilities::getMonthFromLog (std::string& filePath,
					unsigned int lineNum)
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
			       std::string cssFile,
			       bool inlineOutput)
{
        FoundryUtilities::getScript (SCRIPT_CSS, rCSS, cssFile, inlineOutput);
}

void
wml::FoundryUtilities::getJavascript (std::stringstream& rJavascript,
                                      std::string jsFile,
                                      bool inlineOutput)
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
wml::FoundryUtilities::getScript (SCRIPT_TYPE script,
                                  std::stringstream& rScript,
                                  std::string scriptFile,
                                  bool inlineOutput)
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
        return;
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

	return;
}

bool
wml::FoundryUtilities::containsOnlyNumerals (std::string& str)
{
	for (unsigned int i=0; i<str.size(); i++) {
		if (static_cast<unsigned int>(str[i]) < 48 ||
		    static_cast<unsigned int>(str[i]) > 57) {
			// Not ascii '0' to '9'
			return false;
		}
	}
	return true;
}

void
wml::FoundryUtilities::sanitize (std::string& str,
				 const char* allowed,
				 bool eraseForbidden)
{
	string allowedStr(allowed);
	FoundryUtilities::sanitize (str, allowedStr, eraseForbidden);
}

void
wml::FoundryUtilities::sanitize (std::string& str,
				 std::string& allowed,
				 bool eraseForbidden)
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
					std::string allowed,
					char replaceChar)
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
	return;
}

void
wml::FoundryUtilities::coutFile (std::string filePath)
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
	return;
}

int
wml::FoundryUtilities::fileSize (string filePath)
{
	struct stat buf;
	memset (&buf, 0, sizeof(struct stat));
	if (stat (filePath.c_str(), &buf)) {
#ifdef DEBUG
		int the_error = errno;
		debuglog (LOG_ERR,
			  "%s: Failed to stat %s, errno=%d",
			  __FUNCTION__, filePath.c_str(), the_error);
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

std::vector<std::string>
wml::FoundryUtilities::csvToVector (std::string& csvList, char separator,
				    bool ignoreTrailingEmptyVal)
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
wml::FoundryUtilities::csvToList (std::string& csvList, char separator)
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

std::string
wml::FoundryUtilities::vectorToCsv (std::vector<std::string>& vecList, char separator)
{
	vector<string>::iterator i = vecList.begin();
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
wml::FoundryUtilities::listToCsv (std::list<std::string>& listList, char separator)
{
	list<string>::iterator i = listList.begin();
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
wml::FoundryUtilities::suffix (int n)
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
wml::FoundryUtilities::pdfConversion (string inputPath,
				      string outputDevice, string outputPath,
				      unsigned int width, unsigned int height)
{
        string widthS, heightS;
        stringstream tempSS, returnSS;

        tempSS << width;
        widthS = tempSS.str();
        tempSS.str("");
        tempSS << height;
        heightS = tempSS.str();
        tempSS.str("");

        Process ghostScript;
        string processPath = "/usr/bin/gs";
        list<string> args;
        args.push_back ("-dNOPAUSE");
        args.push_back ("-dBATCH");
        args.push_back ("-g" + widthS + "x" + heightS);
        args.push_back ("-sDEVICE=" + outputDevice);
        args.push_back ("-sOutputFile=" + outputPath);
        args.push_back (inputPath);
        ghostScript.start (processPath, args);
}

void
wml::FoundryUtilities::pdfToJpeg (string inputPath, string outputPath,
				  unsigned int width, unsigned int height)
{
	pdfConversion (inputPath, "jpeg", outputPath, width, height);
}

void
wml::FoundryUtilities::pdfToPng (string inputPath, string outputPath,
				 unsigned int width, unsigned int height)
{
	pdfConversion (inputPath, "pngalpha", outputPath, width, height);
}


// Probably "doIconv8to8" cf "doIconv8to16" etc.
#define ICONV_INBUF_SIZE 128
#define ICONV_OUTBUF_SIZE 256
void
wml::FoundryUtilities::doIconv (const char * fromEncoding,
				const char * toEncoding,
				std::string& fromString,
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
		if (iconv_rtn == (size_t)-1) {

			int theE = errno;

			if (theE == EINVAL) {
				throw runtime_error ("Need to deal with this... (memmove?)");
			} else {
				DBG ("doIconv() called on string '" << fromString
				      << "' from " << fromEncoding
				      << " to " << toEncoding);
				stringstream ee;
				ee << __FUNCTION__ << ": Error in iconv(), errno: " << theE;
				throw runtime_error (ee.str());
			}

		} else {
			// cerr << "iconv() returned " << iconv_rtn << endl;
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
	return;
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
	return;
}

void
wml::FoundryUtilities::openFilestreamForAppend (fstream& f, const string& filepath)
{
	FoundryUtilities::openFilestreamForAppend (f, filepath.c_str());
	return;
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
	return;
}

void
wml::FoundryUtilities::openFilestreamForOverwrite (fstream& f, const string& filepath)
{
	FoundryUtilities::openFilestreamForOverwrite (f, filepath.c_str());
	return;
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
	return;
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
	return;
}

#define ONE_MEGABYTE 1048576
void
wml::FoundryUtilities::check_tmp_messages (int megabytes)
{
	struct stat * buf = NULL;
	int the_error = 0;
	FILE * fp = NULL;

	buf = (struct stat*) malloc (sizeof (struct stat));
	if (!buf) {
		// Malloc error.
		exit (-1);
	}
	memset (buf, 0, sizeof(struct stat));
	if (stat ("/tmp/messages", buf)) {
		the_error = errno;
		debuglog (LOG_ERR,
			  "%s: Failed to stat /tmp/messages, errno=%d",
			  __FUNCTION__, the_error);
	}

	if (buf->st_size > (megabytes * ONE_MEGABYTE)) {
		// Truncate /tmp/messages.
		fp = fopen ("/tmp/messages", "w+");
		if (fp != NULL) {
			fprintf (fp, "%s\n", "syslog truncated.");
			fclose (fp);
			syslog (LOG_INFO, "/tmp/messages truncated.");
		}
	}

	free (buf);
	return;
}
