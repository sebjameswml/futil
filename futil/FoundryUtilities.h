/* -*-c++-*- */
/*
 * A class of utility functions
 */

#ifndef _FOUNDRYUTILITIES_H_
#define _FOUNDRYUTILITIES_H_

#ifdef __GNUG__
# pragma interface
#endif

/*! \file FoundryUtilities.h
 * \brief Declares the class \c FoundryUtilities
 *
 * \c FoundryUtilities contains some generally useful static member functions.
 */

namespace foundryWebUI {


	/*!
	 * Class to allow use of transform and tolower() on strings with GNU compiler
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
	 * Class to allow use of transform and toupper() on strings with GNU compiler
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

	/*! \class foundryutilities foundryutilities.h foundryWebUI/foundryutilities.h 
	 * \brief Class that deals with authorizing access to a foundryWebUI system
	 *
	 * The \c foundryutilities class provides methods to generate a password entry box, authenticate
	 * the password and to check sessions. An object of this class is required
	 * for every access of a foundryWebUI system. 
	 */

	class FoundryUtilities
	{
	public:
		/*! \name Constructor and Destructor */
		//@{
		FoundryUtilities();
		~FoundryUtilities();
		//@}

		/*! Public utility functions */
		//@{

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
		 * \brief Return the amount of RAM
		 *
		 * Returns RAM in bytes.
		 */
		static unsigned int getMemory (void);

		/*!
		 * Stat a file, return true if the file exists and is
		 * a regular file.
		 */
		//@{
		static bool fileExists (std::string& path);
		static bool fileExists (const char * path);
		//@}

		/*!
		 * Return a datestamp - st_mtime; the file
		 * modification time for the given file.
		 */
		static std::string fileModDatestamp (const char* filename);

		/*!
		 * get an flock on the given file.
		 */
		static void getLock (FILE * f);

		/*!
		 * release an flock on the given file.
		 */
		static void releaseLock (FILE * f);

		/*!
		 * Obtain a "WMLPP Platform lock"
		 */
		static bool getWmlppLock (void);

		/*!
		 * Release a "WMLPP Platform lock"
		 */
		static void releaseWmlppLock (void);

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
		 * Return true if the given pid is in the sleeping or
		 * running state. False otherwise. Determine this by
		 * reading /proc/[pid]/status and examinging the
		 * second line which will say something like:
		 * Status:\\tS (sleeping)\\n or Status:\\tR (running)
		 */
		static bool pidLoaded (int pid);

		/*!
		 * Get the mac address of the platform
		 */
		static std::string getMacAddr (void);

		/*!
		 * Return the list of all possible alias addresses in
		 * a vector of strings.
		 */
		static std::vector<std::string> getAllAliases (void);

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
		 */
		static void readDirectoryTree (std::vector<std::string>& vec,
					       const char* baseDirPath,
					       const char* subDirPath);

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
		 * Read the lineNum'th line of the syslog file and
		 * obtain the month, returning it. (1==Jan, 12==Dec).
		 */
		static unsigned int getMonthFromLog (std::string& filePath,
						     unsigned int lineNum);

		/*!
		 * Read a javascript file and output to
		 * rJavascript. This is a function to use when you
		 * want to read a javascript file and output it into
		 * the html, so it adds \<script type="text/javascript"\>
		 * at the start and \</script\> at the end.
		 */
		static void getJavascript (std::stringstream& rJavascript,
					   std::string jsFile);

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
	};

} // namespace foundryWebUI

#endif // _FOUNDRYUTILITIES_H_
