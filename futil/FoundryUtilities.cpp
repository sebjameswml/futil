#ifdef __GNUG__
#  pragma implementation
#endif

#include <iostream>
#include <fstream>
#include <sstream> 
#include <pstream.h>
#include <cstdlib>
#include <stdexcept>
#include <list>

#include "FoundryUtilities.h"

//#include <wmlppctrl/processlog.h>

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <dirent.h>
}

using namespace std;

ofstream pdfDbgFile;

// Constructor
foundryWebUI::FoundryUtilities::FoundryUtilities (void)
{
}

// Destructor
foundryWebUI::FoundryUtilities::~FoundryUtilities (void)
{
}

int
foundryWebUI::FoundryUtilities::ensureUnixNewlines (std::string& input)
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
foundryWebUI::FoundryUtilities::stripTrailingCarriageReturn (std::string& input)
{
	if (input[input.size()-1] == '\r') {
		input.erase(input.size()-1, 1);
		return 1;
	}
	return 0;
}

unsigned int
foundryWebUI::FoundryUtilities::getMemory (void)
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

bool
foundryWebUI::FoundryUtilities::fileExists (std::string& path)
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
foundryWebUI::FoundryUtilities::fileExists (const char * path)
{
	string thePath = path;
	return fileExists (thePath);
}

std::string
foundryWebUI::FoundryUtilities::fileModDatestamp (const char* filename)
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

// Same as get_lock() in wmlppctrl (libwmlppfilt())
void
foundryWebUI::FoundryUtilities::getLock (FILE * f)
{
	int fd, theError;

	fd = fileno (f);
	
	if (flock (fd, LOCK_EX)) {
		theError = errno;
		//LOGLN ("flock returned errno " << theError);
	}

	return;
}

void
foundryWebUI::FoundryUtilities::releaseLock (FILE * f)
{
	int fd, theError;

	fd = fileno (f);

	if (flock (fd, LOCK_UN)) {
		theError = errno;
		//LOGLN ("flock returned errno " << theError);
	}

	return;
}

void
foundryWebUI::FoundryUtilities::backSlashEscape (std::string& str)
{
	for (unsigned int i=0; i<str.size(); i++) {
		if (str[i] == '\\') {
			str.insert ((i++), "\\");
		}
	}
}

void
foundryWebUI::FoundryUtilities::slashEscape (std::string& str)
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
foundryWebUI::FoundryUtilities::stripDosPath (std::string& dosPath)
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

bool
foundryWebUI::FoundryUtilities::vectorContains (std::vector<unsigned int>& v, unsigned int i)
{
	for (unsigned int k = 0; k<v.size(); k++) {
		if (v[k] == i) {
			return true;
		}
	}
	return false;
}

int
foundryWebUI::FoundryUtilities::strVectorContains (std::vector<string>& v, string s)
{
	for (unsigned int k = 0; k<v.size(); k++) {
		if (v[k] == s) {
			return k;
		}
	}
	return -1;
}

int
foundryWebUI::FoundryUtilities::strVectorMatches (std::vector<string>& v, string s)
{
	for (unsigned int k = 0; k<v.size(); k++) {
		if (s.find (v[k], 0) != string::npos) {
			return k;
		}
	}
	return -1;
}

int
foundryWebUI::FoundryUtilities::firstNotMatching (std::vector<string>& v, string s)
{
	for (unsigned int k = 0; k<v.size(); k++) {
		if (v[k] != s) {
			return k;
		}
	}
	return -1;
}

bool
foundryWebUI::FoundryUtilities::pidLoaded (int pid)
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
foundryWebUI::FoundryUtilities::getMacAddr (void)
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
foundryWebUI::FoundryUtilities::readDirectoryTree (vector<string>& vec,
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

		if (ep->d_type == DT_DIR || ep->d_type == DT_LNK) {
			
			// Skip "." and ".." directories
			if ( ((entry_len = strlen (ep->d_name)) > 0 && ep->d_name[0] == '.') &&
			     (ep->d_name[1] == '\0' || ep->d_name[1] == '.') ) {
				continue;
			}
			
			// For all other links and directories, recurse.
			string newPath;
			if (sd.size() == 0) {
				newPath = ep->d_name;				
			} else {
				newPath = sd + "/" + ep->d_name;
			}
			FoundryUtilities::readDirectoryTree (vec, baseDirPath, newPath.c_str());

		} else {
			// Non-directories/links are simply added to the vector
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
foundryWebUI::FoundryUtilities::readHostname (void)
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
foundryWebUI::FoundryUtilities::yearNow (void)
{
	time_t curtime; // Current time
	struct tm * t;
	curtime = time(NULL);
	t = localtime (&curtime);
	unsigned int theYear = static_cast<unsigned int>(t->tm_year+1900);
	return theYear;
}

unsigned int
foundryWebUI::FoundryUtilities::monthNow (void)
{
	time_t curtime; // Current time
	struct tm * t;
	curtime = time(NULL);
	t = localtime (&curtime);
	unsigned int theMonth = static_cast<unsigned int>(t->tm_mon+1);
	return theMonth;
}

unsigned int
foundryWebUI::FoundryUtilities::getMonthFromLog (std::string& filePath,
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
foundryWebUI::FoundryUtilities::getJavascript (std::stringstream& rJavascript,
					       std::string jsFile)
{
	ifstream f;
	f.open (jsFile.c_str(), ios::in);

	if (!f.is_open()) {
		stringstream msg;
		msg << "Couldn't open javascript file '" << jsFile << "'";
		throw runtime_error (msg.str());
	}

	// Add the text which tells the browser this is javascript:
	rJavascript << "<script type=\"text/javascript\" >" << endl;

	string line;
	while (getline (f, line, '\n')) {
		rJavascript << line << endl;
	}

	rJavascript << "</script>" << endl;

	f.close();
	return;
}

void
foundryWebUI::FoundryUtilities::unicodeize (std::string& str)
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
foundryWebUI::FoundryUtilities::containsOnlyNumerals (std::string& str)
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

#define WMLPPLOCK_TIMEOUT 5000 // ms
bool
foundryWebUI::FoundryUtilities::getWmlppLock (void)
{
	// Check lock
	struct stat * buf = NULL;
	bool locked = true; // assume locked

	buf = (struct stat*) malloc (sizeof (struct stat));
	if (!buf) {
		throw runtime_error ("Malloc error");
	}
	unsigned int tcount = 0;
	while (locked && tcount < WMLPPLOCK_TIMEOUT) {
		memset (buf, 0, sizeof(struct stat));
		if (stat ("/tmp/wmlpp_lock", buf)) {
			// stat returned non-zero so no file, no lock
			locked = false;
		}
		usleep (1000); // 1 ms
		tcount++;
	}
	if (buf) { free (buf); }

	if (locked == true) {
		// We failed to get a lock within the timeout, so:
		return false;
	}

	// If we get here, then we have the lock, so we can re-lock by
	// creating the lock file.
	FILE * f = NULL;
	f = fopen ("/tmp/wmlpp_lock", "w");
	if (f == NULL) {
		//LOGLN ("Failed to create /tmp/wmlpp_lock");
	} else {
		//LOGLN ("Created platform lock /tmp/wmlpp_lock");
		FoundryUtilities::getLock (f);
		fclose (f);
	}

	return true;
}

void
foundryWebUI::FoundryUtilities::releaseWmlppLock (void)
{
	FILE * f = NULL;
	f = fopen ("/tmp/wmlpp_lock", "r");
	if (f == NULL) {
		//LOGLN ("Failed to find /tmp/wmlpp_lock");
		return;
	} else {
		FoundryUtilities::releaseLock (f);
		fclose (f);
	}

	// release simply by unlinking the file.
	if (unlink ("/tmp/wmlpp_lock") == 0) {
		//LOGLN ("Removed platform lock file /tmp/wmlpp_lock");
	}
}

