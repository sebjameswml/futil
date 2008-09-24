#ifdef __GNUG__
#  pragma implementation
#endif

#include "config.h"

#include <iostream>
#include <fstream>
#include <sstream> 
#include <pstream.h>
#include <cstdlib>
#include <stdexcept>
#include <list>

#include "FoundryUtilities.h"

extern "C" {
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <dirent.h>
#include <syslog.h>
#include "debuglog.h"
}

/*!
 * A single global variable to hold the value of the file we need to
 * unlock
 */
int lock_fd_glob = 0;

extern "C" {

	int emailMsgPlusAttachment (const char *subject, const char *message, const char *attachfile)
	{
		FILE * fp = NULL;
		char recipients[1024];
		char hostname[64];
		char cmd[1064];
		char attachment[256];
		char * ptr = NULL;
		unsigned int len = 0;

		syslog (LOG_DEBUG, "%s: Called", __FUNCTION__);

		/*
		 * Read the ssmtp.conf file to get email recipients.
		 */
		if (!(fp = fopen ("/etc/ssmtp/ssmtp.conf", "r"))) return -1;
		fgetc (fp);                   /* skip the '#' char on the first line */
		fgets (recipients, 1024, fp); /* read the rest of the line. */
		fclose (fp);

		/*
		 * Read in the hostname to place it in the message.
		 */
		if ((fp = fopen ("/etc/hostname", "r"))) {
			fgets (hostname, 64, fp);
			len = strlen (hostname);
			if (hostname[--len] == '\n') {
				hostname[len] = '\0';
			}
			fclose (fp);
		}

		/*
		 * Ought to check the formatting of the recipients here, as well as
		 * checking for some file length.. (WRITEME).
		 */
		len = strlen (recipients);
		if (len == 0) {
			/* Zero length recipients simply means "no alerts please" */
			syslog (LOG_DEBUG, "%s: Zero length recipients", __FUNCTION__);
			return 0;
		} else {
			/* remove the trailing newline */
			recipients[--len] = '\0';
		}

		/*
		 * Remove the path of the attachment
		 */
		ptr = strrchr (attachfile, '/');
		if (ptr) {
			ptr++;
			if (*ptr != '\0') {
				snprintf (attachment, 255, "%s", ptr);
			} else {
				snprintf (attachment, 255, "%s", "unknown");
			}
		} else {
			snprintf (attachment, 255, "%s", "unknown");
		}

		/*
		 * Now we build the email file and the system command line up.
		 */
		if (!(fp = fopen ("/tmp/email.txt", "w"))) {
			syslog (LOG_DEBUG,
				"%s: Failed to open email.txt for writing",
				__FUNCTION__);
			return -1;
		}
		fprintf (fp, "From:wmlpp@wmltd.co.uk\n");
		fprintf (fp, "To:%s\n", recipients);
		fprintf (fp, "Subject: %s (from %s)\n", subject, hostname);
		/* Add mime stuff */
		fprintf (fp, "MIME-Version: 1.0\n");
		fprintf (fp, "Content-Type: multipart/mixed;");
		fprintf (fp, " boundary=\"kWDUIHP984GTHIWOEF8WAHGAWERGF\"\n\n");
		fprintf (fp, "--kWDUIHP984GTHIWOEF8WAHGAWERGF\n");
		fprintf (fp, "Content-Type: text/plain; charset=US-ASCII\n\n");
		fprintf (fp, "%s\n\n", message);
		fprintf (fp, "--kWDUIHP984GTHIWOEF8WAHGAWERGF\n");
		fprintf (fp, "Content-Type: application/octet-stream\n");
		fprintf (fp, "Content-Transfer-Encoding: base64\n");
		fprintf (fp, "Content-Disposition: attachment;\n");
		fprintf (fp, " filename=\"%s\"\n\n", attachment);
		/* Stop, close the file, run base64 to append encoded file */
		fclose (fp);

		/*
		 * base64 encode the attachment. This is a hacked base64 which
		 * appends data.
		 */
		snprintf (cmd, 1024, "/usr/bin/base64 -e -n %s /tmp/email.txt", attachfile);
		syslog (LOG_DEBUG, "%s: cmd is '%s'", __FUNCTION__, cmd);
		system (cmd);

		/* re-open file for appending */
		if (!(fp = fopen ("/tmp/email.txt", "a"))) {
			syslog (LOG_DEBUG,
				"%s: failed to open email.txt for appending",
				__FUNCTION__);
			return -1;
		}
		fprintf (fp, "--kWDUIHP984GTHIWOEF8WAHGAWERGF--\n\n");
		fclose (fp);

		snprintf (cmd, 1064, "/usr/sbin/ssmtp %s </tmp/email.txt", recipients);
		system (cmd);

		return 0;
	}

	/*
	 * 'emailMsg()' Send an email to certain recipients. Right now
	 * the recipients are pulled out of ssmtp.conf but they would be
	 * better specified in cupsd.conf.
	 *
	 * The function expects to get a line of recipients in comma-separated
	 * format starting from the 2nd character of the first line up to the
	 * '\n' character.
	 *
	 * For example:
	 *
	 * #seb@wmltd.co.uk,test@somewhere.else
	 * #
	 * # The rest of the ssmtp.conf file follows..
	 * #
	 *
	 */
	int emailMsg (const char* subject, const char *message)
	{
		FILE *fp = NULL;
		char recipients[1024];
		char hostname[64];
		char cmd[1064];
		unsigned int len = 0;

		/*
		 * Read the ssmtp.conf file to get email recipients.
		 */
		if (!(fp = fopen ("/etc/ssmtp/ssmtp.conf", "r"))) return -1;
		fgetc (fp);                   /* skip the '#' char on the first line */
		fgets (recipients, 1024, fp); /* read the rest of the line. */
		fclose (fp);

		/*
		 * Read in the hostname to place it in the message.
		 */
		if ((fp = fopen ("/etc/hostname", "r"))) {
			fgets (hostname, 64, fp);
			len = strlen (hostname);
			if (hostname[--len] == '\n') {
				hostname[len] = '\0';
			}
			fclose (fp);
		}

		/*
		 * Ought to check the formatting of the recipients here, as well as
		 * checking for some file length.. (WRITEME).
		 */
		len = strlen (recipients);
		if (len == 0) {
			/* Zero length recipients simply means "no alerts please" */
			return 0;
		} else {
			/* remove the trailing newline */
			recipients[--len] = '\0';
		}

		/*
		 * Now we build the email file and the system command line up.
		 */
		if (!(fp = fopen ("/tmp/email.txt", "w"))) return -1;
		fprintf (fp, "From:wmlpp@wmltd.co.uk\n");
		fprintf (fp, "To:%s\n", recipients);
		fprintf (fp, "Subject: %s (from %s)\n", subject, hostname);
		fprintf (fp, "%s\n\n", message);
		fclose (fp);
  
		snprintf (cmd, 1064, "/usr/sbin/ssmtp %s </tmp/email.txt", recipients);
		system (cmd);

		return 0;
	}

	/*
	 * Pass pointer to array of 2 * unsigned ints
	 */
	int get_system_mac (unsigned int * mac)
	{
		struct ifreq ifr;
		int sd;

		/* Set up network socket to get mac address */
		strcpy(ifr.ifr_name, "eth0");

		sd = socket (AF_INET, SOCK_DGRAM, 0);
		if (sd == -1) {
			syslog (LOG_ERR, 
				"%s: Error in call to socket() '%s'",
				__FUNCTION__, strerror (errno));		
			return -1;
		}


		if (ioctl (sd, SIOCGIFHWADDR, &ifr) < 0) {
			syslog (LOG_ERR, 
				"%s: Error in call to ioctl() '%s'",
				__FUNCTION__, strerror (errno));

			// Clean up
			close (sd);
			return -1;
		}

		mac[1] = (((ifr.ifr_hwaddr.sa_data[0]<<8) & 0x00000000ff00) |
			  ((ifr.ifr_hwaddr.sa_data[1])    & 0x0000000000ff)   );

		mac[0] = (((ifr.ifr_hwaddr.sa_data[2]<<24) & 0x0000ff000000) |
			  ((ifr.ifr_hwaddr.sa_data[3]<<16) & 0x000000ff0000) |
			  ((ifr.ifr_hwaddr.sa_data[4]<<8)  & 0x00000000ff00) |
			  ((ifr.ifr_hwaddr.sa_data[5])     & 0x0000000000ff)   );
	
		// Clean up
		close (sd);
		return 0;
	}

#ifdef LOCKING_SORTED_AND_READY_TO_MOVE_THIS_FROM_WMLPP_CPP
	/* Used by cups-lpd.c and rawprint.c so must be available externally */
	void wait_for_wmlpp_lock (void)
	{
		// Check lock
		struct stat * buf = NULL;
		int locked = 1; // assume locked

		buf = (struct stat*) malloc (sizeof (struct stat));
		if (!buf) {
			//throw runtime_error ("Malloc error");
		}
		while (locked) {
			memset (buf, 0, sizeof(struct stat));
			if (stat ("/tmp/wmlpp_lock", buf)) {
				// stat returned non-zero so no file, no lock
				locked = 0;
			}
		}
		if (buf) { free (buf); }

		return;
	}
#endif

} // extern "C"

/*
 * The rest of the file is C++
 */

using namespace std;

ofstream pdfDbgFile;

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
wml::FoundryUtilities::getJavascript (std::stringstream& rJavascript,
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

#define WMLPPLOCK_TIMEOUT 5000 // ms
bool
wml::FoundryUtilities::getWmlppLock (void)
{
	debuglog2 (LOG_DEBUG, "%s: called", __FUNCTION__);

	// Check lock
	struct stat * buf = NULL;
	bool locked = true; // assume locked

	int fd = 0;

	buf = (struct stat*) malloc (sizeof (struct stat));
	if (!buf) {
		throw runtime_error ("Malloc error");
	}
	unsigned int tcount = 0;
	while (locked && tcount < WMLPPLOCK_TIMEOUT) {

		memset (buf, 0, sizeof(struct stat));

		if (stat ("/tmp/wmlpp_lock", buf)) {
			// stat returned non-zero so no file, no lock
			int e = errno;
			stringstream emsg;
			emsg << "getWmlppLock(): stat() set error: ";
			switch (e) {
			case EACCES:
				emsg << "Search permission is denied";
				break;
			case EBADF:
				emsg << "Bad file descriptor";
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
				// Path invalid (part or all of it
				// doesn't exist) THIS means that we
				// can get the lock.
				locked = false;
				// Re-get the lock.
				fd = open ("/tmp/wmlpp_lock", O_RDONLY|O_CREAT);
				if (fd == -1) {
					int e = errno;
					stringstream msg;
					msg << "Error: fopen (\"/tmp/wmlpp_lock\", \"w\") "
						"set errno:" << e;
					throw runtime_error (msg.str());
				} else {
					lock_fd_glob = fd;
					FoundryUtilities::getLock (fd);
					debuglog2 (LOG_DEBUG,
						   "%s: Opened platform lock /tmp/wmlpp_lock",
						   __FUNCTION__);
				}
				break;

			case ENOMEM:
				emsg << "Out of kernel memory";
				break;
			case ENOTDIR:
				emsg << "A component of the path is not a directory.";
				break;
			default:
				break;
			}
			if (locked == false) {
				break; // out of the while loop
			} else {
				throw runtime_error (emsg.str());
			}
		}
		usleep (1000); // 1 ms
		tcount++;

	}
	if (buf) { free (buf); }

	if (locked == true) {
		// We failed to get a lock within the timeout, so:
		debuglog2 (LOG_DEBUG, "%s: no lock obtained, returning false", __FUNCTION__);
		return false;
	}

	debuglog2 (LOG_DEBUG, "%s: returning true", __FUNCTION__);
	return true;
}


void
wml::FoundryUtilities::releaseWmlppLock (void)
{
	debuglog2 (LOG_DEBUG, "%s: called", __FUNCTION__);

	// Use stat to find the file!
	bool haveFile = true;
	struct stat* buf = NULL;
	buf = (struct stat*) malloc (sizeof(struct stat));
	memset (buf, 0, sizeof(struct stat));
	if (stat ("/tmp/wmlpp_lock", buf)) {
		// Error, no file.
		// stat returned non-zero so no file, no lock
		int e = errno;
		stringstream emsg;
		emsg << "releaseWmlppLock(): stat() set error: ";
		switch (e) {
		case EACCES:
			emsg << "Search permission is denied";
			break;
		case EBADF:
			emsg << "Bad file descriptor";
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
			// Path invalid (part or all of it
			// doesn't exist) THIS means that we
			// can get the lock.
			haveFile = false;			
			break;

		case ENOMEM:
			emsg << "Out of kernel memory";
			break;
		case ENOTDIR:
			emsg << "A component of the path is not a directory.";
			break;
		default:
			break;
		}
	}

	if (haveFile == false) {
		// No lock to release.
		debuglog2 (LOG_DEBUG, "%s: no lock to release, returning", __FUNCTION__);
		lock_fd_glob = 0;
		return;
	} else {
		debuglog2 (LOG_DEBUG, "%s: about to releaseLock(f)", __FUNCTION__);
		FoundryUtilities::releaseLock (lock_fd_glob);
		debuglog2 (LOG_DEBUG, "%s: releaseLock(f) returned", __FUNCTION__);
		close (lock_fd_glob);
		lock_fd_glob = 0;
	}

	// release simply by unlinking the file.
	if (unlink ("/tmp/wmlpp_lock") == 0) {
		debuglog2 (LOG_DEBUG,
			   "%s: Removed platform lock file /tmp/wmlpp_lock",
			   __FUNCTION__);
	} else {
		int e = errno;
		stringstream msg;
		msg << "Error: unlink (\"/tmp/wmlpp_lock\") set errno:" << e;
		debuglog2 (LOG_DEBUG,
			   "%s: unlink (\"/tmp/wmlpp_lock\") set errno:%d",
			   __FUNCTION__, e);
		throw runtime_error (msg.str());
	}

	debuglog2 (LOG_DEBUG, "%s: returning", __FUNCTION__);
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

/*!
 * Return the zero character for the given base.
 */
char
wml::FoundryUtilities::zeroChar (int base)
{
	switch (base) {
	case 2:
	case 10:
	case 16:
		return '0';
		break;
	case 52:
		return WMLBASE52_CHAR00;
		break;
	default:
		// unknown base
		stringstream msg;
		msg << "Unimplemented base " << base;
		throw runtime_error (msg.str());
		break;
	}
	return '\0';
}

/*!
 * Add the number n to the string str, represented in the given base.
 */
void
wml::FoundryUtilities::addChar (int n, string& str, int base, bool at_start)
{
	char c;

	switch (base) {

	case 2:
		switch (n) {
		case 0:
			c = '0';
			break;
		case 1:
			c = '1';
			break;
		default:
			// whoop whoop
			throw runtime_error ("Error: n > 1!");
		}
		break;

	case 10:
		switch (n) {
		case 0:
			c = '0';
			break;
		case 1:
			c = '1';
			break;
		case 2:
			c = '2';
			break;
		case 3:
			c = '3';
			break;
		case 4:
			c = '4';
			break;
		case 5:
			c = '5';
			break;
		case 6:
			c = '6';
			break;
		case 7:
			c = '7';
			break;
		case 8:
			c = '8';
			break;
		case 9:
			c = '9';
			break;
		default:
			// whoop whoop
			throw runtime_error ("Error: n > 9!");
		}
		break;

	case 16:
		switch (n) {
		case 0:
			c = '0';
			break;
		case 1:
			c = '1';
			break;
		case 2:
			c = '2';
			break;
		case 3:
			c = '3';
			break;
		case 4:
			c = '4';
			break;
		case 5:
			c = '5';
			break;
		case 6:
			c = '6';
			break;
		case 7:
			c = '7';
			break;
		case 8:
			c = '8';
			break;
		case 9:
			c = '9';
			break;
		case 10:
			c = 'a';
			break;
		case 11:
			c = 'b';
			break;
		case 12:
			c = 'c';
			break;
		case 13:
			c = 'd';
			break;
		case 14:
			c = 'e';
			break;
		case 15:
			c = 'f';
			break;
		default:
			// whoop whoop
			throw runtime_error ("Error: n > 9!");
		}
		break;

	case 52:
		switch (n) {
		case 0:
			c = WMLBASE52_CHAR00;
			break;
		case 1:
			c = WMLBASE52_CHAR01;
			break;
		case 2:
			c = WMLBASE52_CHAR02;
			break;
		case 3:
			c = WMLBASE52_CHAR03;
			break;
		case 4:
			c = WMLBASE52_CHAR04;
			break;
		case 5:
			c = WMLBASE52_CHAR05;
			break;
		case 6:
			c = WMLBASE52_CHAR06;
			break;
		case 7:
			c = WMLBASE52_CHAR07;
			break;
		case 8:
			c = WMLBASE52_CHAR08;
			break;
		case 9:
			c = WMLBASE52_CHAR09;
			break;
		case 10:
			c = WMLBASE52_CHAR10;
			break;
		case 11:
			c = WMLBASE52_CHAR11;
			break;
		case 12:
			c = WMLBASE52_CHAR12;
			break;
		case 13:
			c = WMLBASE52_CHAR13;
			break;
		case 14:
			c = WMLBASE52_CHAR14;
			break;
		case 15:
			c = WMLBASE52_CHAR15;
			break;
		case 16:
			c = WMLBASE52_CHAR16;
			break;
		case 17:
			c = WMLBASE52_CHAR17;
			break;
		case 18:
			c = WMLBASE52_CHAR18;
			break;
		case 19:
			c = WMLBASE52_CHAR19;
			break;
		case 20:
			c = WMLBASE52_CHAR20;
			break;
		case 21:
			c = WMLBASE52_CHAR21;
			break;
		case 22:
			c = WMLBASE52_CHAR22;
			break;
		case 23:
			c = WMLBASE52_CHAR23;
			break;
		case 24:
			c = WMLBASE52_CHAR24;
			break;
		case 25:
			c = WMLBASE52_CHAR25;
			break;
		case 26:
			c = WMLBASE52_CHAR26;
			break;
		case 27:
			c = WMLBASE52_CHAR27;
			break;
		case 28:
			c = WMLBASE52_CHAR28;
			break;
		case 29:
			c = WMLBASE52_CHAR29;
			break;
		case 30:
			c = WMLBASE52_CHAR30;
			break;
		case 31:
			c = WMLBASE52_CHAR31;
			break;
		case 32:
			c = WMLBASE52_CHAR32;
			break;
		case 33:
			c = WMLBASE52_CHAR33;
			break;
		case 34:
			c = WMLBASE52_CHAR34;
			break;
		case 35:
			c = WMLBASE52_CHAR35;
			break;
		case 36:
			c = WMLBASE52_CHAR36;
			break;
		case 37:
			c = WMLBASE52_CHAR37;
			break;
		case 38:
			c = WMLBASE52_CHAR38;
			break;
		case 39:
			c = WMLBASE52_CHAR39;
			break;
		case 40:
			c = WMLBASE52_CHAR40;
			break;
		case 41:
			c = WMLBASE52_CHAR41;
			break;
		case 42:
			c = WMLBASE52_CHAR42;
			break;
		case 43:
			c = WMLBASE52_CHAR43;
			break;
		case 44:
			c = WMLBASE52_CHAR44;
			break;
		case 45:
			c = WMLBASE52_CHAR45;
			break;
		case 46:
			c = WMLBASE52_CHAR46;
			break;
		case 47:
			c = WMLBASE52_CHAR47;
			break;
		case 48:
			c = WMLBASE52_CHAR48;
			break;
		case 49:
			c = WMLBASE52_CHAR49;
			break;
		case 50:
			c = WMLBASE52_CHAR50;
			break;
		case 51:
			c = WMLBASE52_CHAR51;
			break;
		default:
			// whoop whoop
			throw runtime_error ("Error: n > 51!");
		}
		break;

	default:
		// unknown base
		stringstream msg;
		msg << "Unimplemented base " << base;
		throw runtime_error (msg.str());
		break;
	}

	// Add the new number to str.
	if (at_start == true) {
		str.insert (str.begin(), c);
	} else {
		str += c;
	}

	return;
}

/*!
 * Convert the character c, which is represented with the radix base
 * into an integer, which is returned.
 */
int
wml::FoundryUtilities::baseNToInt (char c, int base)
{
	int rtn;
	switch (base) {

	case 10:
		/* Reglar ole base 10 */
		switch (c) {
		case '0':
			rtn = 0;
			break;
		case '1':
			rtn = 1;
			break;
		case '2':
			rtn = 2;
			break;
		case '3':
			rtn = 3;
			break;
		case '4':
			rtn = 4;
			break;
		case '5':
			rtn = 5;
			break;
		case '6':
			rtn = 6;
			break;
		case '7':
			rtn = 7;
			break;
		case '8':
			rtn = 8;
			break;
		case '9':
			rtn = 9;
			break;
		default:
			rtn = -1;
			break;
		}
		break;
		
	case 16:
		/* Base 16, Hexadecimal */
		switch (c) {
		case '0':
			rtn = 0;
			break;
		case '1':
			rtn = 1;
			break;
		case '2':
			rtn = 2;
			break;
		case '3':
			rtn = 3;
			break;
		case '4':
			rtn = 4;
			break;
		case '5':
			rtn = 5;
			break;
		case '6':
			rtn = 6;
			break;
		case '7':
			rtn = 7;
			break;
		case '8':
			rtn = 8;
			break;
		case '9':
			rtn = 9;
			break;
		case 'a':
		case 'A':
			rtn = 10;
			break;
		case 'b':
		case 'B':
			rtn = 11;
			break;
		case 'c':
		case 'C':
			rtn = 12;
			break;
		case 'd':
		case 'D':
			rtn = 13;
			break;
		case 'e':
		case 'E':
			rtn = 14;
			break;
		case 'f':
		case 'F':
			rtn = 15;
			break;
		default:
			rtn = -1;
			break;
		}
		break;

	case 52:
		/* Base 52, our homegrown character representation,
		 * used for WML keys */
		switch (c) {
		case WMLBASE52_CHAR00:
			rtn = 0;
			break;
		case WMLBASE52_CHAR01:
			rtn = 1;
			break;
		case WMLBASE52_CHAR02:
			rtn = 2;
			break;
		case WMLBASE52_CHAR03:
			rtn = 3;
			break;
		case WMLBASE52_CHAR04:
			rtn = 4;
			break;
		case WMLBASE52_CHAR05:
			rtn = 5;
			break;
		case WMLBASE52_CHAR06:
			rtn = 6;
			break;
		case WMLBASE52_CHAR07:
			rtn = 7;
			break;
		case WMLBASE52_CHAR08:
			rtn = 8;
			break;
		case WMLBASE52_CHAR09:
			rtn = 9;
			break;
		case WMLBASE52_CHAR10:
			rtn = 10;
			break;
		case WMLBASE52_CHAR11:
			rtn = 11;
			break;
		case WMLBASE52_CHAR12:
			rtn = 12;
			break;
		case WMLBASE52_CHAR13:
			rtn = 13;
			break;
		case WMLBASE52_CHAR14:
			rtn = 14;
			break;
		case WMLBASE52_CHAR15:
			rtn = 15;
			break;
		case WMLBASE52_CHAR16:
			rtn = 16;
			break;
		case WMLBASE52_CHAR17:
			rtn = 17;
			break;
		case WMLBASE52_CHAR18:
			rtn = 18;
			break;
		case WMLBASE52_CHAR19:
			rtn = 19;
			break;
		case WMLBASE52_CHAR20:
			rtn = 20;
			break;
		case WMLBASE52_CHAR21:
			rtn = 21;
			break;
		case WMLBASE52_CHAR22:
			rtn = 22;
			break;
		case WMLBASE52_CHAR23:
			rtn = 23;
			break;
		case WMLBASE52_CHAR24:
			rtn = 24;
			break;
		case WMLBASE52_CHAR25:
			rtn = 25;
			break;
		case WMLBASE52_CHAR26:
			rtn = 26;
			break;
		case WMLBASE52_CHAR27:
			rtn = 27;
			break;
		case WMLBASE52_CHAR28:
			rtn = 28;
			break;
		case WMLBASE52_CHAR29:
			rtn = 29;
			break;
		case WMLBASE52_CHAR30:
			rtn = 30;
			break;
		case WMLBASE52_CHAR31:
			rtn = 31;
			break;
		case WMLBASE52_CHAR32:
			rtn = 32;
			break;
		case WMLBASE52_CHAR33:
			rtn = 33;
			break;
		case WMLBASE52_CHAR34:
			rtn = 34;
			break;
		case WMLBASE52_CHAR35:
			rtn = 35;
			break;
		case WMLBASE52_CHAR36:
			rtn = 36;
			break;
		case WMLBASE52_CHAR37:
			rtn = 37;
			break;
		case WMLBASE52_CHAR38:
			rtn = 38;
			break;
		case WMLBASE52_CHAR39:
			rtn = 39;
			break;
		case WMLBASE52_CHAR40:
			rtn = 40;
			break;
		case WMLBASE52_CHAR41:
			rtn = 41;
			break;
		case WMLBASE52_CHAR42:
			rtn = 42;
			break;
		case WMLBASE52_CHAR43:
			rtn = 43;
			break;
		case WMLBASE52_CHAR44:
			rtn = 44;
			break;
		case WMLBASE52_CHAR45:
			rtn = 45;
			break;
		case WMLBASE52_CHAR46:
			rtn = 46;
			break;
		case WMLBASE52_CHAR47:
			rtn = 47;
			break;
		case WMLBASE52_CHAR48:
			rtn = 48;
			break;
		case WMLBASE52_CHAR49:
			rtn = 49;
			break;
		case WMLBASE52_CHAR50:
			rtn = 50;
			break;
		case WMLBASE52_CHAR51:
			rtn = 51;
			break;
		default:
			rtn = -1;
			break;
		}
		break;

	default:
		// unknown base
		stringstream msg;
		msg << "Unimplemented base " << base;
		throw runtime_error (msg.str());
		break;
	}

	if (rtn == -1) {
		stringstream msg;
		msg << "Character '" << c
		    << "' can't be represented in base " << base;
		throw runtime_error (msg.str());
	}

	return rtn;
}

/*
 * Separate hex function because hex can be done faster than an
 * arbitrary base because there are 8 hex numerals ("4 bit nibbles")
 * per 32 bit uint32
 */
void
wml::FoundryUtilities::hexToUint32s (const string& str,
					      UINT32_TYPE * num,
					      UINT32_TYPE numlen)
{	
	unsigned int len = str.size();
	int posn = 0; // The position of n in the decimal str. For
		      // "123", 3 is at posn 0, 2 at posn 1 and 1 at
		      // posn 3.
	UINT32_TYPE iter = 0; // The part of the array of UINT32_TYPEs in num
		              // that we are currently filling.
	char c;

	int nibble = 0; // The current active bit-quad in num[iter];
	while (len!= 0) {
		len--;
		c = str[len];
		int n = baseNToInt (c, 16);
		num[iter] |= n << (nibble++<<2);
		if (nibble == 8) {
			nibble = 0;
			iter++;
			if (iter >= numlen) {
				throw runtime_error ("hexToUint32s: Overflow; exiting");
			}
		}
		posn++;
	}
	return;
}

void
wml::FoundryUtilities::baseNToUint32s (const string& str,
						UINT32_TYPE * num,
						UINT32_TYPE numlen,
						int base)
{	
	unsigned int len = str.size();
	unsigned int i = 0;
	string strcopy = ""; // A copy of str which we can modify
	string halfstr = ""; // Holds str divided by two.
	int posn = 0; // The position of n in the decimal str. For
		      // "123", 3 is at posn 0, 2 at posn 1 and 1 at
		      // posn 3.
	UINT32_TYPE iter = 0; // The part of the array of UINT32_TYPEs in num
		              // that we are currently filling.
	int bit = 0; // The current active bit in num[iter];

	// copy str into strcopy, but reversed - we work from the
	// least significant numeral to the most	
	while (len) {
		strcopy += str[--len];
	}

	//cout << "(reversed) string to convert: '" 
	//     << strcopy << "' of length "<< strcopy.length() << endl;       

	int last_result = 0;
	while ((len = strcopy.length()) > 0) {

		//cout << "strcopy = '" << strcopy << "' and len=" << len << endl;
		posn = 0;
		// Initialise the result
		halfstr = "";

		i=0;
		while (i<len) {

			char c = strcopy[i];
			int n = baseNToInt (c, base);
			//printf ("len=%d, i=%d, c=%c, n=%d\n", len, i, c, n);
			// Divide this numeral
			int result = n/2;
			int remain = n%2;

			// For first char, does division of n by 2 generate a remainder?
			if (posn==0 && remain>0) {
				//cout << "  Moving a 1 to bit " << bit
				//     << ", in num[" << iter << "]\n";
				if (iter >= numlen) {
					throw runtime_error ("baseNToUint32s: Overflowed.");
				}
				num[iter] |= 1 << bit++;

			} else if (posn==0 && remain==0) {
				bit++;
				//cout << "  just increment bit to " << bit << "\n";
			} else if (posn>0 && remain>0) {
				// if remain==1, add 26 to last_result
				int halfbase = base/2;
				int halfrem = base%2;
				if (halfrem) {
					throw runtime_error ("Don't know what to do "
							     "if base is not divisible by 2");
				}
				//cout << "  add " << halfbase 
				//     << " to last_result (" << last_result << ")\n";
				last_result += halfbase;
			}

			if (posn>0) {
				addChar (last_result, halfstr, base, false);
			}

			last_result = result;

			if (bit>31) {
				bit = 0;
				iter++;				
			}

			posn++;
			i++;
		}
		// Do the last character here.
		if (posn>0) {
			addChar (last_result, halfstr, base, false);
		}
		
		// Copy halfstr back into strcopy
		strcopy = halfstr;

		// Remove any extraneous zeroes; we want the
		// result to shrink to zero length
		len = strcopy.size();
		for (i=len-1; i>=0; i--) {
			if (strcopy[i] == zeroChar (base)) {
				strcopy.erase(i);
			} else {
				break;
			}
		}
	}

	return;
}

string
wml::FoundryUtilities::formatUint64InBaseN (UINT64_TYPE num, int base)
{
	string outNum = "";

	UINT64_TYPE n = num;
	UINT64_TYPE quotient = 0;
	UINT64_TYPE remainder;
		
	while (n>=(UINT64_TYPE)base) {
		//cerr << "quotient: " << quotient << ", dividing by: " << base << endl;
		quotient = n/(UINT64_TYPE)base;
		remainder = n%(UINT64_TYPE)base;
		//cerr << "quotient now: " << quotient << " remainder: " << remainder << endl;

		addChar ((unsigned int)remainder, outNum, base, true);
		n = quotient;
	}
	addChar (quotient, outNum, base, true);

	return outNum;
}

string
wml::FoundryUtilities::formatUint128InBaseN (UINT32_TYPE * num, UINT32_TYPE numlen, int base)
{
	if (numlen>4) {
		throw runtime_error ("Can only do 128 bit algebra, "
				     "so numlen must be 4 or less.");
	}

	string outNum = "";
	wmlint128 n;

	// Set up the 128 bit data structure
	n.lo = (UINT64_TYPE)num[0] | (UINT64_TYPE)num[1]<<32;
	n.hi = (UINT64_TYPE)num[2] | (UINT64_TYPE)num[3]<<32;
	n.neg = false;
	n.big = true;
	//cerr << "n.lo=" << n.lo << " n.hi=" << n.hi << endl;

	wmlint128 quotient;
	quotient.lo = 0;
	quotient.hi = 0;
	quotient.neg = false;
	quotient.big = false;

	UINT64_TYPE remainder;

	while (bigGTE(n, (UINT64_TYPE)base) == true) {
		//cerr << "n: hi:" << n.hi << " lo:" << n.lo << ", dividing by: " << base << endl;

		quotient = bigDiv (n, (INT64_TYPE)base);
		remainder = bigModUnsigned (n, (UINT64_TYPE)base);

		//cerr << "quotient: hi:" << quotient.hi << " lo:" << quotient.lo << endl;
		//cerr << "remainder: " << remainder << endl;


		addChar ((unsigned int)remainder, outNum, base, true);
		n = quotient;
	}
	//cerr << "n: hi:" << n.hi << " lo:" << n.lo << " not gte " << base << endl;
	addChar ((unsigned int)quotient.lo, outNum, base, true);

	return outNum;
}

wml::wmlint128
wml::FoundryUtilities::bigMultUnsigned (UINT64_TYPE a, UINT64_TYPE b)
{
	if (a == 0xffffffffffffffffULL &&
	    b == 0xffffffffffffffffULL) {
		throw runtime_error ("Multiplication will overflow.");
	}

	wmlint128 prod;
	prod.neg = false;

	//cerr << "bigMultUnsigned: a is " << a << " b is " << b << endl;

	UINT64_TYPE a1 = a >> 32;
	UINT64_TYPE a0 = a - (a1<<32);

	UINT64_TYPE b1 = b >> 32;
	UINT64_TYPE b0 = b - (b1<<32);

	UINT64_TYPE d = a0*b0;
	UINT64_TYPE d1 = d >> 32;
	UINT64_TYPE d0 = d - (d1<<32);

	UINT64_TYPE e = a0*b1;
	UINT64_TYPE e1 = e >> 32;
	UINT64_TYPE e0 = e - (e1<<32);

	UINT64_TYPE f = a1*b0;
	UINT64_TYPE f1 = f >> 32;
	UINT64_TYPE f0 = f - (f1<<32);

	UINT64_TYPE g = a1*b1;
	UINT64_TYPE g1 = g >> 32;
	UINT64_TYPE g0 = g - (g1<<32);

	UINT64_TYPE sum = d1+e0+f0;
	UINT64_TYPE carry = 0;
	/* split <<32 left shift up because cpp will mess it up. */
	UINT64_TYPE roll = 1<<30;
	roll <<= 2;

	UINT64_TYPE pmax = roll-1;
	while (pmax < sum) {
		sum -= roll;
		carry++;
	}

	prod.lo = d0 + (sum<<32);
	prod.hi = carry + e1 + f1 + g0 + (g1<<32);

	if (prod.hi || (prod.lo >> 63)) {
		prod.big = true;
	} else {
		prod.big = false;
	}

	return prod;
}

wml::wmlint128
wml::FoundryUtilities::bigDiv (wmlint128 numerator, UINT64_TYPE denom)
{
	wmlint128 quotient;
	UINT64_TYPE hirem;   /* hi remainder */
	UINT64_TYPE qlo;

	//cerr << "bigDiv: numerator.neg:" << numerator.neg << endl;
	quotient.neg = numerator.neg;
	if (0 > denom) {
		denom = -denom;
		quotient.neg = !quotient.neg;
	}
	//cerr << "bigDiv: quotient.neg:" << quotient.neg << endl;

	quotient.hi = numerator.hi / denom;
	hirem = numerator.hi - quotient.hi * denom;
  
	UINT64_TYPE lo = 1<<30;
	lo <<= 33;
	lo /= denom;
	lo <<= 1;

	lo *= hirem; 
	quotient.lo = lo + numerator.lo/denom;

	/* Deal with low remainder bits.
	 * Is there a more efficient way of doing this?
	 */
	//cerr << "call bigMultUnsigned (" << quotient.lo << ", " << denom << ")" << endl;
	wmlint128 mu = bigMultUnsigned (quotient.lo, denom);

	INT64_TYPE nn = 0x7fffffffffffffffULL & numerator.lo;
	INT64_TYPE rr = 0x7fffffffffffffffULL & mu.lo;
	INT64_TYPE rnd = nn - rr;
	rnd /= denom;

	/* ?? will this ever overflow ? */
	qlo = quotient.lo;
	quotient.lo += rnd;
	if (qlo > quotient.lo) {
		quotient.hi += 1;
	}

	/* compute the carry situation */
	if ((quotient.hi || (quotient.lo >> 63))) {
		quotient.big = true;
	} else {
		quotient.big = false;
	}

	return quotient;
}


UINT64_TYPE
wml::FoundryUtilities::bigModUnsigned (wmlint128 numerator, UINT64_TYPE denom)
{
	wmlint128 quotient;

	quotient = bigDiv (numerator, denom);
	//cerr << "bigMod(): after bigDiv, quotient: lo:" << quotient.lo << " hi:" << quotient.hi << endl;
	wmlint128 mu = bigMultUnsigned (quotient.lo, denom);
	//cerr << "bigMod(): mu(q*d): lo:" << hex << mu.lo << " hi:" << mu.hi << endl;

	UINT64_TYPE nn = 0x7fffffffffffffffULL & numerator.lo;
	UINT64_TYPE rr = 0x7fffffffffffffffULL & mu.lo;
	return nn - rr;
}

/* Greater Than or Equal */
bool
wml::FoundryUtilities::bigGTE (wmlint128 a, UINT64_TYPE b)
{
	if (a.neg == false && a.big == true) {
		return true;
	} else if (a.neg == true && a.big == true) {
		return false;
	}

	/* big is false */
	if (a.neg == false) {
		if (a.lo >= b) {
			return true;
		} else {
			return false;
		}

	} else if (a.neg == true) {
		if (a.lo == 0 && b==0) {
			return true;
		} else {
			return false;
		}
	}

	throw runtime_error ("bigGTE: Error, should already have returned.");
	return false;
}
