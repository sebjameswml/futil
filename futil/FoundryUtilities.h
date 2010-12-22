/* -*-c++-*- */
/*!
 * \file FoundryUtilities.h
 *
 * \brief Declares the class FoundryUtilities
 *
 * FoundryUtilities contains some generally useful static member
 * functions.
 *
 * This file is part of WML futil - a library containing static public
 * utility functions and classes used across WML code.
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

#ifndef _FOUNDRYUTILITIES_H_
#define _FOUNDRYUTILITIES_H_

#ifdef __GNUG__
# pragma interface
#endif

extern "C" {
#include <sys/stat.h>
#include <sys/types.h>
}

/*!
 * Character sets useful when calling FoundryUtilities::sanitize().
 *
 * These are ordered so that the most common chars appear earliest.
 */
//@{
#define CHARS_NUMERIC            "0123456789"
#define CHARS_ALPHA              "etaoinshrdlcumwfgypbvkjxqzETAOINSHRDLCUMWFGYPBVKJXQZ"
#define CHARS_ALPHALOWER         "etaoinshrdlcumwfgypbvkjxqz"
#define CHARS_ALPHAUPPER         "ETAOINSHRDLCUMWFGYPBVKJXQZ"
#define CHARS_NUMERIC_ALPHA      "etaoinshrdlcumwfgypbvkjxqz0123456789ETAOINSHRDLCUMWFGYPBVKJXQZ"
#define CHARS_NUMERIC_ALPHALOWER "etaoinshrdlcumwfgypbvkjxqz0123456789"
#define CHARS_NUMERIC_ALPHAUPPER "0123456789ETAOINSHRDLCUMWFGYPBVKJXQZ"
//@}

/*!
 * These are the chars which are acceptable for use in both unix, mac
 * AND windows file names. This doesn guarantee a safe Windows
 * filename, it imposes some extra conditions (no . at end of name,
 * some files such as NUL.txt AUX.txt disallowed).
 */
#define COMMON_FILE_SAFE_CHARS   CHARS_NUMERIC_ALPHA"_-.{}^[]`=,;"

/*!
 * Chars which are safe for IP domainnames
 */
#define IP_DOMAINNAME_SAFE_CHARS      CHARS_NUMERIC_ALPHA"-."

/*!
 * Chars which are safe for IP addresses
 */
#define IP_ADDRESS_SAFE_CHARS         CHARS_NUMERIC"."

/*!
 * CUPS would appear to be happy with any character under the sun
 * being in the title. However, we'll restrict it to these:
 */
#define WMLCUPS_TITLE_SAFE_CHARS      CHARS_NUMERIC_ALPHA"_{}^[]`=,;"

/*!
 * Internally, cups is happy with a very wide range of characters:
 */
#define CUPS_QUEUENAME_SAFE_CHARS     CHARS_NUMERIC_ALPHA"!\"$\%&'()*+,-.:;<=>?@[\\]^_{|}~"

/*!
 * To make our life a bit easier, we'll disallow a number of queuename
 * characters which would otherwise be acceptable.
 */
#define WMLCUPS_QUEUENAME_SAFE_CHARS  CHARS_NUMERIC_ALPHA"!\"$\%&'()*+,-.:;<=>?@[\\]^_{|}~"

/*!
 * Cups addresses are reckoned to be allowed to contain the same chars
 * as IP domain names.
 */
#define CUPS_ADDRESS_SAFE_CHARS       IP_DOMAINNAME_SAFE_CHARS

/*!
 * The Cups destination queue can only be numbers or letters.
 */
#define CUPS_DESTQUEUEPORT_SAFE_CHARS CHARS_NUMERIC_ALPHA

/*
 * FoundryUtilities code is available to C++ programs only.
 */
#ifdef __cplusplus

#include <vector>
#include <list>
#include <set>
#include <istream>
#include <ostream>
#include <iostream>
#include <fstream>

/*!
 * \brief The standard William Matthew Limited namespace.
 *
 * The standard William Matthew Limited namespace used for all WML code.
 */
namespace wml {

        /*!
         * \brief The type of script file being passed
         */
        enum SCRIPT_TYPE {SCRIPT_JAVASCRIPT, SCRIPT_CSS};

	/*!
	 * Allows the use of transform and tolower() on strings with
	 * GNU compiler
	 */
	class to_lower
	{
	public:
		/*!
		 * Apply lower case operation to the char c.
		 */
		char operator() (char c) const {
			return tolower(c);
		}
	};

	/*!
	 * Allows the use of transform and toupper() on strings with
	 * GNU compiler
	 */
	class to_upper
	{
	public:
		/*!
		 * Apply upper case operation to the char c.
		 */
		char operator() (char c) const {
			return toupper(c);
		}
	};

	/*!
	 * \brief A collection of utility functions
	 *
	 * The FoundryUtilities class provides numerous static utility
	 * functions.
	 */
	class FoundryUtilities
	{
	public:
		/*!
		 * Constructor
		 */
		FoundryUtilities();
		/*!
		 * Destructor
		 */
		~FoundryUtilities();

		/*!
		 * This removes all carriage return characters ('\\r'
		 * 0xd) from input. It will convert all DOS style
		 * newlines, which consist of '\\r''\\n' character duplets,
		 * to UNIX style newlines ('\\n'). A side effect is that
		 * any lone '\\r' characters which are present will be
		 * removed, whether or not they are followed by a '\\n'
		 * character.
		 */
		static int ensureUnixNewlines (std::string& input);

		/*!
		 * If the last character of input is a carriage return
		 * ('\\r' 0xd), then it is erased from input.
		 */
		static int stripTrailingCarriageReturn (std::string& input);

		/*!
		 * Erase trailing spaces from input. Return the
		 * number of spaces removed.
		 */
		static int stripTrailingSpaces (std::string& input);

		/*!
		 * Erase trailing chars c from input. Return the
		 * number of chars removed.
		 */
		static int stripTrailingChars (std::string& input, char c = ' ');

		/*!
		 * Erase trailing whitespace from input. Return the
		 * number of whitespace characters removed.
		 */
		static int stripTrailingWhitespace (std::string& input);

		/*!
		 * Erase leading spaces from input. Return the
		 * number of spaces removed.
		 */
		static int stripLeadingSpaces (std::string& input);

		/*!
		 * Erase any leading character c from input. Return the
		 * number of chars removed.
		 */
		static int stripLeadingChars (std::string& input, char c = ' ');

		/*!
		 * Erase leading whitespace from input. Return the
		 * number of whitespace characters removed.
		 */
		static int stripLeadingWhitespace (std::string& input);

		/*!
		 * Do a search and replace, search for searchTerm,
		 * replacing with replaceTerm. if replaceAll is true,
		 * replace all occurrences of searchTerm, otherwise
		 * just replace the first occurrence of searchTerm
		 * with replaceTerm.
		 */
		//@{
		static int searchReplace (const char* searchTerm,
					  const char* replaceTerm,
					  std::string& data,
					  bool replaceAll = true);
		static int searchReplace (std::string& searchTerm,
					  std::string& replaceTerm,
					  std::string& data,
					  bool replaceAll = true);
		//@}


		/*!
		 * Do a search and replace in the file fileName,
		 * search for searchTerm, replacing with
		 * replaceTerm. if replaceAll is true, replace all
		 * occurrences of searchTerm, otherwise just replace
		 * the first occurrence of searchTerm with
		 * replaceTerm.
		 */
		static int searchReplaceInFile (std::string searchTerm,
						std::string replaceTerm,
						std::string fileName,
						bool replaceAll = true);

		/*!
		 * Search for searchTerm in fileName. Any lines
		 * containing searchTerm should be deleted. If
		 * deleteEndOfLine is true, also remove the EOL NL or
		 * CRNL sequence, otherwise, leave the EOL in place.
		 */
		static int deleteLinesContaining (std::string searchTerm,
						  std::string fileName,
						  bool deleteEndOfLine = true);

		/*!
		 * Take the string str and condition it, so that it
		 * makes a valid XML tag, by replacing disallowed
		 * characters with '_' and making sure it doesn't
		 * start with a numeral.
		 */
		static void conditionAsXmlTag (std::string& str);

		/*!
		 * \brief Return the amount of RAM installed on the system.
		 *
		 * Returns RAM in bytes.
		 */
		static unsigned int getMemory (void);

		/*!
		 * \brief Return the amount of RAM used as cache.
		 *
		 * Returns cached RAM in bytes.
		 */
		static unsigned int getCachedMemory (void);

		/*!
		 * \brief Return the amount of RAM used as buffers.
		 *
		 * Returns buffer RAM in bytes.
		 */
		static unsigned int getBufferedMemory (void);

		/*!
		 * \brief Return the amount of active RAM.
		 *
		 * Returns active RAM in bytes.
		 */
		static unsigned int getActiveMemory (void);

		/*!
		 * \brief Return the amount of inactive RAM.
		 *
		 * Returns inactive RAM in bytes.
		 */
		static unsigned int getInactiveMemory (void);

		/*!
		 * Returns the system load average.
		 */
		static float getLoadAverage (void);

		/*!
		 * Get the uptime for the system in seconds.
		 */
		static float getUptime (void);

		/*!
		 * Return the amount of free space on the filesystem
		 * on which dirPath resides. The returned value is a
		 * human readable amount, with the KB/MB/GB suffix
		 * being automatically selected.
		 */
		static std::string freeSpace (std::string dirPath);

		/*!
		 * Return the amount of free space in KBytes on the
		 * filesystem on which dirPath resides.
		 */
		static UINT64_TYPE freeSpaceKBytes (std::string dirPath);

		/*!
		 * Return the total amount of space in KBytes on the
		 * filesystem on which dirPath resides.
		 */
		static UINT64_TYPE totalSpaceKBytes (std::string dirPath);

		/*!
		 * Stat a file, return true if the file exists and is
		 * any kind of file except a directory.
		 */
		//@{
		static bool fileExists (std::string& path);
		static bool fileExists (const char * path);
		//@}

		/*!
		 * Stat a file, return true if the file exists and is
		 * a regular file.  If file is a hanging symlink,
		 * fileExists returns false, if file is a symlink
		 * pointing to a regular file, fileExists returns
		 * true.
		 */
		static bool regfileExists (std::string& path);

		/*!
		 * Like regfileExists, but for block devices
		 */
		static bool blockdevExists (std::string& path);

		/*!
		 * Like regfileExists, but for sockets
		 */
		static bool socketExists (std::string& path);

		/*!
		 * Like regfileExists, but for fifos
		 */
		static bool fifoExists (std::string& path);

		/*!
		 * Like regfileExists, but for char devices
		 */
		static bool chardevExists (std::string& path);

		/*!
		 * Like lnkExists, but for char devices
		 */
		static bool linkExists (std::string& path);

		/*!
		 * Stat a directory, return true if the directory
		 * exists.
		 */
		//@{
		static bool dirExists (std::string& path);
		static bool dirExists (const char * path);
		//@}

		/*!
		 * Create the directory. Wrapper around mkdir(). If
		 * uid/gid is set to >-1, then chown each directory.
		 */
		static void createDir (std::string path,
				       mode_t mode = 0775,
				       int uid = -1, int gid = -1);

		/*!
		 * Set the permissions for the provided file
		 */
		static void setPermissions (std::string filepath,
					    mode_t mode);

		/*!
		 * Set the ownership for the provided file
		 */
		static void setOwnership (std::string filepath,
					  int uid = -1,
					  int gid = -1);

		/*!
		 * Touch the file.
		 */
		static void touchFile (std::string path);

		/*!
		 * Copy a file. If from/to is a string or a char*,
		 * then these are the filepaths. Some versions allow
		 * you to copy the file contents into an output
		 * stream. Throw exception on failure.
		 *
		 * The "from" file is expected to be a regular file -
		 * an exception will be thrown if this is not the
		 * case.
		 */
		//@{
		static void copyFile (std::string& from, std::string& to);
		static void copyFile (const char * from, const char * to);
		static void copyFile (std::string& from, std::ostream& to);
		static void copyFile (const char * from, std::ostream& to);
		static void copyFile (FILE* from, std::string to);
		static void copyFile (std::istream& from, std::string& to);
		static void copyFile (const char * from, FILE* to);
		//@}

		/*!
		 * Copy from one file pointer to another. Both are
		 * expected to be open, neither is closed after the
		 * copy.
		 */
		static void copyFile (FILE* from, FILE* to);

		/*!
		 * Copy a file from an input stream into a string.
		 */
		static void copyFileToString (std::istream& from, std::string& to);

		/*!
		 * Move a file. Throw exception on failure.
		 */
		static void moveFile (std::string from, std::string to);

		/*!
		 * Call unlink() on the given file path fpath. If
		 * unlinking fails, throw a descriptive error based on
		 * the errno which was set on unlink's return.
		 */
		static void unlinkFile (std::string fpath);

		/*!
		 * This reads the contents of a directory tree, making
		 * up a list of the contents in the vector vec. If the
		 * directory tree has subdirectories, these are
		 * reflected in the vector entries. So, a directory
		 * structure might lead to the following entries in vec:
		 *
		 * file1
		 * file2
		 * dir1/file1
		 * dir2/file1
		 * dir2/file2
		 *
		 * The base directory path baseDirPath should have NO
		 * TRAILING '/'. The subDirPath should have NO INITIAL
		 * '/' character.
		 *
		 * The subDirPath argument is present because this is
		 * a recursive function.
		 *
		 * If olderThanSeconds is passed in with a non-zero
		 * value, then only files older than olderThanSeconds
		 * will be returned.
		 */
		static void readDirectoryTree (std::vector<std::string>& vec,
					       const char* baseDirPath,
					       const char* subDirPath,
					       unsigned int olderThanSeconds = 0);

		/*!
		 * A simple wrapper around the more complex version,
		 * for the user to call.
		 *
		 * If olderThanSeconds is passed in with a non-zero
		 * value, then only files older than olderThanSeconds
		 * will be returned.
		 */
		static void readDirectoryTree (std::vector<std::string>& vec,
					       std::string dirPath,
					       unsigned int olderThanSeconds = 0);

		/*!
		 * Get a list of only the immediate subdirectories in
		 * dirPath.
		 *
		 * For example, if you have a structure like:
		 *
		 * file1
		 * file2
		 * dir1/file1
		 * dir2/file1
		 * dir2/file2
		 * dir2/aDirectory
		 *
		 * The set dset would be filled only with dir1,
		 * dir2.
		 */
		static void readDirectoryDirs (std::set<std::string>& dset,
					       std::string dirPath);

		/*!
		 * Return a datestamp - st_mtime; the file
		 * modification time for the given file.
		 */
		static std::string fileModDatestamp (const char* filename);

		/*!
		 * get an flock on the given file.
		 */
		static void getLock (int fd);

		/*!
		 * release an flock on the locked fd.
		 */
		static void releaseLock (int fd);

		/*!
		 * Replace '\' with '\\' so that str is suitable to be
		 * placed in a shell script.
		 */
		static void backSlashEscape (std::string& str);

		/*!
		 * Escape shell special chars in str with '\' chars so
		 * it is suitable to be placed in a shell script.
		 */
		static void slashEscape (std::string& str);
		//@}

		/*!
		 * Given a path like C:\\path\\to\\file in str, remove
		 * all the preceding C:\\ stuff to leave just the
		 * filename.
		 */
		static void stripDosPath (std::string& dosPath);

		/*!
		 * Given a path like /path/to/file in str, remove all
		 * the preceding /path/to/ stuff to leave just the
		 * filename.
		 */
		static void stripUnixPath (std::string& unixPath);

		/*!
		 * Generate a uuid random string as a temporary random
		 * filename. Pass in the path, and a prefix to
		 * identify the way the file is to be used. prefixPath
		 * might be, for example, "/tmp/xml-" and the
		 * resulting file could be:
		 * /tmp/xml-814b3393-e55a-449e-b16b-b5241497b532
		 */
		static std::string generateRandomFilename (const char* prefixPath);

		/*!
		 * Generate a uuid random string as a temporary random
		 * filename. Pass in the path, including a prefix to
		 * identify the way the file is to be used, and the
		 * number of characters of the uuid string to
		 * include. For example with prefixPath "/tmp/xml-"
		 * and numChars equal to 8, the resulting file could
		 * be: /tmp/xml-814b3393
		 */
		static std::string generateRandomFilename (const char* prefixPath,
							   unsigned int numChars);

		/*!
		 * This could be a template. Return true if v contains
		 * i.
		 */
		static bool vectorContains (std::vector<unsigned int>& v, unsigned int i);

		/*!
		 * Return position in v of first string which is equal
		 * to s, or -1 if it's not present.
		 */
		static int strVectorContains (std::vector<std::string>& v, std::string s);

		/*!
		 * Return position in v of first string which is a
		 * substring of s, or -1 if none is present.
		 */
		static int strVectorMatches (std::vector<std::string>& v, std::string s);

		/*!
		 * Return position in v of first entry which _doesn't_
		 * match s. Return -1 if all members of v match s.
		 */
		static int firstNotMatching (std::vector<std::string>& v, std::string s);

		/*!
		 * This could be a template. Return true if l contains
		 * i.
		 */
		static bool listContains (std::list<unsigned int>& l, unsigned int i);

		/*!
		 * This could be a template. Return true if l contains
		 * i.
		 */
		static bool listContains (std::list<std::string>& l, std::string& s);

		/*!
		 * Return true if the given pid is in the sleeping or
		 * running state. False otherwise. Determine this by
		 * reading /proc/[pid]/status and examinging the
		 * second line which will say something like:
		 * Status:\\tS (sleeping)\\n or Status:\\tR (running)
		 */
		static bool pidLoaded (int pid);

		/*!
		 * Get the command line for the given pid out of
		 * /proc/pid/cmdline.
		 */
		static std::string pidCmdline (int pid);

		/*!
		 * Populate a vector of directories in /proc
		 */
		static void readProcDirs (std::vector<std::string>& vec,
					  const char* baseDirPath,
					  const char* subDirPath);

		/*!
		 * \brief Get the first PID whose program name matches the
		 * argument programName.
		 *
		 * From all running processes, work out if any of the
		 * process named programName is running. Return the
		 * pid of the running process. If there is none,
		 * return -1;
		 *
		 * programName is just the executable file name,
		 * without the path, e.g. tail not /usr/bin/tail and
		 * so on. If the program name is more than 15 chars,
		 * use only the first 15 chars to look for a match.
		 *
		 * Opens all subdirectories in /proc which have only
		 * numerals in their name within these, gets the first
		 * line: Name: whatever If the name is programName,
		 * then get the Pid line, and return that.
		 */
		static int getPid (std::string& programName);

		/*!
		 * Get the mac address of the platform from eth0
		 *
		 * @return The mac address as a string (something like
		 * "aa:bb:cc:00:0a:0b")
		 */
		static std::string getMacAddr (void);

		/*!
		 * Get the mac address of the platform from eth0
		 *
		 * @param[out] mac A buffer of 2 * 32 bit unsigned
		 * ints in which to place the mac address
		 */
		static void getMacAddr (unsigned int* mac);

		/*!
		 * Get the mac address from the string @macStr
		 *
		 * @param[in] macStr A string representing the Mac address
		 * in the form "aa:bb:cc:11:22:33".
		 *
		 * @param[out] mac A buffer of 2 * 32 bit unsigned
		 * ints in which to place the mac address. mac[0]
		 * contains the lowest 4 nibbles of the mac address;
		 * in this example, it would contain
		 * 0xcc112233. mac[1] contains the top two nibbles -
		 * here it would be 0xaabb.
		 */
		static void strToMacAddr (std::string& macStr, unsigned int* mac);

		/*!
		 * Convert @mac, a buffer of 2 * 32 bit unsigned ints
		 * into a string representation in the form
		 * "aa:bb:cc:aa:bb:cc".
		 */
		static std::string macAddrToStr (unsigned int* mac);

		/*!
		 * Return the list of all possible alias addresses in
		 * a vector of strings.
		 */
		static std::vector<std::string> getAllAliases (void);

		/*!
		 * Read /etc/hostname to obtain the hostname of the
		 * running system.
		 */
		static std::string readHostname (void);

		/*!
		 * Return the current year.
		 */
		static unsigned int yearNow (void);

		/*!
		 * Return the current month (1==Jan, 12==Dec).
		 */
		static unsigned int monthNow (void);

		/*!
		 * Return the current date.
		 */
		static unsigned int dateNow (void);

		/*!
		 * Given the month as an int, where 1==Jan, 12==Dec,
		 * return the month as a string. If shortFormat is true,
		 * return "Jan", "Dec", etc., otherwise "January",
		 * "December" etc.
		 */
		static std::string monthStr (int month, bool shortFormat=false);

		/*!
		 * Give the number n, return the suitable (english)
		 * suffix. E.g. "st" for 1, "nd" for 22 etc.
		 */
		static std::string suffix (int n);

		/*!
		 * Convert a date of form 2009-02-16 to the unix epoch
		 * number. The fifth character of the string is
		 * examined, and if it is not a numeral, it is used as
		 * the separator. If the fifth character IS a numeral,
		 * then the date format is read in as YYYYMMDD.
		 */
		static time_t dateToNum (std::string& dateStr);

		/*!
		 * Convert a date/time of form 2009-02-16 14:34:34 to
		 * the unix epoch number. The fifth character of the
		 * string is examined, and if it is not a numeral, it
		 * is used as the date separator. If the fifth
		 * character IS a numeral, then the date format is
		 * read in as YYYYMMDD.
		 *
		 * The 3rd char after the space is read in and used as
		 * time separator
		 */
		static time_t dateTimeToNum (std::string dateTimeStr);

		/*!
		 * Convert a unix epoch number to a date/time of form
		 * 2009-02-16 02:03:01, using dateSeparator to delimit
		 * the date and timeSeparator to delimit the time.
		 */
		static std::string numToDateTime (time_t epochSeconds,
						  char dateSeparator = '\0',
						  char timeSeparator = '\0');

		/*!
		 * Convert a unix epoch number to a date of form
		 * 2009-02-16, using separator to delimit the date.
		 */
		static std::string numToDate (time_t epochSeconds,
					      char separator = '\0');

		/*!
		 * Return the current time in neat string format.
		 */
		static std::string timeNow (void);

		/*!
		 * Read the lineNum'th line of the syslog file and
		 * obtain the month, returning it. (1==Jan, 12==Dec).
		 */
		static unsigned int getMonthFromLog (std::string& filePath,
						     unsigned int lineNum);

                /*!
                 * Read a script file and output to rScript. This is a
                 * function to use when you want to read a script file
                 * and output it into the html, so it adds the relevant
                 *  opening and closing tags.
                 */
                static void getScript (SCRIPT_TYPE script,
				       std::stringstream& rScript,
				       std::string scriptFile,
				       bool inlineOutput = true);

                /*!
                 * Read a javascript file and output to
                 * rJavascript. This is a function to use when you
                 * want to read a javascript file and output it into
                 * the html, so it adds \<script type="text/javascript"\>
                 * at the start and \</script\> at the end.
                 */
                static void getJavascript (std::stringstream& rJavascript,
                                           std::string jsFile,
                                           bool inlineOutput = true);

                /*!
                 * Read a CSS file and output to
                 * rCSS. This is a function to use when you
                 * want to read a CSS file and output it into
                 * the html, so it adds \<style type="text/css"\>
                 * at the start and \</style\> at the end.
                 */
                static void getCSS (std::stringstream& rCSS,
                                    std::string cssFile,
                                    bool inlineOutput = true);

		/*!
		 * Take an ascii string and represent the characters
		 * as unicode utf8.
		 */
		static void unicodeize (std::string& str);

		/*!
		 * If str contains only numerals 0 to 9, return true,
		 * else return false.
		 */
		static bool containsOnlyNumerals (std::string& str);

		/*!
		 * A general purpose sanitization function. This is a
		 * "allow only a list of characters" sort of
		 * sanitiser, so you call it with a different list of
		 * characters in allowed, depending on whether you're
		 * sanitising for html or sql. This function could
		 * throw errors or simply erase disallowed chars. Try
		 * former behaviour, but offer a bool to switch. If
		 * eraseForbidden is true, then the function will
		 * remove forbidden chars from the string rather than
		 * throwing a runtime error.
		 */
		static void sanitize (std::string& str,
				      const char* allowed,
				      bool eraseForbidden = false);
		static void sanitize (std::string& str,
				      std::string& allowed,
				      bool eraseForbidden = false);
		/*!
		 * Modification of santize in which the offending
		 * characters are replaced with replaceChar.
		 */
		static void sanitizeReplace (std::string& str,
					     std::string allowed,
					     char replaceChar = '_');

		/*!
		 * Read filePath and output to stdout. Useful for
		 * outputting static html.
		 */
		static void coutFile (const char* filePath);
		static void coutFile (std::string filePath);

		/*!
		 * Get the size of the file in bytes.
		 */
		static int fileSize (std::string filePath);

		/*!
		 * Increment the count stored in the file given by
		 * filePath. Return the new value of the count, or -1
		 * on error.
		 */
		static int incFileCount (const char * filePath);

		/*!
		 * Increment the count stored in the file given by
		 * filePath. Return the new value of the count, or -1
		 * on error.
		 */
		static int decFileCount (const char * filePath);

		/*!
		 * Zero the file count in filePath, ENSURING that the
		 * file access time is reset to the current time. For
		 * this reason, the file is first unlinked, then
		 * re-created, then "0" is inserted into it. Return
		 * the new value of the count (0), or -1 on error.
		 */
		static int zeroFileCount (const char * filePath);

		/*!
		 * split csv into a vector
		 */
		static std::vector<std::string> csvToVector (std::string& csvList,
							     char separator = ',',
							     bool ignoreTrailingEmptyVal = true);

		/*!
		 * Split a string of values into a vector using the
		 * separator string (not char) passed in as
		 * "separator". If ignoreTrailingEmptyVal is true,
		 * then a trailing separator with nothing after it
		 * will NOT cause an additional empty value in the
		 * returned vector.
		 */
		static std::vector<std::string> stringToVector (std::string& s, std::string& separator,
								bool ignoreTrailingEmptyVal = true);

		/*!
		 * Turn the passed in vector of string values into a
		 * string separated by the separator string (not char)
		 * passed in as "separator".
		 */
		static std::string vectorToString (std::vector<std::string>& s, std::string& separator);


		/*!
		 * split csv into a list
		 */
		static std::list<std::string> csvToList (std::string& csvList,
							 char separator = ',');
		/*!
		 * split csv into a set
		 */
		static std::set<std::string> csvToSet (std::string& csvList,
						       char separator = ',');

		/*!
		 * Output a vector of strings as a csv string.
		 */
		static std::string vectorToCsv (std::vector<std::string>& vecList,
						char separator = ',');

		/*!
		 * Output a list of strings as a csv string.
		 */
		static std::string listToCsv (std::list<std::string>& listList,
					      char separator = ',');

		/*!
		 * Output a set of strings as a csv string.
		 */
		static std::string setToCsv (std::set<std::string>& listList,
					     char separator = ',');

                /*!
                 * Converts pdf files to a ghostscript device
                 * specified. If wait is true, then the function will
                 * block until the conversion is complete. Otherwise,
                 * the forked process will be left to run and the
                 * function returns immediately.
                 */
                static void pdfConversion (std::string inputPath, std::string outputDevice,
                                           std::string outputPath, unsigned int width,
                                           unsigned int height, bool wait = false,
					   unsigned int resolution = 0);

                /*!
                 * Creates jpeg(s) from a pdf file. To create a jpeg for each page
                 * use %d in the output path in place of the page number.
		 *
		 * If wait is true, then the function will block until
                 * the conversion is complete. Otherwise, the forked
                 * process will be left to run and the function
                 * returns immediately.
                 */
                static void pdfToJpeg (std::string inputPath, std::string outputPath,
                                       unsigned int width, unsigned int height,
				       bool wait = false, unsigned int resolution = 0);

                /*!
                 * Creates png(s) from a pdf file. To create a png for each page
                 * use %d in the output path in place of the page number. The png(s)
                 * contain an alpha channel. PDF 1.4 transparent files do not give a
                 * transparent background with this device. Text and graphics anti-aliasing
                 * are enabled by default.
		 *
		 * If wait is true, then the function will block until
                 * the conversion is complete. Otherwise, the forked
                 * process will be left to run and the function
                 * returns immediately.
                 */
                static void pdfToPng (std::string inputPath, std::string outputPath,
				      unsigned int width, unsigned int height,
				      bool wait = false, unsigned int resolution = 0);

		/*!
		 * A wrapper around the iconv() call from glibc.
		 *
		 * @param fromEncoding The iconv-style encoding
		 * specifier from which the string needs to be
		 * transcoded. You can get a list of possible values
		 * using iconv --list on any normal system. Examples
		 * might be UTF-8, UTF-16BE, ISO8859-1
		 *
		 * @param toEncoding The iconv-style encoding specifer
		 * into which the string should be converted.
		 *
		 * @param fromString The string in its existing format.
		 *
		 * @param toString A container into which the
		 * transcoded string will be placed.
		 */
		static void doIconv (const char * fromEncoding,
				     const char * toEncoding,
				     std::string& fromString,
				     std::string& toString);

		/*!
		 * Opens (or re-opens) the referenced filestream in the
		 * correct state for overwriting.
		 */
		static void openFilestreamForOverwrite (std::fstream& f,
							const char * filepath);

		/*!
		 * Opens (or re-opens) the referenced filestream in the
		 * correct state for overwriting.
		 */
		static void openFilestreamForOverwrite (std::fstream& f,
							const std::string& filepath);

		/*!
		 * Opens (or re-opens) the referenced filestream in the
		 * correct state for appending.
		 */
		static void openFilestreamForAppend (std::fstream& f,
						     const char * filepath);

		/*!
		 * Opens (or re-opens) the referenced filestream in the
		 * correct state for appending.
		 */
		static void openFilestreamForAppend (std::fstream& f,
						     const std::string& filepath);

		/*!
		 * Close the filestream if necessary
		 */
		static void closeFilestream (std::fstream& f);

		/*!
		 * Clear the filestream flags if necessary
		 */
		static void clearFilestreamFlags (std::fstream& f);

		/*!
		 * If /tmp/messages has grown larger than megabytes
		 * MB, then truncate it to 0.
		 */
		static void check_tmp_messages (int megabytes);

		/*!
		 * Determine if ip_string is a valid IP
		 * address. Return true if so, false if not.
		 *
		 * This function copied from
		 * wmlnetapui/wmlnetapui/SystemPage.cpp
		 */
		static bool valid_ip (std::string ip_string);

		/*!
		 * Split a string into a set of strings using
		 * the delimiter specified. Unlike most string split
		 * functions, the delimiter in this can be more than
		 * one character in length
		 */
		static void splitString (std::vector<std::string>& tokens,
					 std::string& stringToSplit,
					 std::string& delim);
	};

} // namespace wml

#endif // __cplusplus

#endif // _FOUNDRYUTILITIES_H_
