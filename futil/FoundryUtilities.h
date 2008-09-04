/* -*-c++-*- */
/*
 * A class of utility functions
 */

#ifndef _FOUNDRYUTILITIES_H_
#define _FOUNDRYUTILITIES_H_

#ifdef __GNUG__
# pragma interface
#endif

#include "config.h"

/* The WML Base 52 definitions */
#define WMLBASE52_CHAR00 'a'
#define WMLBASE52_CHAR01 'b'
#define WMLBASE52_CHAR02 'c'
#define WMLBASE52_CHAR03 'd'
#define WMLBASE52_CHAR04 'e'
#define WMLBASE52_CHAR05 'f'
#define WMLBASE52_CHAR06 'g'
#define WMLBASE52_CHAR07 'h'
#define WMLBASE52_CHAR08 'i'
#define WMLBASE52_CHAR09 'j'
#define WMLBASE52_CHAR10 'k'
#define WMLBASE52_CHAR11 'l'
#define WMLBASE52_CHAR12 'm'
#define WMLBASE52_CHAR13 'n'
#define WMLBASE52_CHAR14 'o'
#define WMLBASE52_CHAR15 'p'
#define WMLBASE52_CHAR16 'q'
#define WMLBASE52_CHAR17 'r'
#define WMLBASE52_CHAR18 's'
#define WMLBASE52_CHAR19 't'
#define WMLBASE52_CHAR20 'u'
#define WMLBASE52_CHAR21 'v'
#define WMLBASE52_CHAR22 'w'
#define WMLBASE52_CHAR23 'x'
#define WMLBASE52_CHAR24 'y'
#define WMLBASE52_CHAR25 'z'
#define WMLBASE52_CHAR26 'A'
#define WMLBASE52_CHAR27 'B'
#define WMLBASE52_CHAR28 'C'
#define WMLBASE52_CHAR29 'D'
#define WMLBASE52_CHAR30 'E'
#define WMLBASE52_CHAR31 'F'
#define WMLBASE52_CHAR32 'G'
#define WMLBASE52_CHAR33 'H'
#define WMLBASE52_CHAR34 'I'
#define WMLBASE52_CHAR35 'J'
#define WMLBASE52_CHAR36 'K'
#define WMLBASE52_CHAR37 'L'
#define WMLBASE52_CHAR38 'M'
#define WMLBASE52_CHAR39 'N'
#define WMLBASE52_CHAR40 'O'
#define WMLBASE52_CHAR41 'P'
#define WMLBASE52_CHAR42 'Q'
#define WMLBASE52_CHAR43 'R'
#define WMLBASE52_CHAR44 'S'
#define WMLBASE52_CHAR45 'T'
#define WMLBASE52_CHAR46 'U'
#define WMLBASE52_CHAR47 'V'
#define WMLBASE52_CHAR48 'W'
#define WMLBASE52_CHAR49 'X'
#define WMLBASE52_CHAR50 'Y'
#define WMLBASE52_CHAR51 'Z'

/*
 * First come functions which may be #included by C code.
 */
#ifdef __cplusplus
extern "C" {
#endif
	int emailMsg (const char *subject,
		      const char *message);
	int emailMsgPlusAttachment (const char *subject,
				    const char *message,
				    const char *attachfile);
	int get_system_mac (unsigned int * mac);
#ifdef __cplusplus
}
#endif


/*
 * Remaining code is available to C++ programs only.
 */
#ifdef __cplusplus

#include <vector>

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
		static void getLock (int fd);

		/*!
		 * release an flock on the locked fd.
		 */
		static void releaseLock (int fd);

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

		/*!
		 * Read filePath and output to stdout. Useful for
		 * outputting static html.
		 */
		static void coutFile (const char* filePath);

		/*!
		 * A group of functions to carry out baseN conversions.
		 */
		//@{

		/*!
		 * Return the zero character for the given base. For
		 * decimal or hex, it returns '0', for WML Base 52,
		 * WMLBASE52_CHAR00, and throws an exception if it
		 * doesn't know the representation character for zero
		 * in the given base.
		 *
		 * @param[in] base The base, or radix for which the
		 * zero character is required.
		 * @return The zero character for the base @base.
		 */
		static char zeroChar (int base);

		/*!
		 * Add the number n to the string str, represented in
		 * the given base.
		 *
		 * @param[in] n The number of add to the string @str.
		 * @param[out] str The string to which the number @n
		 * should be appended.
		 * @param[in] base The base or radix in which the 
		 * number should be represented when added to string @str.
		 */
		static void addChar (int n, std::string& str, int base);

		/*!
		 * Convert the character c, which is represented with
		 * the radix base into an integer, which is returned.
		 *
		 * @param[in] c The character to convert into an int.
		 * @param[in] base The radix in which the character
		 * @c represents the number.
		 * @return The number @c as an integer.
		 */
		static int baseNToInt (char c, int base);

		/*!
		 * Convert the string @str into a number, placing it
		 * into the array @num, which is @numlen in length.
		 *
		 * Separate hex function because hex can be done faster than an
		 * arbitrary base because there are 8 hex numerals ("4 bit nibbles")
		 * per 32 bit uint32.
		 */
		static void hexToUint32s (const std::string& str,
					  UINT32_TYPE * num,
					  UINT32_TYPE numlen);

		/*!
		 * Convert the string @str, represented in base @base
		 * into a number, placing it into the array @num,
		 * which is @numlen in length.
		 */
		static void baseNToUint32s (const std::string& str,
					    UINT32_TYPE * num,
					    UINT32_TYPE numlen,
					    int base);
	
		//@}
	};

} // namespace foundryWebUI

#endif // __cplusplus

#endif // _FOUNDRYUTILITIES_H_
